import can
import socket
import struct
import subprocess
import threading
from config import Config
from utils.logger import SingletonLogger
from utils.logger import *

can_lock = threading.Lock()

logger_singleton = SingletonLogger('logger.log')
logger = logger_singleton.logger
logger_frame = logger_singleton.logger_frame

@logger_frame
class CanBridge:
    def __init__(self):
        """
        Initialize CanBridge module
        :param mode: 'test' for direct binding to vcan1, 'release' for using the CAN bridge
        """
        self.CAN_INTERFACE = 'vcan1'
        self.mode = 'release'
        self.bus = None
        self.setup_bus()

    def setup_bus(self):
        """Initialize the CAN bus based on the configuration."""
        if self.bus is None:
            try:
                self.bus = can.interface.Bus(channel=self.CAN_INTERFACE, bustype='socketcan')
            except Exception as e:
                logger.error(f"Failed to initialize CAN bus: {e}")
                raise RuntimeError(f"Failed to initialize CAN bus: {e}")

    def send_frame(self, id, data):
        with can_lock:
            message = can.Message(arbitration_id=id, data=data, is_extended_id=True)
            try:
                self.bus.send(message)
            except can.CanError as e:
                logger.error(f"Message not sent: {e}")

    def setup_vcan_interface(self):
        """Runs the bash script to set up vcan1."""
        bash_script = """
        #!/bin/bash
        if ! ip link show vcan1 > /dev/null 2>&1; then
            echo "Creating vcan1 interface..."
            sudo modprobe vcan
            sudo ip link add dev vcan1 type vcan
            sudo ip link set up vcan1
        else
            echo "vcan1 interface already exists."
        fi
        """
        subprocess.run(bash_script, shell=True, check=True)

    def can_to_udp(self):
        """Bridge CAN to UDP in release mode."""
        if self.mode != 'release':
            raise RuntimeError("CAN to UDP bridge is available only in release mode.")

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

        while True:
            message = self.bus.recv()

            can_id = message.arbitration_id
            data = message.data.ljust(8, b'\x00')  # Ensure data is 8 bytes
            packet_data = struct.pack('I8s', can_id, data)

            sock.sendto(packet_data, ('192.168.241.255', 5000))

    def udp_to_can(self):
        """Bridge UDP to CAN in release mode."""
        if self.mode != 'release':
            raise RuntimeError("UDP to CAN bridge is available only in release mode.")

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(('0.0.0.0', 5000))

        while True:
            packed_data, _ = sock.recvfrom(12)
            if len(packed_data) > 0:
                can_id, data = struct.unpack('I8s', packed_data)
                data = data.rstrip(b'\x00')
                self.send_frame(can_id, data)


    def get_bus(self):
        """Expose the bus instance for external use."""
        if self.bus is None:
            self.bus = can.interface.Bus(channel=self.CAN_INTERFACE, bustype='socketcan')
        return self.bus

    def run(self):
        """Run the appropriate function based on the mode."""
        t1 = threading.Thread(target=self.udp_to_can)
        t2 = threading.Thread(target=self.can_to_udp)

        t1.start()
        t2.start()

        t1.join()
        t2.join()

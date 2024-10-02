import can
import socket
import struct
import subprocess
import threading
from config import Config


class CanBridge:
    def __init__(self):
        """
        Initialize CanBridge module
        :param mode: 'test' for direct binding to vcan1, 'release' for using the CAN bridge
        """
        self.CAN_INTERFACE = Config.CAN_CHANNEL
        self.mode = Config.mode
        self.bus = None

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

    def bind_vcan(self):
        """Bind directly to the vcan1 interface in test mode."""
        if self.mode == 'test':
            self.bus = can.interface.Bus(channel=self.CAN_INTERFACE, bustype='socketcan')
            print(f"Bound directly to {self.CAN_INTERFACE} in test mode.")
        else:
            raise RuntimeError("Bind method is available only in test mode.")

    def can_to_udp(self):
        """Bridge CAN to UDP in release mode."""
        if self.mode != 'release':
            raise RuntimeError("CAN to UDP bridge is available only in release mode.")

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

        while True:
            message = self.bus.recv()
            can_id = message.arbitration_id
            data = message.data.ljust(8, b'\x00')  # Ensure data is 8 bytes

            packet_data = struct.pack('I8s', can_id, data)
            sock.sendto(packet_data, (Config.UDP_IP, Config.UDP_CAN_TO_UDP_PORT))

    def udp_to_can(self):
        """Bridge UDP to CAN in release mode."""
        if self.mode != 'release':
            raise RuntimeError("UDP to CAN bridge is available only in release mode.")

        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind(('0.0.0.0', Config.UDP_TO_CAN_PORT))

        while True:
            packed_data, _ = sock.recvfrom(12)  # 4 bytes for CAN ID, 8 bytes for data
            if len(packed_data) > 0:
                can_id, data = struct.unpack('I8s', packed_data)
                data = data.rstrip(b'\x00')  # Remove padding bytes
                self.send_frame(can_id, data)

    def send_frame(self, can_id, data):
        """Send CAN frame."""
        message = can.Message(
            arbitration_id=can_id,
            data=data,
            is_extended_id=True
        )
        self.bus.send(message)

    def get_bus(self):
        """Expose the bus instance for external use."""
        if self.bus is None:
            raise RuntimeError("The CAN bus is not initialized yet.")
        return self.bus

    def run(self):
        """Run the appropriate function based on the mode."""
        self.setup_vcan_interface()
        if self.mode == 'test':
            self.bind_vcan()
        elif self.mode == 'release':
            self.bus = can.interface.Bus(channel=self.CAN_INTERFACE, bustype='socketcan')
            t1 = threading.Thread(target=self.udp_to_can)
            t2 = threading.Thread(target=self.can_to_udp)

            t1.start()
            t2.start()

            t1.join()
            t2.join()
        else:
            raise ValueError(f"Unknown mode: {self.mode}")

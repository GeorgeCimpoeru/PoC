#!/usr/bin/env python3
import can
import socket
import struct
import threading
import os


def can_to_udp(bus, number_interface: int):
    UDP_IP = "192.168.241.255"  # Broadcast address for the subnet
    UDP_PORT = 5000
    # UDP_IP = "192.168.241.106"
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BINDTODEVICE, b'eth0')

    while True:
        message = bus.recv()
        can_id = message.arbitration_id
        data = message.data.ljust(8, b'\x00')  # Ensure data is 8 bytes

        packet_data = struct.pack('II8s', number_interface, can_id, data)
        sock.sendto(packet_data, (UDP_IP, UDP_PORT))


def udp_to_can(list_of_bus):
    UDP_IP = "0.0.0.0"  # Listen on all interfaces
    UDP_PORT = 5000

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    while True:
        packed_data, _ = sock.recvfrom(16)  # CAN data is usually up to 8 bytes
        if len(packed_data) > 0:
            number_interface, can_id, data = struct.unpack('II8s', packed_data)
            data = data.rstrip(b'\x00')  # Remove padding bytes
            send_frame(list_of_bus[number_interface], can_id, data)


def send_frame(bus, can_id, data):
    message = can.Message(
        arbitration_id=can_id,
        data=data,
        is_extended_id=True
    )
    bus.send(message)


def main():
    output = os.popen("netstat -i | grep \"vcan*\" | awk '{print $1}'").read()
    new_line = output.find('\n')
    if new_line != -1:
        if new_line + 1 == len(output):
            CAN_INTERFACE = output[0:new_line]
            bus0 = can.interface.Bus(channel=CAN_INTERFACE, bustype='socketcan')
            list_of_bus = [bus0]
            t1 = threading.Thread(target=udp_to_can, args=(list_of_bus,))
            t2 = threading.Thread(target=can_to_udp, args=(bus0, 0,))

            t1.start()
            t2.start()

            t1.join()
            t2.join()
        else:
            CAN_INTERFACE1 = output[0:new_line]
            CAN_INTERFACE2 = output[new_line+1:-1]

            bus0 = can.interface.Bus(channel=CAN_INTERFACE1, bustype='socketcan')
            bus1 = can.interface.Bus(channel=CAN_INTERFACE2, bustype='socketcan')
            list_of_bus = [bus0, bus1]
            t1 = threading.Thread(target=udp_to_can, args=(list_of_bus,))
            t2 = threading.Thread(target=can_to_udp, args=(bus0, 0,))
            t3 = threading.Thread(target=can_to_udp, args=(bus1, 1,))

            t1.start()
            t2.start()
            t3.start()

            t1.join()
            t2.join()
            t3.join()

    else:
        print("Not found any vcan interface")


if __name__ == "__main__":
    main()

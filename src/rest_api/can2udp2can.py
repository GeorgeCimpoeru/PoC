#!/usr/bin/env python3
import can
import socket
import struct
import threading
import os


def can_to_udp(bus, number_interface: int):
    UDP_IP = "192.168.241.255" # Broadcast address for the subnet
    UDP_PORT = 5000
    #UDP_IP = "192.168.241.106"
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    #sock.setsockopt(socket.SOL_SOCKET, socket.SO_BINDTODEVICE, b'eth0')

    while True:
        message = bus.recv()
        can_id = message.arbitration_id
        data = message.data.ljust(8, b'\x00')  # Ensure data is 8 bytes

        packet_data = struct.pack('II8s', number_interface, can_id, data)
        sock.sendto(packet_data, (UDP_IP, UDP_PORT))

def udp_to_can(list_of_bus, my_ip):
    UDP_IP = "0.0.0.0"  # Listen on all interfaces
    UDP_PORT = 5000

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT))

    while True:
        packed_data, addr = sock.recvfrom(16)  # CAN data is usually up to 8 bytes
        sender_ip = addr[0];
        
        # Ignore packets coming from your own device
        if sender_ip == my_ip:
            continue
            
        if len(packed_data) > 0:
            number_interface, can_id, data = struct.unpack('II8s', packed_data)
            receiver_id = can_id & 0xFF
            if receiver_id == 0xFA and number_interface == 1:
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
    # Get the IP adress of the local device
    my_ip = os.popen("hostname -I").read().strip().split()[0]

    output = os.popen("netstat -i | grep \"vcan*\" | awk '{print $1}'").read()
    new_line = output.find('\n')
    if new_line != -1:
        if new_line + 1 == len(output):
            CAN_INTERFACE = output[0:new_line]
            bus0 = can.interface.Bus(channel=CAN_INTERFACE, interface='socketcan')
            interface_number = None
            if CAN_INTERFACE == "vcan0":
            	list_of_bus = [bus0, None]
            	interface_number = 0
            else:
            	list_of_bus = [None, bus0]
            	interface_number = 1
            t1 = threading.Thread(target=udp_to_can, args=(list_of_bus, my_ip))
            t2 = threading.Thread(target=can_to_udp, args=(bus0, interface_number,))

            t1.start()
            t2.start()
            

            t1.join()
            t2.join()
        else:
            CAN_INTERFACE1 = output[0:new_line]
            CAN_INTERFACE2 = output[new_line+1:-1]
            
            bus0 = can.interface.Bus(channel=CAN_INTERFACE1, interface='socketcan')
            bus1 = can.interface.Bus(channel=CAN_INTERFACE2, interface='socketcan')
            list_of_bus = [bus0,bus1]
            t1 = threading.Thread(target=udp_to_can, args=(list_of_bus, my_ip))
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

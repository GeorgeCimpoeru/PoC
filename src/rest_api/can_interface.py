import can
import os

def initialize_can_interface(interface, virtual):
    if virtual:
        # Ensure the virtual CAN interface is set up
        setup_vcan_interface(interface)
        bus = can.interface.Bus(channel=interface, bustype='socketcan')
    else:
        bus = can.interface.Bus(channel=interface, bustype='socketcan')
    return bus

def setup_vcan_interface(interface):
    try:
        # Try to initialize the bus to see if the interface exists
        bus = can.interface.Bus(channel=interface, bustype='socketcan')
    except OSError:
        # If the interface does not exist, set it up
        os.system(f'sudo modprobe {interface}')
        os.system(f'sudo ip link add dev {interface} type vcan')
        os.system(f'sudo ip link set up {interface}')
        print(f"Virtual CAN interface {interface} created and set up")

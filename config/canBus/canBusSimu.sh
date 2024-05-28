#!/bin/bash

        ### CONFIGS ###

# Add module for Kernel
sudo modprobe can-gw

        ### GLOBAL VARIABLES ###

# Maximum number of expected modules
interfaces_no=2

# Array that holds the name of the interfaces that have been linked already
interfaces=()

        ### FUNCTIONS ###

# check_status - checks the status of an interface and returns 1 if UP and 0 if DOWN ;
#              - Argument 1: the name of an interface (ex: check_status vcan0)
check_status() {
    status=$(sudo ip link show | grep $1 | awk '{print $9}')

    if [ $status != "DOWN" ]; then
        echo "1"
    else
        echo "0"
    fi
}

# simu_canbus - simulate CAN Bus between two given interfaces ; the connection is created both ways
#             - Argument 1: name of the first interface to be linked
#             - Argument 2: name of the second interface to be linked (ex: simu_canbus vcan0 vcan1)
simu_canbus() {
    sudo cangw -A -s $1 -d $2 -e
    sudo cangw -A -s $2 -d $1 -e
}

#link_new_interface - simulate a CAN Bus between a new interface and the ones already linked(held in interfaces)
#                   - if interfaces is empty, add the new interface
#                   - if it is not empty and does not contain the given interface name, link it with others and add it
#                   - Argument 1: name of the new interface (ex: link_new_interface vcan1)
link_new_interface() {
    if [ ${#interfaces[@]} -eq 0 ]; then
        interfaces+=($1)
    elif ! [[ ${interfaces[@]} =~ $1 ]];then
        for interface in ${interfaces[@]}; do
            simu_canbus $interface $1
        done
        interfaces+=($1)
    fi
}

#syncronize - loop through the interfaces that are UP and simulate the CAN Bus between the new ones and already existing interfaces
syncronize() {
    for interface in $(ip link show | grep -o 'vcan[0-9]*'); do
        if [ $(check_status $interface) -eq 1 ]; then
            link_new_interface $interface
        fi
    done
}

#the infinite loop runs every 5 seconds and if the linked_interfaces array size is smaller than the expected number of interfaces, it checks for
#new interfaces
while :; do
        if [ ${#interfaces[@]} -lt $interfaces_no ]; then
            syncronize
        fi
    
        sleep 5
done

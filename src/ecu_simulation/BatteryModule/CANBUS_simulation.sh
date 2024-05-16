#!/bin/bash

function findCANinterfaces()
{
    #Wait until a vcan interface is found
    while true; do
        lst_int=$(ifconfig | grep vcan* | cut -d ':' -f 1)
        for itf in ${lst_int[@]}; do
            if [[ $itf = "vcan"* ]]; then
                echo "Found $itf as CAN interface"
                return;
            fi
        done
        sleep 10
    done
}

function createRule()
{
    findCANinterfaces
    echo "Waiting 5 sec to read more interfaces"
    sleep 5
    lst_int=$(ifconfig | grep vcan* | cut -d ':' -f 1)

    #Enable can-gw kernel module
    sudo modprobe can-gw 
    #Create rules between interfaces
    for itf in ${lst_int[@]}; do
        echo "Found $itf"
        for itf2 in ${lst_int[@]}; do
            if [[ $itf != $itf2 ]] ; then
                cangw -A -s $itf -d $itf2 -e
            fi
        done
    done
}

createRule

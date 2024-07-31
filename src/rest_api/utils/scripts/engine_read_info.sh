#!/bin/bash

# Authentication
cansend vcan1 000010FA#0467010202
sleep 2
cansend vcan1 000010FA#026702
sleep 1

# Send CAN frames for reading engine information
cansend vcan1 000012FA#0462101021
sleep 1
cansend vcan1 000012FA#0462101022
sleep 1
cansend vcan1 000012FA#0462101023
sleep 1
cansend vcan1 000012FA#0462101024
sleep 1
cansend vcan1 000012FA#0462101025
sleep 1
cansend vcan1 000012FA#0462101026
sleep 1
cansend vcan1 000012FA#0462101027
sleep 1
cansend vcan1 000012FA#0462101028
sleep 1
cansend vcan1 000012FA#0462101029
sleep 1
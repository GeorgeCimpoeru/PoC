#!/bin/bash

# Send CAN frames for reading engine information
cansend vcan0 023#0462101024
sleep 1
cansend vcan0 023#025003
sleep 1
cansend vcan0 023#0141
sleep 1
cansend vcan0 023#0142
sleep 1
cansend vcan0 023#0143
sleep 1
cansend vcan0 023#0144
sleep 1
cansend vcan0 023#0145
sleep 1
cansend vcan0 023#0146
sleep 1
cansend vcan0 023#0147
sleep 1
cansend vcan0 023#0148
sleep 1
cansend vcan0 023#0149
sleep 1

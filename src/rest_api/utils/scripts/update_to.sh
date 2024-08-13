#!/bin/bash

# Verify version
cansend vcan1 000011FA#0462F1AD
sleep 1

# Session Control
cansend vcan1 000011FA#025002
sleep 1

# Auth: Seed
cansend vcan1 000010FA#0467010202
sleep 2
# Auth: Key
cansend vcan1 000010FA#026702
sleep 1

cansend vcan1 000011FA#027401
sleep 1
cansend vcan1 000011FA#027601
sleep 1
cansend vcan1 000011FA#027701
sleep 1
cansend vcan1 000011FA#025002
sleep 1
cansend vcan1 000011FA#025102
sleep 1
# cansend vcan1 000011FA#055901111104 # 4 errors
# sleep 1
cansend vcan1 000011FA#055901111100 # no errors
sleep 1
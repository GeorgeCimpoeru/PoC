#!/bin/bash

cansend vcan1 000001FA#0462101012
sleep 1
cansend vcan1 000001FA#025002
sleep 1
cansend vcan1 000001FA#0467010202
sleep 1
cansend vcan1 000001FA#026702
sleep 1
cansend vcan1 000001FA#027401
sleep 1
cansend vcan1 000001FA#027601
sleep 1
cansend vcan1 000001FA#027701
sleep 1
cansend vcan1 000001FA#025002
sleep 1
cansend vcan1 000001FA#025102
sleep 1
# cansend vcan1 000001FA#055901111104 # 4 errors
# sleep 1
cansend vcan1 000001FA#055901111100 # no errors
sleep 1
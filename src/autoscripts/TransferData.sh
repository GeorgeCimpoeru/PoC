#!/bin/bash

# mcu_data file used for checking ota status
file_path="/home/projectx/accademyprojects/PoC/src/mcu/mcu_data.txt"

idx="01"

while true; do
    # Read the value from the file (second field after '01E0')
    file_value=$(awk '{print $2}' "$file_path")

    if [ "$file_value" == "31" ]; then
        echo "PROCESSING_TRANSFER_COMPLETE, exiting loop."
        break
    fi

    cansend vcan1 0000fa11#0236${idx}

    # Convert the current idx to decimal, increment by 1
    dec_value=$((0x$idx + 1))

    # If the value reaches 256, wrap it back to 1 (0x01)
    if [ "$dec_value" -gt 255 ]; then
        dec_value=1
    fi

    # Convert the decimal value back to a uint8 hex number
    idx=$(printf "%02X" $dec_value)
done

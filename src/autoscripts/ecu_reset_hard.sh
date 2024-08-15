#!/bin/bash

# Process name
PROCESS_NAME="main_mcu"

# Temporary file to store `top` output
TMP_FILE=$(mktemp)

# Capture `top` output in batch mode
top -b -n 1 > "$TMP_FILE"

# Extract PIDs of the specified process name
PIDS=$(awk -v process="$PROCESS_NAME" '$12 ~ process {print $1}' "$TMP_FILE")

# Check if any PIDs were found
if [ -n "$PIDS" ]; then
    echo "Killing processes with PIDs: $PIDS"
    # Kill each PID
    for PID in $PIDS; do
        kill -9 "$PID"
    done
    echo "Processes named '$PROCESS_NAME' have been terminated."
else
    echo "No processes named '$PROCESS_NAME' found."
fi

rm -f "$TMP_FILE"
./main_mcu


# Process name
PROCESS_NAME="main_battery"

# Temporary file to store `top` output
TMP_FILE=$(mktemp)

# Capture `top` output in batch mode
top -b -n 1 > "$TMP_FILE"

# Extract PIDs of the specified process name
PIDS=$(awk -v process="$PROCESS_NAME" '$12 ~ process {print $1}' "$TMP_FILE")

# Check if any PIDs were found
if [ -n "$PIDS" ]; then
    echo "Killing processes with PIDs: $PIDS"
    # Kill each PID
    for PID in $PIDS; do
        kill -9 "$PID"
    done
    echo "Processes named '$PROCESS_NAME' have been terminated."
else
    echo "No processes named '$PROCESS_NAME' found."
fi

cd ../ecu_simulation/BatteryModule/
./main_battery



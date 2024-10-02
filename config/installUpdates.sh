#!/bin/bash

# Base path for executables
BASE_PATH="./"

# Array of possible executables
NEW_EXECUTABLES=("main_mcu_new" "main_battery_new" "main_doors_new" "main_engine_new" "main_hvac_new")
CURRENT_EXECUTABLES=("main_mcu" "main_battery" "main_doors" "main_engine" "main_hvac")

CURRENT_EXECUTABLE_PID=$1
CURRENT_EXECUTABLE=$2
NEW_EXECUTABLE=${CURRENT_EXECUTABLE}_new
# Function to find and kill the running processes
kill_processes() {
    echo "Attempting to kill running processes with executable: $1"

    # Find PIDs of processes running the current executable
    PIDS=$(pgrep -f "$1")

    if [ -z "$PIDS" ]; then
        echo "No running processes found with name $1"
        return
    fi

    # Kill each process
    for PID in $PIDS; do
        echo "Killing process PID: $PID"
        kill -9 "$PID"

        if [ $? -eq 0 ]; then
            echo "Successfully killed process with PID $PID"
            sleep 1
        else
            echo "Failed to kill process with PID $PID"
            exit 1
        fi
    done
}

kill_process_group() {
    echo "Attempting to kill process group with PID: $1 for the $2 executable"
    #Can't kill the process group because it also kills the SCRIPT and it stops after the kill
    #TODO!!
    kill -9 $1

    if [ $? -eq 0 ]; then
        echo "Successfully killed process with PID $PID"
        sleep 1
    else
        echo "Failed to kill process with PID $PID"
        exit 1
    fi

}
# Function to replace the executable
replace_executable() {
    echo "Replacing $1 with $2"

    # Copy the new executable to the current executable path
    sudo cp "$2" "$1"

    # Check if the copy was successful
    if [ $? -eq 0 ]; then
        echo "Replacement successful"
    else
        echo "Failed to replace $1"
        exit 1
    fi

    # Remove the new executable file
    rm "$2"

    # Check if removal was successful
    if [ $? -eq 0 ]; then
        echo "Removed $2"
    else
        echo "Failed to remove $2"
        # exit 1
    fi
}

# Function to start the new executable
start_executable() {
    echo "Starting $1"

    # Start the executable in the background
    "./$1" &

    # Capture the PID of the last background command
    local pid=$!

    # Give the process a moment to initialize and check if it's running
    sleep 1
    
    # Check if the process is running
    if ps -p $pid > /dev/null; then
        echo "$1 started successfully with PID: $pid"
    else
        echo "Failed to start $1"
        exit 1
    fi
}


# kill_processes "${CURRENT_EXECUTABLE}"
# replace_executable "${CURRENT_EXECUTABLE}" "${NEW_EXECUTABLE}"
# start_executable "${CURRENT_EXECUTABLE}"
# Main script execution
# for i in "${!NEW_EXECUTABLES[@]}"; do
#     NEW_EXECUTABLE="${BASE_PATH}${NEW_EXECUTABLES[$i]}"
#     CURRENT_EXECUTABLE="${BASE_PATH}${CURRENT_EXECUTABLES[$i]}"
#     if [ -f "$NEW_EXECUTABLE" ]; then
#         echo "Found new executable: $NEW_EXECUTABLE"
#         kill_processes "$CURRENT_EXECUTABLE"
#         replace_executable "$CURRENT_EXECUTABLE" "$NEW_EXECUTABLE"
#         start_executable "$CURRENT_EXECUTABLE"
#         break
#     else
#         echo "No new executable found for $CURRENT_EXECUTABLE"
#     fi
# done

kill_process_group "${CURRENT_EXECUTABLE_PID}" "${CURRENT_EXECUTABLE}"
replace_executable "${CURRENT_EXECUTABLE}" "${NEW_EXECUTABLE}"
start_executable "${CURRENT_EXECUTABLE}"
sleep 1

echo "Update complete."
exit 0

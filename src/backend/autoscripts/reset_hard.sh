#!/bin/bash

# Check if a parameter was provided
if [ -z "$1" ]; then
    echo "Usage: $0 <process_name>"
    exit 1
fi

PROCESS_NAME="$1"

# Execute the program and replace the current shell with it
exec ./"$PROCESS_NAME"
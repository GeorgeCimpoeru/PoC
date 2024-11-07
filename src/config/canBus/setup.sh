#!/bin/bash

# Variables
SERVICE_FILE="canSimu.service"
SCRIPT_FILE="canBusSimu.sh"

# Get the path of the CAN Bus simulation script
SCRIPT_PATH=$(readlink -f $SCRIPT_FILE)

sudo chmod +x $SCRIPT_PATH

# Create the file .service and fill it
echo "[Unit]
Description=Simulates CAN Bus between vcan interfaces
After=network.target

[Service]
ExecStart=$SCRIPT_PATH

[Install]
WantedBy=default.target" > $SERVICE_FILE

# Move the file in /etc/systemd/system
sudo mv $PWD/$SERVICE_FILE /etc/systemd/system/

# Reload the system daemon to load the new service file
sudo systemctl daemon-reload

# Enable the service to start at boot
sudo systemctl enable $SERVICE_FILE

# Start the service
sudo systemctl start $SERVICE_FILE


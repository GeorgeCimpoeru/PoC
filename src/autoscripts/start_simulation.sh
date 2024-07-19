#!/bin/bash

# Define the commands for the REST API terminal
rest_api_command='
cd ../rest_api &&
echo "Navigated to ../rest_api" &&
sudo apt install -y python3-pip python3.8 python3.8-venv &&
echo "Installed python3-pip, python3.8, and python3.8-venv" &&
python3.8 -m venv venv &&
echo "Created Python virtual environment" &&
source venv/bin/activate &&
echo "Activated Python virtual environment" &&
pip install -r requirements.txt &&
echo "Installed Python packages" &&
python3.8 app.py &
sleep 5  # Wait for the server to start
while ! curl -sSf http://127.0.0.1:5000 > /dev/null; do
    echo "Waiting for the server to start..."
    sleep 1
done
xdg-open http://127.0.0.1:5000 &&
echo "Opened the application in the web browser" &&
exec bash
'

# Define the commands for the MCU terminal
mcu_command='
cd ../mcu &&
echo "Navigated to ../mcu" &&
{
    sudo apt install -y build-essential &&
    echo "Installed build-essential" &&
    make clean &&
    echo "Cleaned the MCUModule build" &&
    make &&
    echo "Compiled the MCU application" &&
    ./main &&
    echo "Ran the MCU application"
} 2>&1 | tee mcu_log.txt &&
exec bash
'

# Define the commands for the BatteryModule terminal
battery_module_command='
cd ../ecu_simulation/BatteryModule &&
echo "Navigated to ../ecu_simulation/BatteryModule" &&
make clean &&
echo "Cleaned the BatteryModule build" &&
make &&
echo "Compiled the BatteryModule application" &&
./main &&
echo "Ran the BatteryModule application" &&
exec bash
'

# Define the commands for the Front End terminal
front_end_command='
cd ../front_end/carsdata &&
echo "Navigated to ../front_end/carsdata" &&

# Check if npm is installed
if ! command -v npm &> /dev/null; then
    echo "npm not found, installing npm..." &&
    sudo apt install -y npm &&
    echo "Installed npm"
else
    echo "npm is already installed"
fi &&

# Clear npm cache
sudo npm cache clean --force &&
echo "Cleared npm cache" &&

# Check if Next.js 14.2.3 is installed before removing the directory
if ! npm list -g next | grep -q "next@14.2.3"; then
    echo "Next.js 14.2.3 not found, removing existing directory and installing Next.js globally..." &&
    sudo rm -rf /usr/local/lib/node_modules/next &&
    echo "Removed existing Next.js directory" &&
    sudo npm install -g next@14.2.3 &&
    echo "Installed Next.js globally"
else
    echo "Next.js 14.2.3 is already installed"
fi &&

# Check if n is installed
if ! command -v n &> /dev/null; then
    echo "n not found, installing n..." &&
    sudo npm install -g n &&
    echo "Installed Node version manager 'n'"
else
    echo "n is already installed"
fi &&

# Check if Node.js version 20.15 is installed
if ! node -v | grep -q "v20.15"; then
    echo "Node.js 20.15 not found, installing Node.js 20.15..." &&
    sudo n 20.15 &&
    echo "Installed Node.js version 20.15"
else
    echo "Node.js 20.15 is already installed"
fi &&

# Check if react and react-dom are installed
if ! npm list react | grep -q "react@"; then
    echo "React not found, installing React and React DOM..." &&
    sudo npm install react react-dom &&
    echo "Installed React and React DOM"
else
    echo "React is already installed"
fi &&

# Check if Firebase is installed
if ! npm list firebase | grep -q "firebase@"; then
    echo "Firebase not found, installing Firebase..." &&
    sudo npm i firebase &&
    echo "Installed Firebase"
else
    echo "Firebase is already installed"
fi &&

# Check if DaisyUI is installed
if ! npm list daisyui | grep -q "daisyui@"; then
    echo "DaisyUI not found, installing DaisyUI..." &&
    sudo npm i -D daisyui@latest &&
    echo "Installed DaisyUI"
else
    echo "DaisyUI is already installed"
fi &&

# Run the development server
sudo npm run dev &

# Wait for the server to start
sleep 5

# Check if the server is running
while ! curl -sSf http://localhost:3000 > /dev/null; do
    echo "Waiting for the server to start..."
    sleep 1
done

# Open the application in the web browser
xdg-open http://localhost:3000 &&
echo "Opened the application in the web browser" &&
exec bash
'

# Define a function to attempt running a command in a list of terminal emulators
run_in_terminal() {
    local cmd="$1"
    shift
    local emulators=("$@")
    for terminal in "${emulators[@]}"; do
        if command -v "$terminal" &> /dev/null; then
            echo "Attempting to open $terminal for the job..."
            case "$terminal" in
                gnome-terminal)
                    gnome-terminal -- bash -c "$cmd; exec bash -i" 2>/dev/null && break || echo "$terminal failed. Trying the next terminal..."
                    ;;
                alacritty)
                    alacritty -e bash -c "$cmd; exec bash -i" 2>/dev/null && break || echo "$terminal failed. Trying the next terminal..."
                    ;;
                xterm)
                    xterm -e bash -c "$cmd; exec bash -i" 2>/dev/null && break || echo "$terminal failed. Trying the next terminal..."
                    ;;
                rxvt)
                    rxvt -e bash -c "$cmd; exec bash -i" 2>/dev/null && break || echo "$terminal failed. No more terminals to try."
                    ;;
                *)
                    echo "$terminal is not supported in this script."
                    ;;
            esac
        else
            echo "$terminal is not installed. Trying the next terminal..."
        fi
    done
}

# List of terminal emulators to try for all jobs
terminals=(gnome-terminal alacritty xterm rxvt)

# Run the REST API job in its own terminal
echo "Starting REST API job..."
(
    run_in_terminal "$rest_api_command" "${terminals[@]}"
) &

# Run the MCU job in its own terminal
echo "Starting MCU job..."
(
    run_in_terminal "$mcu_command" "${terminals[@]}"
) &

# Run the BatteryModule job in its own terminal
echo "Starting BatteryModule job..."
(
    run_in_terminal "$battery_module_command" "${terminals[@]}"
) &

# Run the Front End job in its own terminal
echo "Starting Front End job..."
(
    run_in_terminal "$front_end_command" "${terminals[@]}"
) &

# Wait for all background jobs to complete
wait
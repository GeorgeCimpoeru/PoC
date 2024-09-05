#!/bin/bash

# Save initial directory
INITIAL_DIR="$PWD"

# Ensure the script is run with sudo
if [ "$EUID" -ne 0 ]; then
  echo "Please run with sudo"
  exit
fi

# Step 0: Check if Python version 3.8 or higher is installed
PYTHON_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')
REQUIRED_VERSION="3.8"

if [ "$(printf '%s\n' "$REQUIRED_VERSION" "$PYTHON_VERSION" | sort -V | head -n1)" != "$REQUIRED_VERSION" ]; then
  echo "Python 3.8 or higher is required. Please install the appropriate version."
  exit 1
fi

echo "Python version is $PYTHON_VERSION. Proceeding..."

# Step 1: Change directory to ../rest_api
cd ../rest_api || { echo "rest_api directory not found. Exiting."; exit 1; }

# Step 2: Check if venv folder exists
if [ ! -d "venv" ]; then
  echo "Creating virtual environment..."
  python3.8 -m venv venv
fi

# Step 3: Activate virtual environment and install requirements
echo "Activating virtual environment and installing requirements..."
source venv/bin/activate
pip install -r requirements.txt

# Step 4: Change directory back to ../mcu
cd ../mcu || { echo "mcu directory not found. Exiting."; exit 1; }

# Ensure we are back in the initial directory
cd "$INITIAL_DIR"

# Step 5: Navigate backwards to find the Desktop directory
while [[ "$PWD" != "/" && "${PWD##*/}" != "Desktop" ]]; do
  cd ..
done

# Check if we are in the Desktop directory
if [[ "${PWD##*/}" != "Desktop" ]]; then
  echo "Desktop directory not found. Exiting."
  exit 1
fi

echo "Found Desktop directory at $PWD"

IMG_PATH="sdcard.img"
LOOP_DEVICE=""
PART1_SIZE="+100M"
START_SECTOR=2048
SEEK_OFFSET=118006272
DD_COUNT=17168
MOUNT_DIR="/mnt/sdcard"

# Step 6: Search and remove any existing loop device associated with sdcard.img
EXISTING_LOOP_DEVICE=$(losetup -j "$IMG_PATH" | cut -d: -f1)

if [ -n "$EXISTING_LOOP_DEVICE" ]; then
  echo "Found existing loop device: $EXISTING_LOOP_DEVICE."

  # Unmount any partitions associated with the loop device
  MOUNTED_PARTITIONS=$(lsblk -lno NAME,MOUNTPOINT | grep "^$(basename $EXISTING_LOOP_DEVICE)" | awk '{print $2}')
  if [ -n "$MOUNTED_PARTITIONS" ]; then
    echo "Unmounting partitions:"
    for MOUNT_POINT in $MOUNTED_PARTITIONS; do
      echo "Unmounting $MOUNT_POINT..."
      umount "$MOUNT_POINT"
    done
  fi

  # Detach the loop device
  echo "Detaching loop device $EXISTING_LOOP_DEVICE..."
  losetup -d "$EXISTING_LOOP_DEVICE"
fi

# Step 7: Check if the image already exists
if [ -f "$IMG_PATH" ]; then
  echo "Image already exists at $IMG_PATH."
else
  echo "Creating image..."
  truncate -s +300M "$IMG_PATH"
fi

# Step 8: Create a loop device
LOOP_DEVICE=$(losetup -fP --show "$IMG_PATH")
echo "Loop device created: $LOOP_DEVICE"

# Extract the loop number from LOOP_DEVICE
LOOP_NUMBER=$(basename "$LOOP_DEVICE" | grep -o '[0-9]*')

# Step 9: Check for existing partitions
if [ "$(lsblk -lno NAME "$LOOP_DEVICE" | grep -c 'p1\|p2')" -gt 0 ]; then
  echo "Partitions already exist on $LOOP_DEVICE."
  echo "Skipping partition creation..."
  PARTITIONS_EXIST=true
else
  PARTITIONS_EXIST=false
fi

if [ "$PARTITIONS_EXIST" = false ]; then
  # Step 10: Create partitions
  echo "Creating partitions..."
  (
  echo n # New partition
  echo p # Primary
  echo 1 # Partition number
  echo $START_SECTOR # First sector
  echo $PART1_SIZE # Last sector
  echo n # New partition
  echo p # Primary
  echo 2 # Partition number
  echo   # First sector (Accept default)
  echo   # Last sector (Accept default)
  echo Y # Delete sign
  echo w # Write changes
  ) | fdisk "$LOOP_DEVICE"

  # Step 11: Formatting the partitions
  echo "Formatting partitions..."
  mkfs.fat -F 32 "${LOOP_DEVICE}p1"
  mkfs.fat -F 16 "${LOOP_DEVICE}p2"

  # Step 12: Change permissions of the loop device
  chmod 777 "$LOOP_DEVICE"
fi

# Step 13: Create mount directory
echo "Creating mount directory..."
mkdir -p "$MOUNT_DIR"

# Step 14: Mount partitions
echo "Mounting partitions..."
mount -o rw,uid=1000,gid=1000 "${LOOP_DEVICE}p1" "$MOUNT_DIR"
mount -o rw,uid=1000,gid=1000 "${LOOP_DEVICE}p2" "$MOUNT_DIR"

# Step 15: Display partition data
echo "Reading data from offset $SEEK_OFFSET..."
xxd -l $DD_COUNT -s $SEEK_OFFSET "$LOOP_DEVICE"

# Step 16: Zero out the data at specified offset
echo "Zeroing out data at offset $SEEK_OFFSET..."
dd if=/dev/zero of="$LOOP_DEVICE" bs=1 seek=$SEEK_OFFSET count=$DD_COUNT conv=notrunc

# Display the created partitions
echo "Partitions created:"
lsblk -lno NAME,TYPE,SIZE "$LOOP_DEVICE"

# Step 17: Navigate back to initial location
cd "$INITIAL_DIR"

# Step 18: Navigate backwards to find the 'src' directory
while [[ "$PWD" != "/" && "${PWD##*/}" != "src" ]]; do
  cd ..
done

# Check if we are in the 'src' directory
if [[ "${PWD##*/}" != "src" ]]; then
  echo "'src' directory not found. Exiting."
  exit 1
fi

echo "Found 'src' directory at $PWD"
SRC_DIR="$PWD"

# Step 19: Replace loop numbers in files, excluding MCULogs.log
echo "Replacing loop numbers in files, excluding MCULogs.log..."
find . -type f ! -name "MCULogs.log" -exec sed -i "s/\/dev\/loop[0-9]*/\/dev\/loop${LOOP_NUMBER}/g" {} +

# Navigate to SRC_DIR, go downwards into mcu folder, and run make commands
cd "$SRC_DIR" || { echo "'src' directory not found. Exiting."; exit 1; }

# Go downwards into mcu folder
cd mcu || { echo "'mcu' directory not found. Exiting."; exit 1; }

# Run make file and make commands
echo "Running make commands..."
make clean
make
echo "Build completed successfully."

echo "Script completed successfully."


#!/bin/bash

# Save initial directory
INITIAL_DIR="$PWD"

# Ensure the script is run with sudo
if [ "$EUID" -ne 0 ]; then
  echo "Please run with sudo"
  exit
fi

PYTHON_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')
REQUIRED_VERSION="3.8"

if [ "$(printf '%s\n' "$REQUIRED_VERSION" "$PYTHON_VERSION" | sort -V | head -n1)" != "$REQUIRED_VERSION" ]; then
  echo "Python 3.8 or higher is required. Please install the appropriate version."
  exit 1
fi

echo "Python version is $PYTHON_VERSION. Proceeding..."

# Step 1: Change directory to ../rest_api
cd ../rest_api || { echo "rest_api directory not found. Exiting."; exit 1; }

# Step 2
# Check if the virtual environment exists
if [ -d "venv" ]; then
    echo "Creating virtual environment..."
    python3.8 -m venv venv
    # Activate the virtual environment
    source venv/bin/activate
    echo "Virtual environment activated."

    # Optional: Check if the activation was successful
    if [ "$VIRTUAL_ENV" != "" ]; then
        echo "Currently in virtual environment: $VIRTUAL_ENV"
        pip install -r requirements.txt
    else
        echo "Failed to activate the virtual environment."
    fi
else
    echo "Virtual environment not found. Please create it using 'python3 -m venv venv'."
fi

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
      sudo umount "$MOUNT_POINT"
    done
  fi

  # Reformatting the partitions
  LOOP_PARTITIONS=$(lsblk -lno NAME | grep "^$(basename $EXISTING_LOOP_DEVICE)p")
  if [ -n "$LOOP_PARTITIONS" ]; then
    echo "Reformatting partitions:"
    for PARTITION in $LOOP_PARTITIONS; do
      echo "Reformatting /dev/$PARTITION..."
      sudo mkfs.fat -F 32 "/dev/$PARTITION"  # Reformat as FAT32 (adjust if needed)
    done
  else
    echo "No partitions found for $EXISTING_LOOP_DEVICE."
  fi

  # Detach the loop device
  echo "Detaching loop device $EXISTING_LOOP_DEVICE..."
  sudo losetup -d "$EXISTING_LOOP_DEVICE"

  # Verify that the loop device has been successfully detached
  if losetup -a | grep -q "$IMG_PATH"; then
    echo "Loop device $EXISTING_LOOP_DEVICE could not be detached. Forcing detachment..."
    sudo losetup -D  # Force-detach all loop devices
  fi
fi

# Step 7: Check if the image already exists
# if [ -f "$IMG_PATH" ]; then
  # echo "Image already exists at $IMG_PATH."
sudo rm "$IMG_PATH"

echo "Creating image..."
sudo truncate -s +300M "$IMG_PATH"

# Step 8: Create a loop device
LOOP_DEVICE=$(losetup -fP --show "$IMG_PATH")
echo "Loop device created: $LOOP_DEVICE"

# Extract the loop number from LOOP_DEVICE
LOOP_NUMBER=$(basename "$LOOP_DEVICE" | grep -o '[0-9]*')

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
echo w # Write changes
) | sudo fdisk "$LOOP_DEVICE"

sudo partprobe "$LOOP_DEVICE"

# Step 11: Formatting the partitions
echo "Formatting partitions..."
sudo mkfs.fat -F 32 "${LOOP_DEVICE}p1"
sudo mkfs.fat -F 16 "${LOOP_DEVICE}p2"

# Step 12: Change permissions of the loop device
sudo chmod 777 "$LOOP_DEVICE"

# Step 13: Create mount directory
echo "Creating mount directory..."
mkdir -p "$MOUNT_DIR"

# Step 14: Mount partitions
echo "Mounting partitions..."
sudo mount -o rw,uid=1000,gid=1000 "${LOOP_DEVICE}p1" "$MOUNT_DIR"
sudo mount -o rw,uid=1000,gid=1000 "${LOOP_DEVICE}p2" "$MOUNT_DIR"

# Step 15: Display partition data
# echo "Reading data from offset $SEEK_OFFSET..."
# xxd -l $DD_COUNT -s $SEEK_OFFSET "$LOOP_DEVICE"

# Step 16: Zero out the data at specified offset
# echo "Zeroing out data at offset $SEEK_OFFSET..."
# sudo dd if=/dev/zero of="$LOOP_DEVICE" bs=1 seek=$SEEK_OFFSET count=$DD_COUNT conv=notrunc

# Display the created partitions
echo "Partitions created:"
sudo lsblk -lno NAME,TYPE,SIZE "$LOOP_DEVICE"

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
# make clean
make
echo "Build completed successfully."

echo "Script completed successfully."

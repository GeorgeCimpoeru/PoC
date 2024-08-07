import os
import sys
import subprocess
import shutil

"""
    STEPS TO DO BEFORE RUNNING THE SCRIPT

1.Create and activate the venv, install dependencies
    cd /src/rest_api
    python3.8 -m venv venv
    source venv/bin/activate
    pip install -r requirements.txt
2.Add key.json to PoC folder
  Copy the key.json from drive PoC folder

    HOW TO USE THE SCRIPT

    command: python3 ReleaseScript.py [version_number] [what_to_create: mcu/ecu/all]
    example: python3 ReleaseScript.py 1 all
"""

PROJECT_PATH = os.path.abspath(os.path.join(os.getcwd(), "..", ".."))
PATH_SOFTWARE_RELEASES = PROJECT_PATH + '/software_releases'

def create_exec(version: str, sw_to_build: str):
    dir_name = "release_" + sw_to_build + '_'  + version
    directory_path = os.path.expanduser(PATH_SOFTWARE_RELEASES + "/"+dir_name)

    if subprocess.run(["ls {0}/{1} 2>/dev/null".format(PATH_SOFTWARE_RELEASES, dir_name)], shell=True).returncode == 0:
        print("This release_{0}_{1} already exists".format(version, sw_to_build))
        exit(-1)
    try:
        # Create the directory
        subprocess.run(
            ["mkdir", "-p", "{0}/{1}".format(PATH_SOFTWARE_RELEASES, dir_name)], check=True)
        if sw_to_build == "mcu" or sw_to_build == "all":
            # Build the MCU executable
            subprocess.run(
                ["make", "-C", "{0}/../src/mcu/".format(PATH_SOFTWARE_RELEASES)], check=True)
            mcu_executable_path = "{0}/../src/mcu/main".format(PATH_SOFTWARE_RELEASES)
            mcu_archive_name = "{0}/{1}/MCU_SW_VERSION_{2}".format(
                PATH_SOFTWARE_RELEASES, dir_name, version)
            shutil.make_archive(mcu_archive_name, 'zip', root_dir=os.path.dirname(
                mcu_executable_path), base_dir=os.path.basename(mcu_executable_path))
        if sw_to_build == "ecu" or sw_to_build == "all":
            # Build the Battery Module executable
            subprocess.run(
                ["make", "-C", "{0}/../src/ecu_simulation/BatteryModule".format(PATH_SOFTWARE_RELEASES)], check=True)
            battery_executable_path = "{0}/../src/ecu_simulation/BatteryModule/main".format(
                PATH_SOFTWARE_RELEASES)
            battery_archive_name = "{0}/{1}/ECU_BATTERY_SW_VERSION_{2}".format(
                PATH_SOFTWARE_RELEASES, dir_name, version)
            shutil.make_archive(battery_archive_name, 'zip', root_dir=os.path.dirname(
                battery_executable_path), base_dir=os.path.basename(battery_executable_path))

    except subprocess.CalledProcessError as error:
        print(error)
    except FileNotFoundError as error:
        print(error)
    return directory_path


def uploadRelease(directory_path: str):
    google_drive_api_path = PROJECT_PATH + '/src/ota/google_drive_api'
    sys.path.append(google_drive_api_path)
    from GoogleDriveApi import gDrive, DRIVE_MCU_SW_VERSIONS_FILE, DRIVE_ECU_BATTERY_SW_VERSIONS_FILE

    for file in os.listdir(directory_path):
        if "MCU" in file:
            gDrive.uploadFile(file, os.path.join(
                directory_path, file), DRIVE_MCU_SW_VERSIONS_FILE)
            print(file + ' UPLOADED TO GOOGLE DRIVE')
        elif "BATTERY" in file:
            gDrive.uploadFile(file, os.path.join(directory_path, file),
                              DRIVE_ECU_BATTERY_SW_VERSIONS_FILE)
            print(file + ' UPLOADED TO GOOGLE DRIVE')

def validateSoftwareVersion(software_version):
    #TODO
    return software_version

def validateSoftwareToBuild(software_to_build):
    #TODO
    return software_to_build

def main():
    version = "1.0"
    sw_to_build = "all"

    if len(sys.argv) < 2:
        print("Please provide as first argument the version of the release using format 1.0 -> 1.15 and as second argument the software to build mcu/ecu/all")
        exit(-1)
    else:
        version = validateSoftwareVersion(sys.argv[1])
        sw_to_build = validateSoftwareToBuild(sys.argv[2])

    output_path = create_exec(version, sw_to_build)
    uploadRelease(output_path)

if __name__ == "__main__":
    main()

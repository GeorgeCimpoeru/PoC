import os
import sys
import subprocess
import shutil

# HOW TO USE THE SCRIPT
# command: python3 ReleaseScript.py [version_number] [what_to_create: mcu/ecu/all]
# example: python3 ReleaseScript.py 1 all

path_tool =""
def find_path():
    #Search in most impotrtantdir
    possible_paths = ["~/PoC/tools","~/Desktop/PoC/tools"]
    for path in possible_paths:
        if os.system("cd {0} 2>/dev/null".format(path)) == 0:
            global path_tool
            path_tool = path
            return
    #Search in the whole user dir
    result = subprocess.run("find ~ -type d -name PoC 2>/dev/null", shell=True, stdout=subprocess.PIPE, text=True)
    if str(result.stdout) != "":
        path_tool = str(result.stdout)[:-1]+"/tools"
        return
    #Unable to find
    print("Unable to find the PoC/tool dir, please pass the path as second argument")
    exit(-1)

def checking():
    r = subprocess.run('pwd',shell=True, stdout=subprocess.PIPE, text=True)
    current_dir = str(r.stdout)[-10:-1]
    if current_dir != "PoC/tools":
        find_path()
    
def create_exec(version:str, sw_to_build:str):
    dir_name = "release_"+ version + '_' + sw_to_build
    directory_path = os.path.expanduser( path_tool +"/"+dir_name)

    if subprocess.run(["ls {0}/{1} 2>/dev/null".format(path_tool,dir_name)],shell=True).returncode == 0:
        print("This release v{0} alrready exists".format(version))
        exit (-1)
    try:
        # Create the directory
        subprocess.run(["mkdir", "-p", "{0}/{1}".format(path_tool, dir_name)], check=True)
        if sw_to_build == "mcu" or sw_to_build == "all":
            # Build the MCU executable
            subprocess.run(["make", "-C", "{0}/../src/mcu/".format(path_tool)], check=True)
            mcu_executable_path = "{0}/../src/mcu/main".format(path_tool)
            mcu_archive_name = "{0}/{1}/MCU_SW_VERSION_{2}".format(path_tool, dir_name, version)
            shutil.make_archive(mcu_archive_name, 'zip', root_dir=os.path.dirname(mcu_executable_path), base_dir=os.path.basename(mcu_executable_path))
        if sw_to_build == "ecu" or sw_to_build == "all":
            # Build the Battery Module executable
            subprocess.run(["make", "-C", "{0}/../src/ecu_simulation/BatteryModule".format(path_tool)], check=True)
            battery_executable_path = "{0}/../src/ecu_simulation/BatteryModule/main".format(path_tool)
            battery_archive_name = "{0}/{1}/ECU_BATTERY_SW_VERSION_{2}".format(path_tool, dir_name, version)
            shutil.make_archive(battery_archive_name, 'zip', root_dir=os.path.dirname(battery_executable_path), base_dir=os.path.basename(battery_executable_path))

    except subprocess.CalledProcessError as error:
        print(error)
    except FileNotFoundError as error:
        print(error)
    return directory_path

def uploadRelease(directory_path : str):
    sys.path.append('/home/projectx/accademyprojects/PoC/src/ota/google_drive_api')
    from GoogleDriveApi import gDrive, DRIVE_MCU_SW_VERSIONS_FILE, DRIVE_ECU_BATTERY_SW_VERSIONS_FILE

    for file in os.listdir(directory_path):
        if "MCU" in file:
            gDrive.uploadFile(file, os.path.join(directory_path, file), DRIVE_MCU_SW_VERSIONS_FILE)
        elif "BATTERY" in file:
            gDrive.uploadFile(file, os.path.join(directory_path, file), DRIVE_ECU_BATTERY_SW_VERSIONS_FILE)

    print("\n--------- SW Versions Uploaded ----------")

def main():
    global path_tool
    version = "1"
    sw_to_build = "all"
    if len(sys.argv) > 1:
        version = sys.argv[1]
        checking()
    elif len(sys.argv) <= 1:
        print("Please provide as argument the version of the release")
        # exit(-1)
    if len(sys.argv) > 2:
        sw_to_build = sys.argv[2]
    output_path = create_exec(version, sw_to_build)
    uploadRelease(output_path)

if __name__ == "__main__":
    main()
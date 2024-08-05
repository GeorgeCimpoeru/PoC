import os
import sys
import subprocess
import shutil
import pkg_resources
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
    
def create_exec(version:str):
    dir_name = "release_"+ version
    output_path = ""
    if subprocess.run(["ls {0}/{1} 2>/dev/null".format(path_tool,dir_name)],shell=True).returncode == 0:
        print("This release v{0} alrready exists".format(version))
        exit (-1)
    try:
        subprocess.run(["mkdir", "-p", "{0}/{1}".format(path_tool, dir_name)], check=True)
        subprocess.run(["make", "-C", "{0}/../src/mcu/".format(path_tool)], check=True)
        subprocess.run(["mv", "{0}/../src/mcu/main".format(path_tool), "{0}/{1}/executable_mcu_{2}".format(path_tool, dir_name, version)], check=True)
        subprocess.run(["make", "-C", "{0}/../src/ecu_simulation/BatteryModule".format(path_tool)], check=True)
        subprocess.run(["mv", "{0}/../src/ecu_simulation/BatteryModule/main".format(path_tool), "{0}/{1}/executable_battery_{2}".format(path_tool, dir_name, version)], check=True)

        directory_path = os.path.expanduser( path_tool +"/"+dir_name)
        output_path = os.path.expanduser("{0}/{1}/release_{2}".format(path_tool, dir_name, version))
        print(output_path, directory_path)
        # shutil.make_archive(output_path, 'zip', directory_path)
    except subprocess.CalledProcessError as error:
        print(error)
    except FileNotFoundError as error:
        print(error)
    return directory_path

def uploadRelease(directory_path : str):
    sys.path.append('/home/projectx/accademyprojects/PoC/src/ota/google_drive_api')
    from GoogleDriveApi import gDrive, DRIVE_MCU_SW_VERSIONS_FILE, DRIVE_ECU_BATTERY_SW_VERSIONS_FILE

    for file in os.listdir(directory_path):
        if "mcu" in file:
            gDrive.uploadFile(file, os.path.join(directory_path, file), DRIVE_MCU_SW_VERSIONS_FILE)
        elif "battery" in file:
            gDrive.uploadFile(file, os.path.join(directory_path, file), DRIVE_ECU_BATTERY_SW_VERSIONS_FILE)

    print("\n--------- File is Uploaded ----------")

def main():
    global path_tool
    version = "1"
    if len(sys.argv) > 1:
        version = sys.argv[1]
        checking()
    elif len(sys.argv) <= 1:
        print("Please provide as argument the version of the release")
        # exit(-1)
    if len(sys.argv) > 2:
        path_tool = sys.argv[2]
    output_path = create_exec(version)
    uploadRelease(output_path)

if __name__ == "__main__":
    main()
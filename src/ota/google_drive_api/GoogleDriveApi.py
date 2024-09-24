import io
import os

from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.errors import HttpError
from googleapiclient.http import MediaIoBaseDownload
from googleapiclient.http import MediaFileUpload

# Colors used for printing messages
RED = '\033[91m'
GREEN = '\033[92m'
YELLOW = '\033[93m'
RESET = '\033[0m'

PROJECT_PATH = os.path.abspath(os.path.join(os.getcwd(), "..", ".."))
CREDS_PATH = PROJECT_PATH + '/key.json'
OAUTH2_SCOPE = 'https://www.googleapis.com/auth/drive'
DRIVE_BASE_FILE = {
    "name": "SW_VERSIONS",
    "id": '1LtP9TKMDLFv2Hbf-d51DaXDCBE59vZer',
}
DRIVE_ECU_HVAC_SW_VERSIONS_FILE = '1sTZIRTOHBPoxyiWxF2m2pUQSwu1E9_wH'
DRIVE_ECU_DOORS_SW_VERSIONS_FILE = '1xeZCYZ0akwIjmFhJkCkfDg63h-JVGhIC'
DRIVE_ECU_ENGINE_SW_VERSIONS_FILE = '16e1Hp9QYl1oEjZ0XK927XiwF9h5ySmVn'
DRIVE_ECU_BATTERY_SW_VERSIONS_FILE = '1QkgBWPEaKg5bnOU0eXjPOEcz6lqNCG-N'
DRIVE_MCU_SW_VERSIONS_FILE = '1aGo68MWCYBxMVSPd0-jZ4cBGICGdMRxp'
# TO BE CHANGED WITH THE DESIRED PATH FOR DOWNLOADS
DRIVE_DOWNLOAD_PATH = PROJECT_PATH

# QUERY STRINGS for google drive api filest.list() method
FOLDER_MIMETYPE_QUERY = "mimeType = 'application/vnd.google-apps.folder'"
FILE_MIMETYPE_QUERY = "mimeType = 'application/vnd.google-apps.file'"

ecu_map = {
    0x10: "mcu",
    0x11: "battery"
}

class GDriveAPI:
    # credentials needed for authorization. Created from the google cloud key.json file
    __creds = None
    # drive service object for cusing the drive api
    __drive_service = None
    # store data from the drive in json format
    __drive_data_json = {}
    __drive_data_array = []

    # class variable __instance will keep track of the lone object instance
    __instance = None

    # Method used for getting the GDriveAPI object. When first time called, the path to the credentials is needed.
    @staticmethod
    def getInstance(creds_path=None):
        if GDriveAPI.__instance is None:
            if creds_path is None:
                raise Exception("Credentials path needed for creating the credentials.")
            else:
                GDriveAPI(creds_path)
        return GDriveAPI.__instance

    def __init__(self, creds_path):
        if GDriveAPI.__instance is not None:
            raise Exception("Singleton object already created!")
        else:
            self.__creds = service_account.Credentials.from_service_account_file(creds_path, scopes=[
                                                                                 OAUTH2_SCOPE])
            self.__drive_service = build('drive', 'v3', credentials=self.__creds)
            GDriveAPI.__instance = self

    def uploadFile(self, file_name, file_path, size_uncompressed, parent_folder_id=DRIVE_BASE_FILE['id']):
        file_metadata = {
            'name': file_name,
            'parents': [parent_folder_id],  # ID of the folder where you want to upload
            'appProperties':{
                'size_uncompressed': str(size_uncompressed)
            }
        }
        media = MediaFileUpload(file_path, mimetype='text/plain')
        print(file_metadata)
        try:
            print(f"{GREEN}Uploading {file_name} to Google Drive..{RESET}")
            file = self.__drive_service.files().create(
                body=file_metadata, media_body=media, fields='id, appProperties').execute()
            if 'id' in file:
                print(f"{GREEN}File uploaded successfully. File ID:{RESET}", file['id'])
            else:
                print(f"{RED}File upload failed. No file ID returned.{RESET}")
        except Exception as e:
            print(f"{RED}An error occurred during file upload:{RESET}", e)

    def downloadFile(self, ecu_id, sw_version_byte, path_to_download=DRIVE_DOWNLOAD_PATH):
        try:
            # pylint: disable=maybe-no-member
            
            file_to_download = self.searchVersion(ecu_id, sw_version_byte, True)
            if file_to_download == 0:
                return 0
            
            print(f"{GREEN}Downloading..{RESET}")
            request = self.__drive_service.files().get_media(
                fileId=file_to_download['id'])
            downloaded_file = io.BytesIO()
            downloader = MediaIoBaseDownload(downloaded_file, request)
            done = False

            while not done:
                status, done = downloader.next_chunk()
                print(f"{GREEN}Download {int(status.progress() * 100)}%.{RESET}")

        except HttpError as error:
            print(f"{RED}An error occurred: {error}{RESET}")
            return 0

        downloaded_file.seek(0)
        with open(f"{path_to_download}/{file_to_download['name']}", 'wb') as f:
            f.write(downloaded_file.read())
            print(f"{GREEN}File downloaded and saved to {path_to_download}{RESET}")
        file_to_download = None

    def searchVersion(self, ecu_id, sw_version_byte, return_file=False):
        self.getDriveData()
        sw_version = self.__convertByteToSwVersion(hex(sw_version_byte))
        print(f"{GREEN}Searching for version {RESET}" +
              ecu_map[ecu_id] + ' ' + sw_version)
        file_to_download = [
            data for data in self.__drive_data_array if data['type'] == ecu_map[ecu_id] and data['sw_version'] == str(sw_version)]
        
        file_to_download = file_to_download[0] if file_to_download else None
        if file_to_download is None:
            print(
                    f"{RED}No file found with type:{ecu_map[ecu_id]} and version {sw_version}{RESET}")
            return 0
        print(f"{GREEN}Version found{RESET}")
        if return_file == True:
            return file_to_download
        
        return int(file_to_download['size_uncompressed'])
    # PRIVATE METHODS
    def __convertByteToSwVersion(self, software_version_byte):
        # Convert the hex string to an integer
        int_value = int(str(software_version_byte), 16)

        # SMost significant 3 bits => major version, next 4 bits minor_version, lsb not important for versioning
        major_version = ((int_value & 0b11110000) >> 4) + 1
        minor_version = (int_value & 0b00001111)

        # Combine the parts into the version string
        software_version = f"{major_version}.{minor_version}"
        return software_version

    def __getFilesFromFolder(self, folder_name):
        return self.__drive_service.files().list(q="'" + folder_name + "' in parents", pageSize=10, fields="nextPageToken, files(*)").execute()

    def __getFileType(self, file):
        type = "folder"
        if '.' in file["name"]:
            if 'MCU' in file["name"]:
                type = "mcu"
            if 'BATTERY' in file["name"]:
                type = "battery"
        return type

    def __getSoftwareVersion(self, file_name):
        version_with_zip = file_name.split('_')[-1]
        version = version_with_zip.rstrip('.zip')
        return version

    def getDriveData(self, file=DRIVE_BASE_FILE):
        folder_data = self.__getFilesFromFolder(file["id"])
        json_file = {
            'name': file['name'],
            'id': file['id'],
            'type': self.__getFileType(file),
            'children': [],
        }
        if (json_file['type'] != "folder"):
            json_file['sw_version'] = self.__getSoftwareVersion(file['name'])
            json_file['size'] = file.get('size', 'N/A')
            json_file['size_uncompressed'] = file.get('appProperties', {}).get('size_uncompressed', 0)
        self.__drive_data_array.append(json_file)
        if json_file['type'] == "folder":
            json_file['children'].extend(self.getDriveData(file)
                                         for file in folder_data['files'])

        return json_file


# Object to be imported in other modules
gDrive = GDriveAPI.getInstance(CREDS_PATH)
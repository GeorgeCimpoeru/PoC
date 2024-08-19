import io
import json
import os

from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.errors import HttpError
from googleapiclient.http import MediaIoBaseDownload
from googleapiclient.http import MediaFileUpload

PROJECT_PATH = os.path.abspath(os.path.join(os.getcwd(), "..", ".."))
CREDS_PATH = os.path.join(PROJECT_PATH, 'key.json')
OAUTH2_SCOPE = 'https://www.googleapis.com/auth/drive'
DRIVE_BASE_FILE = {
    "name": "SW_VERSIONS",
    "id": '1LtP9TKMDLFv2Hbf-d51DaXDCBE59vZer',
}
DRIVE_ECU_BATTERY_SW_VERSIONS_FILE = '1QkgBWPEaKg5bnOU0eXjPOEcz6lqNCG-N'
DRIVE_MCU_SW_VERSIONS_FILE = '1aGo68MWCYBxMVSPd0-jZ4cBGICGdMRxp'
DRIVE_DOWNLOAD_PATH = PROJECT_PATH  # Adjust to your desired download path

ecu_map = {
    0x10: "mcu",
    0x11: "battery"
}


class GDriveAPI:
    __creds = None
    __drive_service = None
    __drive_data_json = {}
    __drive_data_array = []
    __instance = None

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
            self.__creds = service_account.Credentials.from_service_account_file(
                creds_path, scopes=[OAUTH2_SCOPE]
            )
            self.__drive_service = build('drive', 'v3', credentials=self.__creds)
            GDriveAPI.__instance = self

    def updateDriveData(self):
        self.__drive_data_json = json.dumps(self.getDriveData(), indent=4)
        return self.__drive_data_json

    def uploadFile(self, file_name, file_path, parent_folder_id=DRIVE_BASE_FILE['id']):
        file_metadata = {
            'name': file_name,
            'parents': [parent_folder_id]  # ID of the folder where you want to upload
        }

        # Automatically determine the MIME type
        mimetype = 'text/plain' if file_name.endswith('.txt') else None
        media = MediaFileUpload(file_path, mimetype=mimetype)

        self.__drive_service.files().create(
            body=file_metadata, media_body=media, fields='id').execute()

    def downloadFile(self, ecu_id, sw_version_byte, path_to_download=DRIVE_DOWNLOAD_PATH):
        try:
            sw_version = self.__convertByteToSwVersion(hex(sw_version_byte))
            print('Searching for version ' + sw_version)
            file_to_download = [
                data for data in self.__drive_data_array if data['type'] == ecu_map[ecu_id] and data['sw_version'] == str(sw_version)]
            if not file_to_download:
                print(
                    f"No file found with type:{ecu_map[ecu_id]} and version {sw_version}")
                return
            print('Version found, downloading..')
            file_to_download = file_to_download[0]
            request = self.__drive_service.files().get_media(
                fileId=file_to_download['id'])
            downloaded_file = io.BytesIO()
            downloader = MediaIoBaseDownload(downloaded_file, request)
            done = False
            while not done:
                status, done = downloader.next_chunk()
                print(f"Download {int(status.progress() * 100)}%.")
        except HttpError as error:
            print(f"An error occurred: {error}")
            return
        downloaded_file.seek(0)
        file_name = self.__sanitizeFileName(file_to_download['name'])
        with open(f"{path_to_download}/{file_name}", 'wb') as f:
            f.write(downloaded_file.read())
            print(f"File downloaded and saved to {path_to_download}")

    def __convertByteToSwVersion(self, software_version_byte):
        int_value = int(str(software_version_byte), 16)
        major_version = ((int_value & 0b11100000) >> 5) + 1
        minor_version = (int_value & 0b00011110) >> 1
        software_version = f"{major_version}.{minor_version}"
        return software_version

    def __getFilesFromFolder(self, folder_id):
        query = f"'{folder_id}' in parents"
        return self.__drive_service.files().list(q=query, pageSize=10, fields="nextPageToken, files(id, name, size)").execute()

    def __getFileType(self, file):
        if '.' in file["name"]:
            if 'zip' in file["name"]:
                return "zip"
            elif 'tar' in file["name"]:
                return "tar"
            else:
                return "file"
        return "folder"

    def __getSoftwareVersion(self, file_name):
        version_with_zip = file_name.split('_')[-1]
        version = version_with_zip.rstrip('.zip')
        return version

    def __sanitizeFileName(self, file_name):
        return "".join([c if c.isalnum() or c in (' ', '.', '_') else '_' for c in file_name])

    def getDriveData(self, file=DRIVE_BASE_FILE):
        folder_data = self.__getFilesFromFolder(file["id"])
        json_file = {
            'name': file['name'],
            'id': file['id'],
            'type': self.__getFileType(file),
        }
        if json_file['type'] != "folder":
            json_file['sw_version'] = self.__getSoftwareVersion(file['name'])
            json_file['size'] = file.get('size', 'N/A')
        else:
            children = [self.getDriveData(child_file) for child_file in folder_data['files']]
            if children:
                json_file['children'] = children

        self.__drive_data_array.append(json_file)
        return json_file


# Object to be imported in other modules
gDrive = GDriveAPI.getInstance(CREDS_PATH)

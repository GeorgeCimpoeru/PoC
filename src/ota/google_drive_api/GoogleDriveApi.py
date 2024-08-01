import io
import json

from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.errors import HttpError
from googleapiclient.http import MediaIoBaseDownload
from googleapiclient.http import MediaFileUpload

CREDS_PATH = '/home/projectx/accademyprojects/PoC/key.json'
OAUTH2_SCOPE = 'https://www.googleapis.com/auth/drive'
DRIVE_BASE_FILE = {
    "name": "SW_VERSIONS",
    "id": '11Zj4tGV1EjEF9IfXKG29GnYkAbB7HRL_',
}
DRIVE_DOWNLOAD_PATH = '/home/projectx/Downloads'

# QUERY STRINGS for google drive api filest.list() method
FOLDER_MIMETYPE_QUERY = "mimeType = 'application/vnd.google-apps.folder'"
FILE_MIMETYPE_QUERY = "mimeType = 'application/vnd.google-apps.file'"


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

    def updateDriveData(self):
        self.__drive_data_json = json.dumps(self.__getDriveData(), indent=4)

        return self.__drive_data_json

    def uploadFile(self, file_name, file_path, parent_folder_id=DRIVE_BASE_FILE['id']):
        file_metadata = {
            'name': file_name,
            'parents': [parent_folder_id]  # ID of the folder where you want to upload
        }

        media = MediaFileUpload(file_path, mimetype='text/plain')

        self.__drive_service.files().create(body=file_metadata, media_body=media, fields='id').execute()

    def downloadFile(self, file_id, path_to_download=DRIVE_DOWNLOAD_PATH):
        try:
            # pylint: disable=maybe-no-member
            file_to_download = [
                data for data in self.__drive_data_array if data['id'] == str(file_id)]
            if not file_to_download:
                print(f"No file found with ID: {file_id}")
                return

            file_to_download = file_to_download[0]  # Access the first element
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
        with open(f"{path_to_download}/{file_to_download['name']}", 'wb') as f:
            f.write(downloaded_file.read())
            print(f"File downloaded and saved to {path_to_download}")

    # PRIVATE METHODS
    def __getFilesFromFolder(self, folder_name):
        return self.__drive_service.files().list(q="'" + folder_name + "' in parents", pageSize=10, fields="nextPageToken, files(id, name)").execute()

    def __getFileType(self, file):
        if '.' in file["name"]:
            return "file"
        else:
            return "folder"

    def __getDriveData(self, file=DRIVE_BASE_FILE):

        folder_data = self.__getFilesFromFolder(file["id"])
        json_file = {
            'name': file['name'],
            'id': file['id'],
            'type': self.__getFileType(file),
            'children': []
        }

        self.__drive_data_array.append(json_file)
        if json_file['type'] == "folder":
            json_file['children'].extend(self.__getDriveData(file)
                                         for file in folder_data['files'])

        return json_file


# Object to be imported in other modules
gDrive = GDriveAPI.getInstance(CREDS_PATH)

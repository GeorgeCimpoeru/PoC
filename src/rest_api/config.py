import os


# For Swagger Doc
YAML_FILE_PATH = os.path.abspath('utils/docs/api_doc.yaml')


class Config:
    CAN_CHANNEL = 'vcan1'
    BUS_RECEIVE_TIMEOUT = 99

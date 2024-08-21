import os


# For Swagger Doc
YAML_FILE_PATH = os.path.abspath('utils/docs/api_doc.yaml')


class Config:
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'you-will-never-guess'
    DATABASE_URI = 'sqlite:///mcu_ids.db'
    CAN_CHANNEL = 'vcan1'
    BUS_RECEIVE_TIMEOUT = 99

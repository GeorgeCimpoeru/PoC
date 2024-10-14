import os
from flask import request
from flask_limiter import Limiter


# For Swagger Doc
YAML_FILE_PATH = os.path.abspath('utils/docs/api_doc.yaml')


limiter = Limiter(key_func=lambda: request.remote_addr)


class Config:
    CAN_CHANNEL = 'vcan1'
    BUS_RECEIVE_TIMEOUT = 99

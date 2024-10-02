import os


# For Swagger Doc
YAML_FILE_PATH = os.path.abspath('utils/docs/api_doc.yaml')


class Config:
    CAN_CHANNEL = 'vcan1'
    BUS_RECEIVE_TIMEOUT = 99
    mode = 'test'
    # mode='release' # change to this mode when api project runs on raspberry pi
    UDP_IP = "192.168.241.255"
    UDP_CAN_TO_UDP_PORT = 5001
    UDP_IP = "0.0.0.0"
    UDP_UDP_TO_CAN_PORT = 5002

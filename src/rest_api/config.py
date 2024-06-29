import os

class Config:
    SECRET_KEY = os.environ.get('SECRET_KEY') or 'you-will-never-guess'
    DATABASE_URI = 'sqlite:///mcu_ids.db'
    CAN_INTERFACE = 'vcan0'
    VIRTUAL_INTERFACE = True
    
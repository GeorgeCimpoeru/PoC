import sys
import os
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
sys.path.append(PROJECT_ROOT)
from flask import request, jsonify, Blueprint  # noqa: E402
from actions.request_id_action import RequestIdAction  # noqa: E402
from actions.update_action import Updates  # noqa: E402
from actions.read_info_action import *  # noqa: E402
from utils.logger import log_memory  # noqa: E402
from actions.manual_send_frame import manual_send_frame  # noqa: E402
from actions.write_info_action import WriteInfo  # noqa: E402
from src.ota.google_drive_api.GoogleDriveApi import gDrive  # noqa: E402
from actions.secure_auth import Auth  # noqa: E402

api_bp = Blueprint('api', __name__)


@api_bp.route('/request_ids', methods=['GET'])
def request_ids():
    requester = RequestIdAction(my_id=0xFA99)
    response = requester.read_ids()
    return jsonify(response)


@api_bp.route('/update_to_version', methods=['POST'])
def update_to_version():
    data = request.get_json()
    sw_file_type = data.get('update_file_type')
    sw_file_version = data.get('update_file_version')
    ecu_id = data.get('ecu_id')
    updater = Updates(my_id=0xFA, id_ecu=[0x10, 0x11, 0x12, 0x13])
    response = updater.update_to(type=sw_file_type,
                                 version=sw_file_version,
                                 id=ecu_id)
    return jsonify(response)


@api_bp.route('/read_info_battery', methods=['GET'])
def read_info_bat():
    reader = ReadInfo(0xFA, [0x10, 0x11, 0x12])
    response = reader.read_from_battery()
    return jsonify(response)


@api_bp.route('/read_info_engine', methods=['GET'])
def read_info_eng():
    reader = ReadInfo(API_ID, [0x10, 0x11, 0x12])
    response = reader.read_from_engine()
    return jsonify(response)


@api_bp.route('/read_info_doors', methods=['GET'])
def read_info_doors():
    reader = ReadInfo(API_ID, [0x10, 0x11, 0x12])
    response = reader.read_from_doors()
    return jsonify(response)


@api_bp.route('/send_frame', methods=['POST'])
def send_frame():
    data = request.get_json()
    can_id = data.get('can_id')
    can_data = data.get('can_data')
    return jsonify(manual_send_frame(can_id, can_data))


@api_bp.route('/write_info_doors', methods=['POST'])
def write_info_doors():
    data = request.get_json()

    writer = WriteInfo(API_ID, [0x10, 0x11, 0x12], data)
    response = writer.write_to_doors()
    return jsonify(response)


@api_bp.route('/write_info_battery', methods=['POST'])
def write_info_battery():
    data = request.get_json()
    writer = WriteInfo(API_ID, [0x10, 0x11, 0x12], data)
    response = writer.write_to_battery()
    return jsonify(response)


@api_bp.route('/logs')
def get_logs():
    return jsonify({'logs': log_memory})


# Google Drive API Endpoints
@api_bp.route('/drive_update_data', methods=['GET'])
def update_drive_data():
    drive_data_json = gDrive.getDriveData()
    return jsonify(drive_data_json)


@api_bp.route('/authenticate', methods=['GET'])
def authenticate():
    try:
        auth = Auth(API_ID, [0x10, 0x11, 0x12])
        response_json = auth._auth_to()
        return jsonify(response_json), 200

    except CustomError as e:
        return jsonify(e.message), 400

    except Exception as e:
        return jsonify({"error": str(e)}), 500

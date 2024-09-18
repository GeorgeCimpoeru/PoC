import sys
from configs.data_identifiers import data_identifiers
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
from actions.dtc_info import DiagnosticTroubleCode  # noqa: E402
from actions.diag_session import SessionManager  # noqa: E402
from actions.tester_present import Tester  # noqa: E402
from actions.access_timing_action import *  # noqa: E402
from actions.ecu_reset import Reset  # noqa: E402

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
    identifier = request.args.get('identifier', default=None, type=str)
    response = reader.read_from_battery(identifier)
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
    response = writer.write_to_battery(data)
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


@api_bp.route('/read_dtc_info', methods=['GET'])
def read_dtc_info():
    try:
        reader = DiagnosticTroubleCode(API_ID, [0x10, 0x11, 0x12])
        response_json = reader.read_dtc_info()
        return jsonify(response_json), 200

    except CustomError as e:
        return jsonify(e.message), 400

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@api_bp.route('/clear_dtc_info', methods=['GET'])
def clear_dtc_info():
    try:
        clearer = DiagnosticTroubleCode(API_ID, [0x10, 0x11, 0x12])
        response_json = clearer.clear_dtc_info()
        return jsonify(response_json), 200

    except CustomError as e:
        return jsonify(e.message), 400

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@api_bp.route('/change_session', methods=['POST'])
def change_session():
    data = request.get_json()
    sub_funct = data.get('sub_funct')
    session = SessionManager(API_ID)
    response = session._change_session(id, sub_funct)
    return jsonify(response)


@api_bp.route('/tester_present', methods=['GET'])
def get_tester_present():
    try:
        tester = Tester(API_ID, [0x10, 0x11, 0x12])
        response = tester.is_present()
        return jsonify(response), 200

    except CustomError as e:
        return jsonify(e.message), 400

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@api_bp.route('/get_identifiers', methods=['GET'])
def get_data_identifiers():
    try:
        return jsonify(data_identifiers)
    except CustomError as e:
        return jsonify(e.message), 400
    except Exception as e:
        return jsonify({"error": str(e)}), 500


@api_bp.route('/read_access_timing', methods=['POST'])
def access_timing():
    data = request.get_json()
    sub_funct = data.get('sub_funct')
    if sub_funct is None:
        return jsonify({"status": "error", "message": "Missing 'sub_funct' parameter"}), 400
    requester = ReadAccessTiming(API_ID, [0x10, 0x11, 0x12])
    response = requester._read_timing_info(id, sub_funct)
    return jsonify(response)


@api_bp.route('/reset_ecu', methods=['POST'])
def reset_module():
    data = request.get_json()
    type_reset = data.get('type_reset')
    wh_id = data.get('ecu_id')
    reseter = Reset(API_ID, [0x10, 0x11, 0x12])
    response = reseter.reset_ecu(wh_id, type_reset)
    return jsonify(response)


@api_bp.route('/write_timing', methods=['POST'])
def write_timing():
    data = request.get_json()

    if not data or 'p2_max' not in data or 'p2_star_max' not in data:
        return jsonify({"status": "error", "message": "Missing required parameters"}), 400

    p2_max = data.get('p2_max')
    p2_star_max = data.get('p2_star_max')

    timing_values = {
        "p2_max": p2_max,
        "p2_star_max": p2_star_max
    }

    writer = WriteAccessTiming(API_ID, [0x10, 0x11, 0x12])
    result = writer._write_timing_info(id, timing_values)

    return jsonify(result)

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
from actions.security_decorator import *  # noqa: E402
from utils.input_validation import validate_update_request  # noqa: E402
from config import *  # noqa: E402

api_bp = Blueprint('api', __name__)


@api_bp.route('/request_ids', methods=['GET'])
def request_ids():

    man_flow = request.args.get('is_manual_flow', default='false').lower() == 'false'
    if man_flow is None:
        return jsonify({"status": "error", "message": "'is_manual_flow' query parameter is required"}), 400

    if man_flow == 'false':
        session = SessionManager()
        session._change_session(2)

        auth = Auth()
        auth._auth_to()

        requester = RequestIdAction()
        response = requester.read_ids()
    else:
        requester = RequestIdAction()
        response = requester.read_ids()
    return jsonify(response)


@api_bp.route('/update_to_version', methods=['POST'])
@validate_update_request
def update_to_version():
    data = request.get_json()
    sw_file_type = data.get('update_file_type')
    sw_file_version = data.get('update_file_version')
    ecu_id = data.get('ecu_id')
    updater = Updates()
    response = updater.update_to(type=sw_file_type,
                                 version=sw_file_version,
                                 id=ecu_id)
    return jsonify(response)


@api_bp.route('/read_info_battery', methods=['GET'])
@requires_auth
def read_info_bat():
    reader = ReadInfo()
    item = request.args.get('item', default=None, type=str)
    response = reader.read_from_battery(item)
    return jsonify(response)


@api_bp.route('/read_info_engine', methods=['GET'])
@requires_auth
def read_info_eng():
    reader = ReadInfo()
    item = request.args.get('item', default=None, type=str)
    response = reader.read_from_engine(item)
    return jsonify(response)


@api_bp.route('/read_info_doors', methods=['GET'])
@requires_auth
def read_info_doors():
    reader = ReadInfo()
    item = request.args.get('item', default=None, type=str)
    response = reader.read_from_doors(item)
    return jsonify(response)


@api_bp.route('/read_info_hvac', methods=['GET'])
@requires_auth
def read_info_hvac():
    reader = ReadInfo()
    item = request.args.get('item', default=None, type=str)
    response = reader.read_from_hvac(item)
    return jsonify(response)


@api_bp.route('/send_frame', methods=['POST'])
def send_frame():
    data = request.get_json()
    can_id = data.get('can_id')
    can_data = data.get('can_data')
    response = manual_send_frame(can_id, can_data)
    return jsonify(response)


@api_bp.route('/write_info_doors', methods=['POST'])
@requires_auth
def write_info_doors():
    data = request.get_json()
    writer = WriteInfo(data)
    response = writer.write_to_doors(data)
    return jsonify(response)


@api_bp.route('/write_info_battery', methods=['POST'])
@requires_auth
def write_info_battery():
    data = request.get_json()
    writer = WriteInfo(data)
    response = writer.write_to_battery(data)
    return jsonify(response)


@api_bp.route('/write_info_engine', methods=['POST'])
@requires_auth
def write_info_engine():
    data = request.get_json()
    writer = WriteInfo(data)
    response = writer.write_to_engine(data)
    return jsonify(response)


@api_bp.route('/write_info_hvac', methods=['POST'])
@requires_auth
def write_info_hvac():
    data = request.get_json()
    writer = WriteInfo(data)
    response = writer.write_to_hvac(data)
    return jsonify(response)


@api_bp.route('/logs')
def get_logs():
    """ curl -X GET http://127.0.0.1:5000/api/logs """
    response = log_memory
    return jsonify({'logs': response})


@api_bp.route('/drive_update_data', methods=['GET'])
def update_drive_data():
    """ curl -X GET http://127.0.0.1:5000/api/drive_update_data """
    drive_data_json = gDrive.getDriveData()
    return jsonify(drive_data_json)


@api_bp.route('/authenticate', methods=['GET'])
def authenticate():
    try:
        auth = Auth()
        response_json = auth._auth_to()
        return jsonify(response_json), 200

    except CustomError as e:
        return jsonify(e.message), 400

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@api_bp.route('/read_dtc_info', methods=['GET'])
def read_dtc_info():
    try:
        reader = DiagnosticTroubleCode()
        response_json = reader.read_dtc_info()
        return jsonify(response_json), 200

    except CustomError as e:
        return jsonify(e.message), 400

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@api_bp.route('/clear_dtc_info', methods=['GET'])
def clear_dtc_info():
    try:
        clearer = DiagnosticTroubleCode()
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
    session = SessionManager()
    response = session._change_session(sub_funct)
    return jsonify(response)


@api_bp.route('/tester_present', methods=['GET'])
def get_tester_present():
    try:
        tester = Tester()
        response = tester.is_present()
        return jsonify(response), 200

    except CustomError as e:
        return jsonify(e.message), 400

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@api_bp.route('/get_identifiers', methods=['GET'])
def get_data_identifiers():
    """ curl -X GET http://127.0.0.1:5000/api/get_identifiers """
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
    requester = ReadAccessTiming()
    response = requester._read_timing_info(id, sub_funct)
    return jsonify(response)


@api_bp.route('/reset_ecu', methods=['POST'])
def reset_module():
    data = request.get_json()
    type_reset = data.get('type_reset')
    wh_id = data.get('ecu_id')
    reseter = Reset()
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

    writer = WriteAccessTiming()
    result = writer._write_timing_info(id, timing_values)

    return jsonify(result)

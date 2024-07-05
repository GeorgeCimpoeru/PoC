from flask import request, jsonify, Blueprint
from . import api_bp
from actions.request_id_action import RequestIdAction
from actions.update_action import Updates
from config import Config
from actions.generate_frames import GenerateFrame
from actions.read_info_action import *
from utils.logger import log_memory


api_bp = Blueprint('api', __name__)

@api_bp.route('/request_ids', methods=['POST'])
def request_ids():
    requester = RequestIdAction(my_id=0xFA)
    response = requester.run()
    return jsonify(response)

@api_bp.route('/update_to_version', methods=['POST'])
def update_to_version():
    data = request.get_json()
    ecu_id = data.get('ecu_id')
    version = data.get('version')
    updater = Updates(0x23, 0x12)
    response = updater.update_to(int(ecu_id), int(version))
    return jsonify(response)

@api_bp.route('/read_info_battery', methods=['GET'])
def read_info_bat():
    updater = ReadInfo(0x23, [0x11, 0x12, 0x13])
    response = updater.read_from_battery()
    return jsonify(response)

@api_bp.route('/send_frame', methods=['POST'])
def send_frame():
    data = request.get_json()

    try:
        bus = can.interface.Bus(channel=Config.CAN_CHANNEL, bustype='socketcan')

        can_id = int(data.get('can_id'), 16)
        can_data = [int(byte, 16) for byte in data.get('can_data').split(',')]
        
        if can_id > 0xFFFF or len(can_data) > 8:
            raise ValueError("CAN ID or Data out of bounds")
        
        generator = GenerateFrame()
        generator.send_frame(can_id, can_data)

        received_frame = bus.recv(timeout=15)
        
        if received_frame is not None:
            received_data = {
                'can_id': hex(received_frame.arbitration_id),
                'can_data': [hex(byte) for byte in received_frame.data]
            }
        else:
            received_data = None

        return jsonify({'response': received_data})
    
    except ValueError as e:
        return jsonify({'status': 'Error', 'message': str(e)}), 400
    except Exception as e:
        return jsonify({'status': 'Error', 'message': str(e)}), 500
    finally:
        bus.shutdown()
        
@api_bp.route('/logs')
def get_logs():
    return jsonify({'logs': log_memory})

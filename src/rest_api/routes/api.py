from flask import request, jsonify, Blueprint
from . import api_bp
from can_interface import initialize_can_interface
from actions.request_id_action import FrameRequester
from actions.update_action import Updates
from config import Config
from actions.generate_frames import GenerateFrame
from app import bus

api_bp = Blueprint('api', __name__)
# bus = initialize_can_interface(Config.CAN_INTERFACE, Config.VIRTUAL_INTERFACE)

@api_bp.route('/request_ids', methods=['POST'])
def request_ids():
    # bus = initialize_can_interface(Config.CAN_INTERFACE, True)
    requester = FrameRequester(bus)
    response = requester.request_ids()
    return jsonify(response)

@api_bp.route('/update_to_version', methods=['POST'])
def update_to_version():
    # bus = initialize_can_interface(Config.CAN_INTERFACE, True)
    data = request.get_json()
    version = data.get('version')
    updater = Updates(bus, 0x23, 0x12)
    response = updater.update_to(version)
    return jsonify(response)

@api_bp.route('/send_frame', methods=['POST'])
def send_frame():
    # bus = initialize_can_interface(Config.CAN_INTERFACE, True)
    data = request.get_json()
    try:
        can_id = int(data.get('can_id'), 16)
        can_data = [int(byte, 16) for byte in data.get('can_data').split(',')]
        
        if can_id > 0xFFFF or len(can_data) > 8:
            raise ValueError("CAN ID or Data out of bounds")

        generator = GenerateFrame(bus)
        generator.send_frame(can_id, can_data)
        return jsonify({'status': 'Frame sent'})
    except ValueError as e:
        return jsonify({'status': 'Error', 'message': str(e)}), 400
from flask import request, jsonify, Blueprint
from actions.request_id_action import RequestIdAction
from actions.update_action import Updates
from config import Config
from actions.generate_frames import GenerateFrame
from actions.read_info_action import *
from utils.logger import log_memory

api_bp = Blueprint('api', __name__)


@api_bp.route('/request_ids', methods=['GET'])
def request_ids():
    """
    Request IDs.
    ---
    responses:
      200:
        description: IDs requested successfully
    """
    requester = RequestIdAction(my_id=0xFA)
    response = requester.run()
    return jsonify(response)


@api_bp.route('/update_to_version', methods=['POST'])
def update_to_version():
    """
    Update ECU to a specific version.
    ---
    parameters:
      - name: body
        in: body
        required: true
        schema:
          type: object
          properties:
            ecu_id:
              type: string
            version:
              type: string
    responses:
      200:
        description: Update successful
    """
    data = request.get_json()
    ecu_id = data.get('ecu_id')
    version = data.get('version')
    updater = Updates(0x23, 0x12)
    response = updater.update_to(int(ecu_id), int(version))
    return jsonify(response)


@api_bp.route('/read_info_doors', methods=['GET'])
def read_info_doors():
    """
    Read information from the doors.
    ---
    responses:
      200:
        description: Information retrieved successfully
        schema:
          type: object
          properties:
            Door_param:
              type: integer
            Serial_number:
              type: string
            Cigarette_Lighter_Voltage:
              type: number
              format: float
            Light_state:
              type: string
            BeltCard:
              type: string
            WindowStatus:
              type: string
      """


@api_bp.route('/read_info_engine', methods=['GET'])
def read_info_eng():
    """
    Read information from the engine.
    ---
    responses:
      200:
        description: Information retrieved successfully
        schema:
          type: object
          properties:
            power_output:
              type: integer
            weight:
              type: integer
            fuel_consumption:
              type: integer
            torque:
              type: integer
            fuel_used:
              type: integer
            state_of_running:
              type: integer
            current_speed:
              type: integer
            engine_state:
              type: integer
            serial_number:
              type: integer
    """
    reader = ReadInfo(0x23, [0x11, 0x12, 0x13])
    response = reader.read_from_engine()
    return jsonify(response)


@api_bp.route('/read_info_battery', methods=['GET'])
def read_info_bat():
    """
    Read information from the battery.
    ---
    responses:
      200:
        description: Information retrieved successfully
        schema:
          type: object
          properties:
            battery_level:
              type: integer
            voltage:
              type: integer
            battery_state_of_charge:
              type: integer
            temperature:
              type: integer
            life_cycle:
              type: integer
            fully_charged:
              type: boolean
            serial_number:
              type: string
            range_battery:
              type: integer
            charging_time:
              type: integer
            device_consumption:
              type: integer
            time_stamp:
              type: string
              format: date-time
    """
    reader = ReadInfo(0x23, [0x11, 0x12, 0x13])
    response = reader.read_from_battery()
    return jsonify(response)


@api_bp.route('/send_frame', methods=['POST'])
def send_frame():
    """
    Send a CAN frame.
    ---
    parameters:
      - name: body
        in: body
        required: true
        schema:
          type: object
          properties:
            can_id:
              type: string
            can_data:
              type: string
    responses:
      200:
        description: Frame sent successfully
    """
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


@api_bp.route('/logs', methods=['GET'])
def get_logs():
    """
    Get application logs.
    ---
    responses:
      200:
        description: Logs retrieved successfully
        schema:
          type: object
          properties:
            logs:
              type: array
              items:
                type: string
    """
    return jsonify({'logs': log_memory})

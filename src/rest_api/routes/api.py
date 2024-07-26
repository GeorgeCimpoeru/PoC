from flask import request, jsonify, Blueprint
from actions.request_id_action import RequestIdAction
from actions.update_action import Updates
from actions.read_info_action import *
from utils.logger import log_memory
from actions.manual_send_frame import manual_send_frame
from actions.write_info_action import WriteInfo


api_bp = Blueprint('api', __name__)


ecu_ids = [0x10, 0x11, 0x12]


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
    updater = Updates(my_id=0xFA, id_ecu=ecu_id)
    response = updater.update_to(ecu_id=int(ecu_id), version=int(version))
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
    reader = ReadInfo(0xFA, [0x10, 0x11, 0x12])
    response = reader.read_from_battery()
    return jsonify(response)


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
    reader = ReadInfo(0xFA, [0x10, 0x11, 0x12])
    response = reader.read_from_engine()
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
    reader = ReadInfo(0xFA, [0x10, 0x11, 0x12])
    response = reader.read_from_doors()
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
    can_id = data.get('can_id')
    can_data = data.get('can_data')
    return jsonify(manual_send_frame(can_id, can_data))


@api_bp.route('/write_info_doors', methods=['POST'])
def write_info_doors():
    """
    Write information to the doors.
    ---
    parameters:
      - name: body
        in: body
        required: true
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
    responses:
      200:
        description: Information written successfully
        schema:
          type: object
          properties:
            status:
              type: string
            No of errors:
              type: integer
            time_stamp:
              type: string
    """
    data = request.get_json()

    writer = WriteInfo(0xFA, [0x10, 0x11, 0x12], data)
    response = writer.run()
    return jsonify(response)


@api_bp.route('/logs')
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

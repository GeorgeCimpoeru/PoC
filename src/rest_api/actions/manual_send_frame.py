from flask import request, jsonify
import can
from utils.logger import *
from config import Config
from actions.base_actions import *

def manual_send_frame(can_id, can_data):
    data = request.get_json()

    try:
        bus = can.interface.Bus(channel=Config.CAN_CHANNEL, bustype='socketcan')

        can_id = int(data.get('can_id'), 16)
        can_data = [int(byte, 16) for byte in data.get('can_data').split(',')]

        if can_id > 0xFFFF or len(can_data) > 8:
            raise ValueError("CAN ID or Data out of bounds")

        message = can.Message(arbitration_id=can_id, data=can_data, is_extended_id=True)
        bus.send(message)
        received_frame = bus.recv(timeout=15)

        if received_frame is not None:
            received_data = {
                'can_id': hex(received_frame.arbitration_id),
                'can_data': [hex(byte) for byte in received_frame.data]
            }

            if received_frame.data[1] == 0x67 and \
                received_frame.data[2] == 0x01 and \
                    received_frame.data[3] == 0x00 or \
                        received_frame.data[1] == 0x67 and received_frame.data[2] == 0x02:
                    log_info_message(logger, "Authentication successful")
                    received_data['auth_status'] = 'success'

            if received_frame.data[1] == 0x7F:  # Diagnostic negative response
                nrc = received_frame.data[3]
                if nrc == 0x37:
                    time_delay_ms = int.from_bytes(received_frame.data[4:8], byteorder='big')
                    time_delay_s = time_delay_ms / 1000
                    log_info_message(logger, f"Retries exceeded. Try again in : {time_delay_s} s")
                    received_data['retry_timeout_ms'] = time_delay_ms
                else:
                    error_text = handle_negative_response(received_frame.data)
                    received_data['auth_status'] = 'failed'
                    received_data['error_text'] = error_text
        else:
            received_data = None

        return jsonify({'response': received_data})

    except ValueError as e:
        return jsonify({'status': 'Error', 'message': str(e)}), 400
    except Exception as e:
        return jsonify({'status': 'Error', 'message': str(e)}), 500
    finally:
        bus.shutdown()


def handle_negative_response(frame_response):
    """
    Handles the negative response scenarios.
    """
    negative_responses = {
        0x12: "SubFunctionNotSupported",
        0x13: "IncorrectMessageLengthOrInvalidFormat",
        0x24: "RequestSequenceError",
        0x35: "InvalidKey",
        0x36: "ExceededNumberOfAttempts",
        0x37: "RequiredTimeDelayNotExpired"
    }
    nrc = frame_response[3]
    error_message = negative_responses.get(nrc, "Unknown error")
    log_error_message(logger, f"Authentication failed: {error_message}")
    return error_message
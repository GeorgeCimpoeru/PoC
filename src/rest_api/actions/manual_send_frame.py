from flask import request
import can
from utils.logger import *
from config import Config
from actions.base_actions import *
from can_bridge import CanBridge


bridge = CanBridge()


def manual_send_frame(can_id, can_data):
    log_info_message(logger, "Starting manual_send_frame function")
    data = request.get_json()
    error_text = None
    try:
        log_info_message(logger, f"Attempting to connect to CAN bus on channel: {Config.CAN_CHANNEL}")
        bus = bridge.get_bus()
        log_info_message(logger, "Successfully connected to CAN bus")

        can_id = int(data.get('can_id'), 16)
        can_data = [int(byte, 16) for byte in data.get('can_data').split(',')]
        log_info_message(logger, f"Parsed CAN ID: {hex(can_id)}, CAN Data: {[hex(b) for b in can_data]}")

        if can_id > 0xFFFF or len(can_data) > 8:
            log_error_message(logger, f"Invalid CAN ID or Data: ID={hex(can_id)}, \
                              Data Length={len(can_data)}")
            raise ValueError("CAN ID or Data out of bounds")

        message = can.Message(arbitration_id=can_id, data=can_data, is_extended_id=True)
        log_info_message(logger, f"Sending CAN message: ID={hex(message.arbitration_id)}, \
                         Data={[hex(b) for b in message.data]}")
        bus.send(message)

        received_frames = []  # List to store all received frames
        log_info_message(logger, "Starting to receive CAN frames")
        while True:
            received_frame = bus.recv(3)
            if received_frame is None:
                log_info_message(logger, "No more frames received, exiting receive loop")
                break

            log_info_message(logger, f"Received frame: ID={hex(received_frame.arbitration_id)}, \
                             Data={[hex(b) for b in received_frame.data]}")
            received_data = {
                'can_id': hex(received_frame.arbitration_id).upper(),
                'can_data': [hex(byte) for byte in received_frame.data]
            }

            # Authentication success case
            if received_frame.data[1] == 0x67 and \
                (received_frame.data[2] in {0x01, 0x02}) and \
                    received_frame.data[3] == 0x00:
                log_info_message(logger, "Authentication successful")

                received_data['auth_status'] = 'success'

            # Handle negative response codes
            elif received_frame.data[1] == 0x7F:
                nrc = received_frame.data[3]
                service_id = received_frame.data[2]
                error_handler = Action(can_id, [0x10, 0x11, 0x12, 0x13])
                error_text = error_handler.handle_negative_response(nrc, service_id)

                if nrc == 0x37:
                    time_delay_ms = int.from_bytes(received_frame.data[4:8], byteorder='big')
                    time_delay_s = time_delay_ms / 1000
                    log_warning_message(logger, f"Retries exceeded. Try again in: {time_delay_s} s")
                    received_data['retry_timeout_ms'] = time_delay_ms
                else:
                    received_data['auth_status'] = 'failed'
                    received_data['error_text'] = error_text
                    log_error_message(logger, f"Authentication failed: {error_text}")

            received_frames.append(received_data)
            log_error_message(logger, f"Authentication failed: {error_text}")

        log_info_message(logger, f"Total frames received: {len(received_frames)}")
        return {'response': received_frames}

    except ValueError as e:
        log_error_message(logger, f"ValueError occurred: {str(e)}")
        return {'message': str(e)}, 400
    except Exception as e:
        log_error_message(logger, f"Unexpected error occurred: {str(e)}")
        return {'message': str(e)}, 500
    finally:
        log_info_message(logger, "Shutting down CAN bus connection")
        bus.shutdown()

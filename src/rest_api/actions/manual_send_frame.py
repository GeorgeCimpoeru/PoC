from flask import request
import can
from utils.logger import *
from config import Config
from actions.base_actions import *


def manual_send_frame(can_id, can_data):
    log_info_message(logger, "Starting manual_send_frame function")
    data = request.get_json()
    error_text = None
    try:
        log_info_message(logger, f"Attempting to connect to CAN bus on channel: {Config.CAN_CHANNEL}")
        bus = can.interface.Bus(channel=Config.CAN_CHANNEL, bustype='socketcan')
        log_info_message(logger, "Successfully connected to CAN bus")

        can_id = int(data.get('can_id'), 16)
        can_data = [int(byte, 16) for byte in data.get('can_data').split(',')]
        log_info_message(logger, f"Parsed CAN ID: {hex(can_id)}, CAN Data: {[hex(b) for b in can_data]}")

        if can_id > 0xFFFF or len(can_data) > 8:
            log_error_message(logger, f"Invalid CAN ID or Data: ID={hex(can_id)}, Data Length={len(can_data)}")
            raise ValueError("CAN ID or Data out of bounds")

        message = can.Message(arbitration_id=can_id, data=can_data, is_extended_id=True)
        log_info_message(logger, f"Sending CAN message: ID={hex(message.arbitration_id)}, Data={[hex(b) for b in message.data]}")
        bus.send(message)

        received_frames = []  # List to store all received frames
        log_info_message(logger, "Starting to receive CAN frames")
        while True:
            received_frame = bus.recv(3)
            if received_frame is None:
                log_info_message(logger, "No more frames received, exiting receive loop")
                break

            log_info_message(logger, f"Received frame: ID={hex(received_frame.arbitration_id)}, Data={[hex(b) for b in received_frame.data]}")
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
                error_text = handle_negative_response(nrc, service_id)

                received_data['error'] = error_text

                if nrc == 0x37:  # Specific handling for "RequiredTimeDelayNotExpired"
                    time_delay_ms = int.from_bytes(received_frame.data[4:8], byteorder='big')
                    time_delay_s = time_delay_ms / 1000
                    log_info_message(logger, f"Retries exceeded. Try again in: {time_delay_s} s")
                    received_data['retry_timeout_ms'] = time_delay_ms
                else:
                    # error_text = handle_negative_response(received_frame.data, )
                    received_data['auth_status'] = 'failed'
                    received_data['error_text'] = error_text
                    log_error_message(logger, f"Authentication failed: {error_text}")

            received_frames.append(received_data)
            log_error_message(logger, f"Authentication failed: {error_text}")

        log_info_message(logger, f"Total frames received: {len(received_frames)}")
        return {'response': received_frames}

    except ValueError as e:
        log_error_message(logger, f"ValueError occurred: {str(e)}")
        return {'status': 'Error', 'message': str(e)}, 400
    except Exception as e:
        log_error_message(logger, f"Unexpected error occurred: {str(e)}")
        return {'status': 'Error', 'message': str(e)}, 500
    finally:
        log_info_message(logger, "Shutting down CAN bus connection")
        bus.shutdown()


def handle_negative_response(nrc, service_id):
    """
    Handles the negative response scenarios for various services.
    """
    service_error_mapping = {
        # Write Data by Identifier (0x2E)
        0x2E: {0x13, 0x31, 0x33},

        # Security Access (0x27)
        0x27: {0x12, 0x24, 0x35, 0x36},

        # Diagnostic Session Control Service (0x10)
        0x10: {0x12},

        # Access Timing Parameter Service (0x83)
        0x83: {0x12, 0x13, 0x78},

        # Clear Diagnostic Information (0x14)
        0x14: {0x13, 0x31},

        # Read DTC Information (0x19)
        0x19: {0x12, 0x13},

        # Routine Control (0x31)
        0x31: {0x12, 0x31},

        # Tester Present (0x3E)
        0x3E: {0x12, 0x13}
    }

    # General negative response codes
    negative_responses = {
        0x12: "SubFunction Not Supported",
        0x13: "Incorrect Message Length Or Invalid Format",
        0x14: "Incorrect Message Length Or Invalid Format",
        0x22: "Conditions Not Correct",
        0x24: "Request Sequence Error",
        0x25: "No Response From Subnet Component",
        0x31: "Request Out Of Range",
        0x33: "Security Access Denied",
        0x34: "Authentication Required",
        0x35: "Invalid Key",
        0x36: "Exceeded Number Of Attempts",
        0x37: "Required Time Delay Not Expired",
        0x70: "Upload Download Not Accepted",
        0x71: "Transfer Data Suspended",
        0x72: "General Programming Failure",
        0x73: "Wrong Block Sequence Counter",
        0x92: "Voltage Too High",
        0x93: "Voltage Too Low",
        0x78: "Request Correctly Received-Response Pending",
        0x7E: "SubFunction Not Supported In Active Session",
        0x7F: "Function Not Supported In Active Session"
    }

    # Check if the NRC is within the allowed list for the service
    if service_id in service_error_mapping and nrc in service_error_mapping[service_id]:
        error_message = negative_responses.get(nrc, "Unknown error")
    else:
        error_message = "Unknown service or error"

    log_error_message(logger, f"Negative response received: NRC={hex(nrc)}, Error={error_message} (Code: {hex(nrc)})")
    return error_message

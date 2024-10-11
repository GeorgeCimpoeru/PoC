import can
import datetime
from actions.generate_frames import GenerateFrame as GF
from utils.logger import *
from config import Config
from configs.data_identifiers import *

logger_singleton = SingletonLogger('base_action.log')
logger = logger_singleton.logger
logger_frame = logger_singleton.logger_frame

SESSION_CONTROL = 0x10
RESET_ECU = 0x11
READ_BY_ADDRESS = 0x23
READ_BY_IDENTIFIER = 0x022
AUTHENTICATION_SEND = 0x27
AUTHENTICATION_RECV = 0x67
AUTHENTICATION_SUBF_REQ_SEED = 0x1
AUTHENTICATION_SUBF_SEND_KEY = 0x2
ROUTINE_CONTROL = 0X31
WRITE_BY_IDENTIFIER = 0X2E
READ_DTC = 0X19
CLEAR_DTC = 0X14
REQUEST_UPDATE_STATUS = 0x32
REQUEST_DOWNLOAD = 0X34
TRANSFER_DATA = 0X36
REQUEST_TRANSFER_EXIT = 0X37
TESTER_PRESENT = 0x3E
ACCESS_TIMING_PARAMETERS = 0X83


class FrameWithData:
    """Base class for frames with data extraction methods."""
    def _data_from_frame(self):
        pass


class ReadByIdentifier(FrameWithData):
    """Extracts data from frames read by identifier."""
    def _data_from_frame(self, msg: can.Message):
        data = msg.data[4:]
        return data


class ReadByAddress(FrameWithData):
    """Extracts data from frames read by address."""
    def _data_from_frame(self, msg: can.Message):
        addr_siz_len = msg.data[2]
        byte_start_data = addr_siz_len % 0x10 + addr_siz_len // 0x10
        data = msg.data[3 + byte_start_data:]
        return data


class AuthenticationSeed(FrameWithData):
    """Extracts seed from frame request seed."""
    def _data_from_frame(self, msg: can.Message):
        data = msg.data[3:]
        return data


class CustomError(Exception):
    """Custom exception class for handling specific update errors."""
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)


class Action(GF):
    """
    Base class for actions involving CAN bus communication.

    Attributes:
    - my_id: Identifier for the device initiating the action.
    - id_ecu: Identifier for the specific ECU being communicated with.
    - g: Instance of GenerateFrame for generating CAN bus frames.
    """

    def __init__(self, my_id, id_ecu: list = []):
        self.my_id = my_id
        self.id_ecu = id_ecu
        self.last_msg = None
        super().__init__()

    def __collect_response(self, sid: int):
        """
        Collects the response message from the CAN bus.

        Args:
        - sid: Service identifier to verify the response.

        Returns:
        - The collected CAN message if valid, otherwise None.
        """
        flag = False
        msg_ext = None
        msg = self.bus.recv(35)
        self.last_msg = msg
        while msg is not None:

            if msg.data[1] == 0x7F and msg.data[3] == 0x78:
                log_info_message(logger, f"Response pending for SID {msg.data[2]:02X}. Waiting for the actual response...")
                msg = self.bus.recv(Config.BUS_RECEIVE_TIMEOUT)
                self.last_msg = msg
                continue
            # First Frame
            if msg.data[0] == 0x10:
                flag = True
                msg_ext = msg[1:]
            # Consecutive frame
            elif flag and 0x20 < msg.data[0] < 0x30:
                msg_ext.data += msg.data[1:]
            # Simple Frame
            else:
                break
            msg = self.bus.recv(Config.BUS_RECEIVE_TIMEOUT)
            self.last_msg = msg
        if flag:
            msg = msg_ext
        if msg is not None and self.__verify_frame(msg, sid):
            return msg
        return None

    def __verify_frame(self, msg: can.Message, sid: int):

        log_info_message(logger, f"Verifying frame with SID: {sid:02X}, \
                         message data: {[hex(byte) for byte in msg.data]}")

        if msg.arbitration_id % 0x100 != self.my_id:
            return False

        if msg.data[1] == 0x7F and msg.data[3] == 0x78:
            return True

        if msg.data[1] == 0x7F:
            return False

        if msg.data[0] != 0x10:
            if msg.data[1] != sid + 0x40:
                return False
        else:
            if msg.data[2] != sid + 0x40:
                return False

        return True

    def _passive_response(self, sid, error_str="Error service"):
        """
        Collects and verifies the response, raising an error if invalid.

        Args:
        - sid: Service identifier to verify the response.
        - error_str: Error message to raise if the response is invalid.

        Raises:
        - CustomError: If the response is invalid.
        """
        log_info_message(logger, "Collecting the response")
        response = self.__collect_response(sid)

        if response is None:
            log_error_message(logger, error_str)
            response_json = self._to_json_error("interrupted", 1)
            raise CustomError(response_json)
        return response

    def _data_from_frame(self, msg: can.Message):
        """
        Extracts data from the CAN message based on the frame type. Implemented for
        ReadByIdentifier and ReadByAddress
        Args:
        - msg: The CAN message to extract data from.

        Returns:
        - The extracted data if the frame type is recognized, otherwise None.
        """
        handlers = {
            0x62: ReadByIdentifier(),
            0x63: ReadByAddress(),
            0x67: AuthenticationSeed(),
        }
        handler = handlers.get(msg.data[1] if msg.data[0] != 0x10 else msg.data[2])
        if handler:
            return handler._data_from_frame(msg)
        return None

    def _read_by_identifier(self, id, identifier):
        """
        Function to read data from a specific identifier. The function requests, reads the data, and processes it.

        Args:
        - identifier: Identifier of the data.

        Returns:
        - Data as a string.
        """
        log_info_message(logger, f"Read from identifier {identifier}")
        self.read_data_by_identifier(id, identifier)
        frame_response = self._passive_response(READ_BY_IDENTIFIER,
                                                f"Error reading data from identifier {identifier}")
        log_info_message(logger, f"Frame response: {frame_response}")
        data = self._data_from_frame(frame_response)
        data_str = self._list_to_number(data)
        return data_str

    def _write_by_identifier(self, id, identifier, value):
        """
        Function to write data to a specific identifier.

        Args:
        - id: Identifier of the request.
        - identifier: Identifier of the data.
        - value: Value to write.

        Returns:
        - True if the operation is triggered.
        """
        log_info_message(logger, f"Write by identifier {identifier}")

        value_list = self._number_to_list(value)

        if isinstance(value_list, list) and len(value_list) > 4:
            self.write_data_by_identifier_long(id, identifier, value_list)
        else:
            self.write_data_by_identifier(id, identifier, value_list)

        self._passive_response(WRITE_BY_IDENTIFIER, f"Operation complete for identifier {identifier}")

        return True

    def _algorithm(self, seed: list):
        """
        Method to generate a key based on the seed.
        """
        return [(~num + 1) & 0xFF for num in seed]

    def _authentication(self, id):
        """
        Function to authenticate. Makes the proper request to the ECU.
        Returns a JSON response with detailed information about the authentication process.
        """
        log_info_message(logger, "Authenticating")
        # Send the request for authentication seed
        self.authentication_seed(id,
                                 sid_send=AUTHENTICATION_SEND,
                                 sid_recv=AUTHENTICATION_RECV,
                                 subf=AUTHENTICATION_SUBF_REQ_SEED)
        frame_response = self._passive_response(AUTHENTICATION_SEND,
                                                "Error requesting seed")

        if frame_response.data[1] == 0x7F:
            nrc_msg = frame_response.data[3]
            sid_msg = frame_response.data[2]
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            return {
                "status": "error",
                "message": "Negative response received while requesting seed",
                "negative_response": negative_response
            }

        if frame_response.data[1] == 0x67 and frame_response.data[2] == 0x01 and frame_response.data[3] == 0x00:
            log_info_message(logger, "Authentication successful")
            return {
                "message": "Authentication successful"
            }

        else:
            # Extract seed and compute key
            seed = self._data_from_frame(frame_response)
            key = self._algorithm(seed)
            log_info_message(logger, f"Key: {key}")

            # Send the key for authentication
            self.authentication_key(id,
                                    key=key,
                                    sid_send=AUTHENTICATION_RECV,
                                    sid_recv=AUTHENTICATION_SEND,
                                    subf=AUTHENTICATION_SUBF_SEND_KEY)
            frame_response = self._passive_response(AUTHENTICATION_SEND,
                                                    "Error sending key")

            if frame_response.data[1] == 0x7F:
                nrc_msg = frame_response.data[3]
                sid_msg = frame_response.data[2]
                negative_response = self.handle_negative_response(nrc_msg, sid_msg)
                return {
                    "status": "error",
                    "message": "Negative response received while sending key",
                    "negative_response": negative_response
                }

            if frame_response.data[1] == 0x67 and frame_response.data[2] == 0x02:
                log_info_message(logger, "Authentication successful")
                return {
                    "message": "Authentication successful",
                }
            else:
                log_info_message(logger, "Authentication failed")
                return {
                    "message": "Authentication failed",
                }

    def handle_negative_response(self, nrc_msg, sid_msg):
        """
        Handles the negative response scenarios for various services.
        """
        nrc = nrc_msg
        service_id = sid_msg

        service_error_mapping = {
            0x2E: "Write Data by Identifier",
            0x27: "Security Access",
            0x10: "Diagnostic Session Control",
            0x11: "Ecu Reset",
            0x83: "Access Timing Parameter",
            0x14: "Clear Diagnostic Information",
            0x19: "Read DTC Information",
            0x22: "Read by ID",
            0x31: "Routine Control",
            0x3E: "Tester Present"
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

        error_message = negative_responses.get(nrc, "Unknown error")
        service_description = service_error_mapping.get(service_id, "Unknown service")

        logger.error(f"Negative response received: Service={service_description}, NRC={hex(nrc)}, Error={error_message}")

        response = {
            "service_id": hex(service_id),
            "nrc": hex(nrc),
            "service_description": service_description,
            "error_message": error_message
        }

        return response

    def _to_json(self, status, no_errors):
        response_to_frontend = {
            "status": status,
            "No of errors": no_errors,
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return response_to_frontend

    def _to_json_error(self, error, no_errors):
        response_to_frontend = {
            "ERROR": error,
            "No of errors": no_errors,
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return response_to_frontend

    def _list_to_number(self, list: list):
        number = ""
        for item in list:
            if item <= 0xF:
                number += "0"
            number += hex(item)[2:]
        return number

    def _number_to_list(self, number: int) -> list:
        list = []
        while number:
            list.append(number % 0x100)
            number = number//0x100
        return list[::-1]

    def _number_to_byte_list(self, number: int):
        """Converts a number to a list of bytes."""
        byte_list = []
        while number > 0:
            byte_list.insert(0, number & 0xFF)
            number = number >> 8
        return byte_list

    def hex_to_dec(self, value):
        """Helper function to convert hex to decimal if not 'No data'."""
        if value is None or value == "No data":
            return "No data"

        try:
            return int(value, 16)
        except (ValueError, TypeError):
            return value

"""
Author: Mujdei Ruben

Implement the Action class in your classes that perform actions.
"""

import can
from generate_frames import GenerateFrame as GF

IDENTIFIER_VERSION_SOFTWARE_MCU = 0x1010

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

class CustomError(Exception):
    """Custom exception class for handling specific update errors."""
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)

class Action:
    """
    Base class for actions involving CAN bus communication.
    
    Attributes:
    - bus: CAN bus interface for communication.
    - my_id: Identifier for the device initiating the action.
    - id_ecu: Identifier for the specific ECU being communicated with.
    - g: Instance of GenerateFrame for generating CAN bus frames.
    """

    def __init__(self, bus, my_id, id_ecu):
        self.bus = bus
        self.my_id = my_id
        self.id_ecu = id_ecu
        self.g = GF(bus)

    def _collect_response(self, sid: int):
        """
        Collects the response message from the CAN bus.
        
        Args:
        - sid: Service identifier to verify the response.

        Returns:
        - The collected CAN message if valid, otherwise None.
        """
        flag = False
        msg_ext = None
        msg = self.bus.recv(3)
        while msg is not None:
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
            msg = self.bus.recv(3)
        if flag:
            msg = msg_ext
        if msg is not None and self._verify_frame(msg, sid):
            return msg
        return None

    def _verify_frame(self, msg: can.Message, sid: int):
        """
        Verifies the validity of the received CAN message.
        
        Args:
        - msg: The received CAN message.
        - sid: Service identifier to verify the response.

        Returns:
        - True if the frame is valid, False otherwise.
        """
        if msg.arbitration_id % 0x100 != self.my_id:
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
        response = self._collect_response(sid)
        if response is None:
            print(error_str)
            response_json = self._to_json("interrupted", 1)
            raise CustomError(response_json)
        return response

    def _data_from_frame(self, msg: can.Message):
        """
        Extracts data from the CAN message based on the frame type.
        
        Args:
        - msg: The CAN message to extract data from.

        Returns:
        - The extracted data if the frame type is recognized, otherwise None.
        """
        handlers = {
            0x62: ReadByIdentifier(),
            0x63: ReadByAddress(),
        }
        handler = handlers.get(msg.data[1] if msg.data[0] != 0x10 else msg.data[2])
        if handler:
            return handler._data_from_frame(msg)
        return None

    # Implement in the child class
    def _to_json(self, status, no_errors):
        pass
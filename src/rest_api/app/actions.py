'''
Author: Mujdei Ruben

'''

import can
from generate_frames import GenerateFrame as GF

IDENTIFIER_VERSION_SOFTWARE_MCU = 0x1010

class FrameWithData:
    def _data_from_frame(self):
        pass
class ReadByIdentifier(FrameWithData):
    def _data_from_frame(self,msg: can.Message):
        data = msg.data[4:]
        return data
class ReadByAddress(FrameWithData):
    def _data_from_frame(self,msg: can.Message):
        addr_siz_len = msg.data[2]
        byte_start_data = addr_siz_len % 0x10 + addr_siz_len // 0x10
        data = msg.data[3 + byte_start_data:]
        return data
class CustomError(Exception):
    def __init__(self, message):
        self.message = message
        super().__init__(self.message)

#Implement this class in the new action class you want to implement
class Action:

    def __init__(self, can_interface, my_id, id_ecu):
        self.bus = can.interface.Bus(channel=can_interface, bustype='socketcan')
        self.my_id = my_id
        self.id_ecu = id_ecu
        self.g = GF(self.bus)

    def _collect_response(self, sid: int):
        flag = False
        msg_ext = None
        msg = self.bus.recv(3)
        while msg != None:
            #FirstFrame
            if msg.data[0] == 0x10:
                flag = True
                msg_ext = msg[1:]
            #Consecutive frame
            elif flag and msg.data[0] > 0x20 and msg.data[0] < 0x30:
                msg_ext.data += msg.data[1:]
            #Simple Frame
            else:
                break
            msg = self.bus.recv(3)
        if flag:
            msg = msg_ext
        if msg is not None:
            if self._verify_frame(msg, sid):
                return msg
        return None

    def _verify_frame(self, msg: can.Message, sid: int):
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
        response = self._collect_response(sid)
        if response is None:
            print(error_str)
            response_json = self._to_json("interrupted",1)
            raise CustomError(response_json)
        return response

    def _data_from_frame(self,msg: can.Message):
        handlers = {
                0x62: ReadByIdentifier(),
                0x63: ReadByAddress(),
        }
        if msg.data[0] != 0x10:
            handler = handlers.get(msg.data[1])
        else:
            handler = handlers.get(msg.data[2])
        if handler:
            return handler._data_from_frame(msg)
        return None

    #Implement in the child class
    def _to_json(self, status, no_errors):
        pass
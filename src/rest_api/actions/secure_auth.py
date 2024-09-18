from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class Auth(Action):
    def _auth_to(self):

        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu

        try:
            id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + self.id_ecu[MCU]
            self._authentication(id)
            response_json = {
                "message": "Auth succesfull",
                "can_id": f"0x{id:03X}",
            }
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

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
            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(id, 0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")
            id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + self.id_ecu[MCU]

            # has already internal logger info
            response = self._authentication(id)
            return response

        except CustomError as e:
            self.bus.shutdown()
            return e.message

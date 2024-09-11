from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class Reset(Action):
    def _reset_ecu(self, ecu_id, type_reset):

        id = self.my_id * 0x100 + int(ecu_id, 16)

        try:
            # log_info_message(logger, "Changing session to programming")
            # self.generate.session_control(id, 0x02)
            # self._passive_response(SESSION_CONTROL, "Error changing session control")
            # id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + self.id_ecu[MCU]

            # has already internal logger info
            if type_reset == 'hard':
                self.generate.ecu_reset(id, 0x01, False)
            else:
                self.generate.ecu_reset(id, 0x02, False)

            response = self._passive_response(RESET_ECU, f"Error resetting device {id}")

            # If response received, check the data (assuming index 1 contains 0x7E and index 2 contains 0x00)
            if response.data[1] == 0x51 and response.data[2] == 0x00:
                response_json = {
                    "message": "Ecu reset succesfull",
                    "can_id": f"0x{id:03X}",
                }
            else:
                # If the response is not valid, raise an error
                response_json = self._to_json_error("invalid response", 2)
                raise CustomError(response_json)

            # Log the successful response
            log_info_message(logger, f"Success resetting device {id}")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

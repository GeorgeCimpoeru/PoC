from actions.base_actions import *
from configs.data_identifiers import *
from actions.manual_send_frame import handle_negative_response

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class Reset(Action):
    def reset_ecu(self, ecu_id, type_reset):

        if ecu_id == "10":
            id = self.my_id * 0x100 + self.id_ecu[0]
        if ecu_id == "11":
            id = self.my_id * 0x100 + self.id_ecu[1]

        try:
            # has already internal logger info
            if type_reset == "hard":
                self.generate.ecu_reset(id, 0x01, False)
            else:
                self.generate.ecu_reset(id, 0x02, False)

            self._passive_response(RESET_ECU, f"Error resetting device {id}")

            response_json = {
                "message": "Ecu reset succesfull",
                "can_id": f"0x{id:03X}",
            }

            log_info_message(logger, f"Success resetting device {id}")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

from actions.base_actions import *
from configs.data_identifiers import *
from actions.manual_send_frame import handle_negative_response

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class Reset(Action):
    def reset_ecu(self, ecu_id, type_reset):

        # id = (int(ecu_id, 16) << 16) + self.my_id * 0x100 + self.id_ecu[0]
        id = self.my_id * 0x100 + self.id_ecu[0]

        try:
            # has already internal logger info
            if type_reset == "hard":
                self.generate.ecu_reset(id, 0x01, False)
            else:
                self.generate.ecu_reset(id, 0x02, False)

            self._passive_response(RESET_ECU, f"Error resetting device {id}")
            # response = self._passive_response(RESET_ECU, f"Error resetting device {id}")
            # log_info_message(logger, f"ECU RESET {response}")
            # response_nrc = handle_negative_response(response.data[2], response.data[1])

            # if response.data[1] == 0x51 and response.data[2] == 0x00:
            response_json = {
                "message": "Ecu reset succesfull",
                "can_id": f"0x{id:03X}",
            }
            # else:
                # If the response is not valid, raise an error
                # response_json = self._to_json_error("invalid response", 2)
                # response_json["nrc"] = response_nrc
                # raise CustomError(response_json)

            # Log the successful response
            log_info_message(logger, f"Success resetting device {id}")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

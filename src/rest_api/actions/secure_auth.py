from actions.base_actions import *
from configs.data_identifiers import *
from actions.manual_send_frame import handle_negative_response

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
            response = self._authentication(id)
            response_nrc = handle_negative_response(response.data[3, response.data[2]])

            if response.data[1] == 0x51 and response.data[2] == 0x00:
                response_json = {
                    "message": "Auth succesfull",
                    "can_id": f"0x{id:03X}",
                }
            else:
                # If the response is not valid, raise an error
                response_json = self._to_json_error("invalid response", 2)
                response_json["nrc"] = response_nrc

            return response

        except CustomError as e:
            self.bus.shutdown()
            return e.message

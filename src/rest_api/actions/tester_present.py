from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class Tester(Action):
    """ curl -X GET http://127.0.0.1:5000/api/tester_present """
    def is_present(self):
        """
        Sends a Tester Present request and collects the response to construct a JSON message.

        Args:
        - id: CAN ID of the module.

        Returns:
        - JSON message indicating the status of the tester.
        """
        try:

            id_mcu = self.id_ecu[MCU]
            id = self.my_id * 0x100 + id_mcu
            self.tester_present(id)
            response = self._passive_response(TESTER_PRESENT, "Error for tester present")

            if response.data[1] == 0x7F:
                negative_response = self.handle_negative_response(response.data[3],
                                                                  response.data[2])
                return {
                    "message": "Negative response received while tester present",
                    "negative_response": negative_response
                }

            if response.data[1] == 0x7E and response.data[2] == 0x00:
                response_json = {
                    "message": "Tester is present",
                    "can_id": f"0x{id:03X}",
                }
            else:
                response_json = self._to_json_error("The response was invalid", 2)
                raise CustomError(response_json)

            log_info_message(logger, "Tester is present.")
            return response_json

        except CustomError:
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            return {
                "message": "Error during authentication",
                "negative_response": negative_response
            }

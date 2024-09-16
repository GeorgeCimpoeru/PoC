from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class Tester(Action):
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

            # log_info_message(logger, "Changing session to programming")
            # self.generate.session_control(id, 0x02)
            # self._passive_response(SESSION_CONTROL, "Error changing session control")

            # id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + self.id_ecu[MCU]

            # Generate the tester present request
            self.generate.tester_present(id)

            # Collect the response
            response = self._passive_response(TESTER_PRESENT, "Error for tester present")

            # If response received, check the data (assuming index 1 contains 0x7E and index 2 contains 0x00)
            if response.data[1] == 0x7E and response.data[2] == 0x00:
                # Tester is present, construct the JSON message
                response_json = {
                    "message": "Tester is present",
                    "can_id": f"0x{id:03X}",
                }
            else:
                # If the response is not valid, raise an error
                response_json = self._to_json_error("invalid response", 2)
                raise CustomError(response_json)

            # Log the successful response
            log_info_message(logger, "Tester is present.")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

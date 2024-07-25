from actions.base_actions import *
import time
from configs.data_identifiers import *

class WriteInfo(Action):
    def __init__(self, my_id, id_ecu_list, data):
        super().__init__(my_id, id_ecu_list)
        self.data = data

    def _handleMCU(self, id_MCU):
        id_MCU = self.id_ecu[0]
        id = self.my_id * 0x100 + id_MCU
        log_info_message(logger, "Changing session to default")
        self.generate.session_control(id, 0x01)
        self._passive_response(SESSION_CONTROL, "Error changing session control")
        self._authentication(id)

    def _write_data_to_mcu(self, id_MCU):
        id = self.my_id * 0x100 + id_MCU
        log_info_message(logger, f"Writing data to MCU ID: {id_MCU}")

        data_params = [
            (IDENTIFIER_DOOR, int(self.data.get('Door_param'))),
            (IDENTIFIER_DOOR_SERIALNUMBER, int(self.data.get('Serial_number'))),
            (IDENTIFIER_LIGHTER_VOLTAGE, int(self.data.get('Cigarette_Lighter_Voltage'))),
            (IDENTIFIER_LIGHT_STATE, int(self.data.get('Light_state'))),
            (IDENTIFIER_BELT_STATE, int(self.data.get('BeltCard'))),
            (IDENTIFIER_WINDOWS_CLOSED, int(self.data.get('WindowStatus')))
        ]

        for identifier, data_param in data_params:
            if data_param is not None:
                if isinstance(data_param, list) and len(data_param) > 4:
                    self.generate.write_data_by_identifier_long(id, identifier, self._number_to_list(data_param))
                else:
                    self.generate.write_data_by_identifier(id, identifier, self._number_to_list(data_param))
                self._passive_response(WRITE_BY_IDENTIFIER, f"Error writing {identifier}")

        log_info_message(logger, f"Data written successfully to MCU ID: {id_MCU}")

    def run(self):
        try:
            for id_MCU in self.id_ecu:
                self._handleMCU(id_MCU)
                self._write_data_to_mcu(id_MCU)

            self.bus.shutdown()
            response_json = self._to_json("success", 0)
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def _to_json(self, status, no_errors):
        response_to_frontend = {
            "status": status,
            "No of errors": no_errors,
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return response_to_frontend

    def _number_to_list(self, number):
        """Converts a number to a list of bytes."""
        byte_list = []
        while number > 0:
            byte_list.insert(0, number & 0xFF)
            number = number >> 8
        return byte_list


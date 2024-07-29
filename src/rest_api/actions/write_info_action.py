import datetime
from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class WriteInfo(Action):
    def __init__(self, my_id, id_ecu_list, data):
        super().__init__(my_id, id_ecu_list)
        self.data = data

    def _auth_mcu(self):
        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu

        try:
            log_info_message(logger, "Changing session to default")
            self._authentication(id)
        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def _write_by_identifier(self, id, identifier, value):
        """
        Function to write data to a specific identifier.

        Args:
        - id: The id to write to.
        - identifier: Identifier of the data.
        - value: The value to be written.
        """
        log_info_message(logger, f"Write by identifier {identifier}")
        value_list = self._number_to_list(value)

        if isinstance(value_list, list) and len(value_list) > 4:
            self.generate.write_data_by_identifier_long(id, identifier, value_list)
        else:
            self.generate.write_data_by_identifier(id, identifier, value_list)
            self._passive_response(WRITE_BY_IDENTIFIER, f"Error writing {identifier}")

        return True

    def _write_data(self, id_ECU):
        id = self.my_id * 0x100 + id_ECU
        log_info_message(logger, f"Writing data to ECU ID: {id_ECU}")

        data_params = [
            (IDENTIFIER_DOOR, int(self.data.get('Door_param'))),
            # (IDENTIFIER_DOOR_SERIALNUMBER, int(self.data.get('Serial_number'))),
            # (IDENTIFIER_LIGHTER_VOLTAGE, int(self.data.get('Cigarette_Lighter_Voltage'))),
            # (IDENTIFIER_LIGHT_STATE, int(self.data.get('Light_state'))),
            # (IDENTIFIER_BELT_STATE, int(self.data.get('BeltCard'))),
            # (IDENTIFIER_WINDOWS_CLOSED, int(self.data.get('WindowStatus')))
        ]

        for identifier, data_param in data_params:
            if data_param is not None:
                self._write_by_identifier(id, identifier, data_param)

        log_info_message(logger, f"Data written successfully to ECU ID: {id_ECU}")

    def run(self):
        try:
            self._auth_mcu()
            self._write_data(0x11)
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

    def _number_to_list(self, number: int):
        """Converts a number to a list of bytes."""
        byte_list = []
        while number > 0:
            byte_list.insert(0, number & 0xFF)
            number = number >> 8
        return byte_list

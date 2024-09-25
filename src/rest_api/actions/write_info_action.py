from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class WriteInfo(Action):
    """
    Base class for writing data to various ECUs.

    Attributes:
    - my_id: Identifier for the device initiating the update.
    - id_ecu: Dictionary containing ECU identifiers.
    - generate: Instance of GenerateFrame for generating CAN bus frames.
    - data: Dictionary containing data to be written.
    """

    def __init__(self, my_id, id_ecu_list, data):
        super().__init__(my_id, id_ecu_list)
        self.data = data

    def _auth_mcu(self):
        """
        Authenticate MCU by changing the session to programming and performing authentication.
        """
        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu

        try:
            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(id, 0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")
            self._authentication(id)
        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def write_to_battery(self, data_dict=None):
        """
        Method to write information to the battery module. Handles authentication, data preparation,
        and writing operations.

        Args:
        - item: Optional identifier for a specific data item to write.

        Returns:
        - JSON response.
        """
        auth_result = self._auth_mcu()
        if isinstance(auth_result, str):
            return auth_result

        try:
            id_battery = self.id_ecu[ECU_BATTERY]
            id = self.my_id * 0x100 + id_battery
            log_info_message(logger, f"Writing data to ECU ID: {hex(id_battery)}")

            key_to_identifier_map = {
                "battery_level": IDENTIFIER_BATTERY_ENERGY_LEVEL,
                "voltage": IDENTIFIER_BATTERY_VOLTAGE,
                "percentage": IDENTIFIER_BATTERY_PERCENTAGE,
                "state_of_charge": IDENTIFIER_BATTERY_STATE_OF_CHARGE,
            }

            for key, value in data_dict.items():
                if key in key_to_identifier_map:
                    identifier = key_to_identifier_map[key]
                else:
                    print(f"ERROR: Unknown key '{key}'")
                    continue

                try:
                    value = int(value)
                    if not (0 <= value <= 255):
                        log_error_message(logger, f"Value '{value}' for key '{key}' is out of byte range (0-255).")
                        continue
                except ValueError:
                    log_error_message(logger, f"Invalid value type for key '{key}': {value}")
                    continue

                data_parameter = [value]
                if len(data_parameter) <= 4:
                    self.generate.write_data_by_identifier(id, identifier, data_parameter)
                    self._passive_response(WRITE_BY_IDENTIFIER, f"Error writing {identifier}")

                else:
                    self.generate.write_data_by_identifier_long(id, identifier, data_parameter)

            log_info_message(logger, f"Data written successfully to ECU ID: {id_battery}")
            response_json = self._to_json("success", 0)
            return response_json

        except CustomError:
            self.bus.shutdown()
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            self.bus.shutdown()
            return {
                "status": "error",
                "message": "Error during Read by ID",
                "negative_response": negative_response
            }

    def write_to_doors(self, item=None):
        """
        Method to write information to the doors module. Handles changing session, authentication, data preparation,
            and writing operations.

        Args:
        - item: Optional identifier for a specific data item to write.

        Returns:
        - JSON response.
        """
        log_info_message(logger, "Changing session to programming")
        self.generate.session_control(self.id, 0x02)
        self._passive_response(SESSION_CONTROL, "Error changing session control")

        auth_result = self._auth_mcu()
        if isinstance(auth_result, str):
            return auth_result

        try:
            id_doors = self.id_ecu[ECU_DOORS]
            id = self.my_id * 0x100 + id_doors
            log_info_message(logger, f"Writing data to ECU ID: {id_doors}")

            all_identifiers = {
                IDENTIFIER_DOOR, int(self.data.get('door')),
                IDENTIFIER_DOOR_SERIALNUMBER, int(self.data.get('serial_number')),
                IDENTIFIER_LIGHTER_VOLTAGE, int(self.data.get('lighter_voltage')),
                IDENTIFIER_LIGHT_STATE, int(self.data.get('light_state')),
                IDENTIFIER_BELT_STATE, int(self.data.get('belt')),
                IDENTIFIER_WINDOWS_CLOSED, int(self.data.get('windows_closed'))
            }

            data_to_write = [(item, all_identifiers.get(item))] if item else [(id_, value) for id_, value in all_identifiers.items() if value is not None]

            for identifier, value in data_to_write:
                if value is not None:
                    value_list = self._number_to_byte_list(value)
                    log_info_message(logger, f"Write by identifier {identifier}")

                    if isinstance(value_list, list) and len(value_list) > 4:
                        self.generate.write_data_by_identifier_long(id, identifier, value_list)
                    else:
                        self.generate.write_data_by_identifier(id, identifier, value_list)
                        self._passive_response(WRITE_BY_IDENTIFIER, f"Error writing {identifier}")

            log_info_message(logger, f"Data written successfully to ECU ID: {id_doors}")
            response_json = self._to_json("success", 0)
            return response_json

        except CustomError:
            self.bus.shutdown()
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            self.bus.shutdown()
            return {
                "status": "error",
                "message": "Error during Read by ID",
                "negative_response": negative_response
            }

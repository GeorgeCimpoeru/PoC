from actions.base_actions import *
from configs.data_identifiers import *


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

    def write_to_battery(self, data_dict=None):
        """
        Method to write information to the battery module. Handles authentication, data preparation,
        and writing operations.

        Args:
        - item: Optional identifier for a specific data item to write.

        Returns:
        - JSON response.
        """

        try:
            id = self.my_id * 0x100 + self.id_ecu[ECU_BATTERY]
            log_info_message(logger, f"Writing data to ECU ID: {hex(ECU_BATTERY)}")

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

            log_info_message(logger, f"Data written successfully to ECU ID: {ECU_BATTERY}")
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

    def write_to_doors(self, data_dict=None):
        """
        Method to write information to the doors module. Handles authentication, data preparation,
        and writing operations.
        Args:
        - data_dict: Dictionary containing the data to write.
        Returns:
        - JSON response.
        """

        try:
            id = self.my_id * 0x100 + self.id_ecu[ECU_DOORS]
            log_info_message(logger, f"Writing data to ECU ID: {hex(ECU_DOORS)}")

            key_to_identifier_map = {
                "door": IDENTIFIER_DOOR_STATUS,
                "passenger": IDENTIFIER_PASSENGER_DOOR_STATUS,
                "passenger_lock": IDENTIFIER_PASSENGER_DOOR_LOCKED_STATUS,
                "driver": IDENTIFIER_DRIVER_DOOR_STATUS,
                "ajar": IDENTIFIER_AJAR_STATUS
            }

            for key, value in data_dict.items():
                if key in key_to_identifier_map:
                    identifier = key_to_identifier_map[key]
                else:
                    log_error_message(logger, f"ERROR: Unknown key '{key}'")
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

            log_info_message(logger, f"Data written successfully to ECU ID: {ECU_DOORS}")
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

    def write_to_engine(self, data_dict=None):
        """
        Method to write information to the engine module. Handles authentication, data preparation,
        and writing operations.
        Args:
        - data_dict: Dictionary containing the data to write.
        Returns:
        - JSON response.
        """

        try:
            id = self.my_id * 0x100 + self.id_ecu[ECU_ENGINE]

            log_info_message(logger, f"Writing data to ECU ID: {hex(ECU_ENGINE)}")

            key_to_identifier_map = {
                "engine_rpm": IDENTIFIER_ENGINE_RPM,
                "coolant_temperature": IDENTIFIER_ENGINE_COOLANT_TEMPERATURE,
                "throttle_position": IDENTIFIER_THROTTLE_POSITION,
                "vehicle_speed": IDENTIFIER_VEHICLE_SPEED,
                "engine_load": IDENTIFIER_ENGINE_LOAD,
                "fuel_level": IDENTIFIER_FUEL_LEVEL,
                "oil_temperature": IDENTIFIER_OIL_TEMPERATURE,
                "fuel_pressure": IDENTIFIER_FUEL_PRESSURE,
                "intake_air_temperature": IDENTIFIER_ENGINE_AIR_INTAKE
            }

            for key, value in data_dict.items():
                if key in key_to_identifier_map:
                    identifier = key_to_identifier_map[key]
                else:
                    log_error_message(logger, f"ERROR: Unknown key '{key}'")
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

            log_info_message(logger, f"Data written successfully to ECU ID: {ECU_ENGINE}")
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

    def write_to_hvac(self, data_dict=None):
        """
        Method to write information to the HVAC module. Handles changing session, authentication, data preparation,
        and writing operations.
        Args:
        - data_dict: Dictionary containing HVAC parameters to write.
        Returns:
        - JSON response.
        """

        try:
            id = self.my_id * 0x100 + self.id_ecu[ECU_HVAC]
            log_info_message(logger, f"Writing data to ECU ID: {ECU_HVAC}")

            key_to_identifier_map = {
                "mass_air_flow": IDENTIFIER_MASS_AIR_FLOW_MAF_SENSOR,
                "ambient_air_temperature": IDENTIFIER_AMBIENT_AIR_TEMPERATURE,
                "cabin_temperature": IDENTIFIER_CABIN_TEMPERATURE,
                "cabin_temperature_driver_set": IDENTIFIER_CABIN_TEMPERATURE_DRIVER_SET,
                "fan_speed": IDENTIFIER_FAN_SPEED,
                "hvac_modes": IDENTIFIER_HVAC_MODES,
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

            log_info_message(logger, f"Data written successfully to ECU ID: {ECU_HVAC}")
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

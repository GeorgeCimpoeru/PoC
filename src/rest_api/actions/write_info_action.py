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

    def write_to_battery(self, item=None):
        """
        Method to write information to the battery module. Handles session changes, authentication, data preparation,
        and writing operations.

        Args:
        - item: Optional identifier for a specific data item or list of items to write. If None, writes all items.

        curl -X POST "http://localhost:5000/api/write_info_battery" \
        -H "Content-Type: application/json" \
        -d '{
            "battery_level": 80,
            "voltage": 12.5,
            "percentage": 50,
            "battery_state_of_charge": 75
            }'

        Returns:
        - JSON response.
        """

        # Step 1: Authenticate the MCU
        auth_result = self._auth_mcu()
        if isinstance(auth_result, str):
            return auth_result

        try:
            id_battery = self.id_ecu[ECU_BATTERY]
            id = self.my_id * 0x100 + id_battery
            log_info_message(logger, f"Writing data to ECU ID: {id_battery}")

            # Step 2: Retrieve valid battery identifiers from data_identifiers
            valid_battery_identifiers = data_identifiers["Battery_Identifiers"]

            # Step 3: Prepare all possible data and ensure they are integers
            all_identifiers = {
                valid_battery_identifiers["energy_level"]: int(self.data.get('battery_level', 0)),
                valid_battery_identifiers["voltage"]: int(self.data.get('voltage', 0) * 10),  # Convert to integer
                valid_battery_identifiers["percentage"]: int(self.data.get('percentage', 0)),
                valid_battery_identifiers["state_of_charge"]: int(self.data.get('battery_state_of_charge', 0)),
            }

            # Step 4: Filter the data to write based on the item argument
            data_to_write = []

            if item:
                if isinstance(item, str):
                    item = [item]  # Convert to list for uniform handling

                for single_item in item:
                    if single_item in valid_battery_identifiers:
                        identifier = valid_battery_identifiers[single_item]
                        value = all_identifiers.get(identifier)
                        if value is not None:
                            data_to_write.append((identifier, value))
                        else:
                            return {"error": f"Value for '{single_item}' is missing or invalid."}
                    else:
                        return {"error": f"Invalid parameter '{single_item}'. Use /get_identifiers to see valid parameters."}
            else:
                data_to_write = [(identifier, value) for identifier, value in all_identifiers.items() if value is not None]

            # Step 5: Perform the write operation for each identifier
            for identifier, value in data_to_write:
                if value is not None:
                    value_list = self._number_to_byte_list(value)
                    log_info_message(logger, f"Writing {value} to identifier {identifier}")

                    if isinstance(value_list, list) and len(value_list) > 4:
                        self.generate.write_data_by_identifier_long(id, int(identifier, 16), value_list)
                    else:
                        self.generate.write_data_by_identifier(id, int(identifier, 16), value_list)
                    self._passive_response(WRITE_BY_IDENTIFIER, f"Error writing {identifier}")

            # Step 6: Success message
            log_info_message(logger, f"Data written successfully to ECU ID: {id_battery}")
            response_json = self._to_json("success", 0)
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

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
        if isinstance(auth_result, str):  # If authentication fails and returns a message
            return auth_result

        try:
            id_doors = self.id_ecu[ECU_DOORS]
            id = self.my_id * 0x100 + id_doors
            log_info_message(logger, f"Writing data to ECU ID: {id_doors}")

            # Data preparation
            all_identifiers = {
                IDENTIFIER_DOOR, int(self.data.get('door')),
                IDENTIFIER_DOOR_SERIALNUMBER, int(self.data.get('serial_number')),
                IDENTIFIER_LIGHTER_VOLTAGE, int(self.data.get('lighter_voltage')),
                IDENTIFIER_LIGHT_STATE, int(self.data.get('light_state')),
                IDENTIFIER_BELT_STATE, int(self.data.get('belt')),
                IDENTIFIER_WINDOWS_CLOSED, int(self.data.get('windows_closed'))
            }

            # Determine which data to write
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

        except CustomError as e:
            self.bus.shutdown()
            return e.message

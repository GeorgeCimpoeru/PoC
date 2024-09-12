import datetime
from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class ToJSON:
    """Open-Close principle. Base class for different JSON formats."""
    def _to_json(self, data):
        pass


class BatteryToJSON():
    def _to_json(self, data: list):
        response = {
            "battery_level": data[0],
            "voltage": data[1],
            "percentage": data[2],
            "battery_state_of_charge": data[3],
            "life_cycle": data[4],
            "fully_charged": data[5],
            "serial_number": data[6],
            "range_battery": data[7],
            "charging_time": data[8],
            "device_consumption": data[9],
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return response


class EngineToJSON():
    def _to_json(self, data: list):
        response = {
            "power_output": data[0],
            "weight": data[1],
            "fuel_consumption": data[1],
            "torque": data[2],
            "fuel_used": data[3],
            "state_of_running": data[4],
            "current_speed": data[5],
            "engine_state": data[6],
            "serial_number": data[7]
        }
        return response


class DoorsToJSON():
    def _to_json(self, data: list):
        response_to_frontend = {
            "door": data[0],
            "serial_number": data[1],
            "lighter_voltage": data[2],
            "light_state": data[3],
            "belt": data[4],
            "windows_closed": data[5],
        }
        return response_to_frontend


class ReadInfo(Action):
    """
    ReadInfo class to read information from different ECUs.

    Attributes:
    - my_id: Identifier for the device initiating the update.
    - id_ecu: Identifier for the specific ECU being updated.
    - g: Instance of GenerateFrame for generating CAN bus frames.
    """
    def _auth_mcu(self):

        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu

        try:
            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(id, 0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")
            id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + self.id_ecu[MCU]
            self._authentication(id)

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    @staticmethod
    def _get_battery_state_of_charge(state_of_charge):
        # Remove the '0x' prefix if present
        if state_of_charge.startswith("0x"):
            state_of_charge = state_of_charge[2:]

        # Dictionary mapping hex string values to battery states
        state_mapping = {
            "00": "Unknown state",
            "01": "Charging",
            "02": "Discharging",
            "03": "Empty",
            "04": "Fully charged",
            "05": "Pending charge",
            "06": "Pending discharge"
        }

        # Return the corresponding state or "Unknown state" if not found
        return state_mapping.get(state_of_charge, "Unknown state")

    def read_from_battery(self, item=None):
        """
        Method to read information from the battery module.

        Args:
        - item: Optional identifier for a specific data item to read.

        Returns:
        - JSON response.
        """
        auth_result = self._auth_mcu()
        if isinstance(auth_result, str):
            return auth_result

        try:
            log_info_message(logger, "Reading data from battery")
            id_battery = self.id_ecu[ECU_BATTERY]
            id = self.my_id * 0x100 + id_battery

            identifiers = data_identifiers["Battery_Identifiers"]
            results = {}

            def hex_to_dec(value):
                """Helper function to convert hex to decimal if not 'No data'."""
                try:
                    return int(value, 16)
                except (ValueError, TypeError):
                    return value

            if item:
                if item in identifiers:
                    identifier = identifiers[item]
                    result_value = self._read_by_identifier(id, int(identifier, 16))

                    if item == "state_of_charge" and result_value:
                        result_value = self._get_battery_state_of_charge(result_value)

                    results[item] = result_value if result_value else "No data"
                else:
                    return {"error": f"Invalid parameter '{item}'. Use /get_identifiers to see valid parameters."}
            else:
                for key, identifier in identifiers.items():
                    result_value = self._read_by_identifier(id, int(identifier, 16))

                    if key == "state_of_charge" and result_value:
                        result_value = self._get_battery_state_of_charge(result_value)

                    results[key] = result_value if result_value else "No data"

            # Convert hex results to decimal
            response_json = {
                "battery_level": hex_to_dec(results.get("battery_level", "No data")),
                "voltage": hex_to_dec(results.get("voltage", "No data")),
                "percentage": hex_to_dec(results.get("percentage", "No data")),
                "battery_state_of_charge": results.get("state_of_charge", "No data"),
                "life_cycle": hex_to_dec(results.get("life_cycle", "No data")),
                "fully_charged": hex_to_dec(results.get("fully_charged", "No data")),
                "serial_number": hex_to_dec(results.get("serial_number", "No data")),
                "range_battery": hex_to_dec(results.get("range", "No data")),
                "charging_time": hex_to_dec(results.get("charging_time", "No data")),
                "device_consumption": hex_to_dec(results.get("device_consumption", "No data")),
                "time_stamp": datetime.datetime.now().isoformat()
            }

            self.bus.shutdown()
            log_info_message(logger, "Sending JSON response")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def read_from_engine(self):

        """
        Method to read information from the engine module.

        Returns:
        - data response.
        """
        self._auth_mcu()
        id_engine = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_engine

        try:
            id_battery = self.id_ecu[ECU_ENGINE]
            id = self.my_id * 0x100 + id_battery

            log_info_message(logger, "Reading data from engine")
            power_output = self._read_by_identifier(id, IDENTIFIER_ENGINE_POWER_OUTPUT)
            weight = self._read_by_identifier(id, IDENTIFIER_ENGINE_WEIGHT)
            fuel_consumption = self._read_by_identifier(id, IDENTIFIER_ENGINE_FUEL_CONSUMPTION)
            torque = self._read_by_identifier(id, IDENTIFIER_ENGINE_TORQUE)
            fuel_used = self._read_by_identifier(id, IDENTIFIER_ENGINE_FUEL_USED)
            state_of_running = self._read_by_identifier(id, IDENTIFIER_ENGINE_STATE_OF_RUNNING)
            current_speed = self._read_by_identifier(id, IDENTIFIER_ENGINE_CURRENT_SPEED)
            engine_state = self._read_by_identifier(id, IDENTIFIER_ENGINE_STATE)
            serial_number = self._read_by_identifier(id, IDENTIFIER_ENGINE_SERIAL_NUMBER)

            data = [power_output, weight, fuel_consumption, torque, fuel_used, state_of_running, current_speed, engine_state, serial_number]
            module = EngineToJSON()
            response = module._to_json(data)

            self.bus.shutdown()

            log_info_message(logger, "Sending JSON")
            return response

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def read_from_doors(self):
        id_door = self.id_ecu[1]
        id = self.my_id * 0x100 + id_door

        try:
            self._authentication(id)

            log_info_message(logger, "Reading data from doors")

            door = self._read_by_identifier(id, IDENTIFIER_DOOR)
            serial_number = self._read_by_identifier(id, IDENTIFIER_DOOR_SERIALNUMBER)
            cigarette_lighter_voltage = self._read_by_identifier(id, IDENTIFIER_LIGHTER_VOLTAGE)
            light_state = self._read_by_identifier(id, IDENTIFIER_LIGHT_STATE)
            belt = self._read_by_identifier(id, IDENTIFIER_BELT_STATE)
            windows_closed = self._read_by_identifier(id, IDENTIFIER_WINDOWS_CLOSED)
            data = [door, serial_number, cigarette_lighter_voltage, light_state, belt, windows_closed]

            module = DoorsToJSON()
            response = module._to_json(data)
            # Shutdown the CAN bus interface
            self.bus.shutdown()

            log_info_message(logger, "Sending JSON")
            return response
        except CustomError as e:
            self.bus.shutdown()
            return e.message

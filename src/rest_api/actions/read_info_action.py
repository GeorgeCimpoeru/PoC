"""
Author: Mujdei Ruben
Date: June 2024
Use the class ReadInfo to read information from different ECUs.
Methods:
    -read_from_battery(): Read info from battery module
    -read_from_custom(identifiers[]): Read specific data

How to use:
    u = ReadInfo(0x23, [0x11, 0x12, 0x13])
    u.read_from_battery()
    #or
    u.read_from_custom([0x1234, 0x6543])
"""

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
            log_info_message(logger, "Changing session to default")
            # self.generate.session_control(id, 0x01)
            # self._passive_response(SESSION_CONTROL, "Error changing session control")
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

    def read_from_battery(self):
        """
        Method to read information from the battery module.

        Returns:
        - JSON response.
        """
        self._auth_mcu()
        id_battery = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_battery

        try:
            log_info_message(logger, "Reading data from battery")
            id_battery = self.id_ecu[ECU_BATTERY]
            id = self.my_id * 0x100 + id_battery

            identifiers = {
                "level": IDENTIFIER_BATTERY_ENERGY_LEVEL,
                "voltage": IDENTIFIER_BATTERY_VOLTAGE,
                "percentage": IDENTIFIER_BATTERY_PERCENTAGE,
                "state_of_charge": IDENTIFIER_BATTERY_STATE_OF_CHARGE,
                "life_cycle": IDENTIFIER_BATTERY_LIFE_CYCLE,
                "fully_charged": IDENTIFIER_BATTERY_FULLY_CHARGED,
                "serial_number": IDENTIFIER_ECU_SERIAL_NUMBER,
                "range_battery": IDENTIFIER_BATTERY_RANGE,
                "charging_time": IDENTIFIER_BATTERY_CHARGING_TIME,
                "device_consumption": IDENTIFIER_DEVICE_CONSUMPTION
        }

            data = {key: self._read_by_identifier(id, identifier) for key, identifier in identifiers.items()}

            # Handle missing values
            data = {key: (value if value is not None else "No read") for key, value in data.items()}

            data["state_of_charge"] = self._get_battery_state_of_charge(data["state_of_charge"])
            data["percentage"] = round(((int(data["percentage"], 16) / 255) * 100), 2)

            json_data = [
                str(int(data["level"], 16)),
                int(data["voltage"], 16),
                data["percentage"],
                data["state_of_charge"],
                data["life_cycle"],
                data["fully_charged"],
                data["serial_number"],
                data["range_battery"],
                data["charging_time"],
                data["device_consumption"]
            ]

            response_json = BatteryToJSON()._to_json(json_data)

            # Shutdown the CAN bus interface
            self.bus.shutdown()
            log_info_message(logger, "Sending JSON")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def read_from_engine(self):
        """
        Method to read information from the engine module.

        Returns:
        - Data response in JSON format or error message.
        """
        self._auth_mcu()
        id_engine = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_engine

        try:
            log_info_message(logger, "Reading data from engine")

            id_engine = self.id_ecu[ECU_ENGINE]
            id = self.my_id * 0x100 + id_engine

            identifiers = {
                "power_output": IDENTIFIER_ENGINE_POWER_OUTPUT,
                "weight": IDENTIFIER_ENGINE_WEIGHT,
                "fuel_consumption": IDENTIFIER_ENGINE_FUEL_CONSUMPTION,
                "torque": IDENTIFIER_ENGINE_TORQUE,
                "fuel_used": IDENTIFIER_ENGINE_FUEL_USED,
                "state_of_running": IDENTIFIER_ENGINE_STATE_OF_RUNNING,
                "current_speed": IDENTIFIER_ENGINE_CURRENT_SPEED,
                "engine_state": IDENTIFIER_ENGINE_STATE,
                "serial_number": IDENTIFIER_ENGINE_SERIAL_NUMBER
            }

            # Read data from the identifiers
            data = {key: self._read_by_identifier(id, identifier) for key, identifier in identifiers.items()}

            # Handle missing values
            data = {key: (value if value is not None else "No read") for key, value in data.items()}

            json_data = [
            data["power_output"],
            data["weight"],
            data["fuel_consumption"],
            data["torque"],
            data["fuel_used"],
            data["state_of_running"],
            data["current_speed"],
            data["engine_state"],
            data["serial_number"]
            ]

            response_json = EngineToJSON()._to_json(json_data)

            self.bus.shutdown()
            log_info_message(logger, "Sending JSON")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message


    def read_from_doors(self):
        id_door = self.id_ecu[1]
        id = self.my_id * 0x100 + id_door

        try:
            log_info_message(logger, "Changing session to default")
            self.generate.session_control(id, 0x01)
            self._passive_response(SESSION_CONTROL, "Error changing session control")
            log_info_message(logger, "Changing session to default")
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
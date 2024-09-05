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

    def read_from_battery(self):
        """
        Method to read information from the battery module.

        Returns:
        - JSON response.
        """
        self._auth_mcu()
        try:
            log_info_message(logger, "Reading data from battery")
            id = self.my_id * 0x100 + self.id_ecu[MCU]

            level = None
            voltage = None
            percentage = None
            state_of_charge = None
            life_cycle = None
            fully_charged = None
            serial_number = None
            range_battery = None
            charging_time = None
            device_consumption = None

            level = self._read_by_identifier(id, IDENTIFIER_BATTERY_ENERGY_LEVEL)
            voltage = self._read_by_identifier(id, IDENTIFIER_BATTERY_VOLTAGE)
            percentage = self._read_by_identifier(id, IDENTIFIER_BATTERY_PERCENTAGE)
            state_of_charge = self._read_by_identifier(id, IDENTIFIER_BATTERY_STATE_OF_CHARGE)
            # life_cycle = self._read_by_identifier(id, IDENTIFIER_BATTERY_LIFE_CYCLE)
            # fully_charged = self._read_by_identifier(id, IDENTIFIER_BATTERY_FULLY_CHARGED)
            # serial_number = self._read_by_identifier(id, IDENTIFIER_ECU_SERIAL_NUMBER)
            # range_battery = self._read_by_identifier(id, IDENTIFIER_BATTERY_RANGE)
            # charging_time = self._read_by_identifier(id, IDENTIFIER_BATTERY_CHARGING_TIME)
            # device_consumption = self._read_by_identifier(id, IDENTIFIER_DEVICE_CONSUMPTION)

            life_cycle = life_cycle if life_cycle is not None else "No read"
            fully_charged = fully_charged if fully_charged is not None else "No read"
            serial_number = serial_number if serial_number is not None else "No read"
            range_battery = range_battery if range_battery is not None else "No read"
            charging_time = charging_time if charging_time is not None else "No read"
            device_consumption = device_consumption if device_consumption is not None else "No read"

            battery_state_of_charge = self._get_battery_state_of_charge(state_of_charge)

            data = [str(int(level, 16)),
                    int(voltage, 16),
                    round(((int(percentage, 16) / 255) * 100), 2),
                    battery_state_of_charge,
                    life_cycle,
                    fully_charged,
                    serial_number,
                    range_battery,
                    charging_time,
                    device_consumption]

            response_json = BatteryToJSON()._to_json(data)

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

            # Shutdown the CAN bus interface
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

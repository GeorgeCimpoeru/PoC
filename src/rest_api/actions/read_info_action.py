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


class ToJSON:
    """Open-Close principle. Base class for different JSON formats."""
    def _to_json(self, data):
        pass


# class BatteryToJSON():
#     def _to_json(self, data: list):
#         response_to_frontend = {
#             "battery_level": data[0],
#             "voltage": data[1],
#             "battery_state_of_charge": data[2],
#             "temperature": data[3],
#             "life_cycle": data[4],
#             "fully_charged": data[5],
#             "serial_number": data[6],
#             "range_battery": data[7],
#             "charging_time": data[8],
#             "device_consumption": data[9],
#             "time_stamp": datetime.datetime.now().isoformat()
#         }
#         return (response_to_frontend)

class BatteryToJSON():
    def _to_json(self, data: list):
        response_to_frontend = {
            "battery_level": data[0],
            "voltage": data[1],
            "battery_state_of_charge": data[2],
            "percentage": data[3],
            "life_cycle": data[4],
            "fully_charged": data[5],
            "serial_number": data[6],
            "range_battery": data[7],
            "charging_time": data[8],
            "device_consumption": data[9],
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return (response_to_frontend)

# class ElementToJSON(ToJSON):
#     def _to_json(self, data: list):
#         response_to_frontend = {}
#         for index, element in enumerate(data, start=1):
#             response_to_frontend[f"Element{index}"] = element
#         return json.dumps(response_to_frontend)


class EngineToJSON():
    def _to_json(self, data: list):
        response_to_frontend = {
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
        return (response_to_frontend)


class DoorsToJSON():
    def _to_json(self, data: list):
        response_to_frontend = {
            "Door_param": data[0],
            "Serial_number": data[1],
            "Cigarette_Lighter_Voltage": data[2],
            "Light_state": data[3],
            "BeltCard": data[4],
            "WindowStatus": data[5],
        }
        return (response_to_frontend)


class ReadInfo(Action):
    """
    ReadInfo class to read information from different ECUs.

    Attributes:
    - my_id: Identifier for the device initiating the update.
    - id_ecu: Identifier for the specific ECU being updated.
    - g: Instance of GenerateFrame for generating CAN bus frames.
    """

    def read_from_battery(self):
        """
        Method to read information from the battery module.

        Returns:
        - JSON response.
        """

        id_battery = self.id_ecu[0]
        id = self.my_id * 0x100 + id_battery

        try:
            log_info_message(logger, "Changing session to default")
            # self.generate.session_control(id, 0x01)
            # self._passive_response(SESSION_CONTROL, "Error changing session control")

            # self._authentication(id)

            log_info_message(logger, "Reading data from battery")
            level = self._read_by_identifier(id, IDENTIFIER_BATTERY_ENERGY_LEVEL)
            voltage = self._read_by_identifier(id, IDENTIFIER_BATTERY_VOLTAGE)
            state_of_charge = self._read_by_identifier(id, IDENTIFIER_BATTERY_STATE_OF_CHARGE)
            percentage = self._read_by_identifier(id, IDENTIFIER_BATTERY_PERCENTAGE)
            # temperature = self._read_by_identifier(id, IDENTIFIER_BATTERY_TEMPERATURE) # ToDO
            # life_cycle = self._read_by_identifier(id, IDENTIFIER_BATTERY_LIFE_CYCLE) # ToDo
            # fully_charged = self._read_by_identifier(id, IDENTIFIER_BATTERY_FULLY_CHARGED) # ToDo
            # serial_number = self._read_by_identifier(id, IDENTIFIER_ECU_SERIAL_NUMBER) # ToDo
            # range_battery = self._read_by_identifier(id, IDENTIFIER_BATTERY_RANGE) # ToDo
            # charging_time = self._read_by_identifier(id, IDENTIFIER_BATTERY_CHARGING_TIME) # ToDo
            # device_consumption = self._read_by_identifier(id, IDENTIFIER_DEVICE_CONSUMPTION) # ToDo
            # data = [level, voltage, state_of_charge, temperature, life_cycle, fully_charged, serial_number, range_battery, charging_time, device_consumption]

            life_cycle = "NA"
            fully_charged = "NA"
            serial_number = "NA"
            range_battery = "NA"
            charging_time = "NA"
            device_consumption = "NA"

            percentage_value = ((int(percentage, 16) / 255) * 100)
            percentage_string = f"{percentage_value:.2f}%"

            voltage_value = int(voltage, 16)
            voltage_string = f"{voltage_value:.2f}V"

            battery_state_of_charge = "Charging" if state_of_charge == "01" else "Not charging"

            data = [str(int(level, 16)), voltage_string, battery_state_of_charge, percentage_string, life_cycle, fully_charged, serial_number, range_battery, charging_time, device_consumption]
            module = BatteryToJSON()

            response_json = module._to_json(data)
            # Shutdown the CAN bus interface
            self.bus.shutdown()

            log_info_message(logger, "Sending JSON")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    # def read_from_custom(self, identifiers:list):
    #     """
    #     Method to read information from specific identifier.

    #     Returns:
    #     - JSON response.
    #     """
    #     id_battery = self.id_ecu[1]
    #     id = self.my_id * 0x100 + id_battery
    #     try:
    #         log_info_message(logger, "Changing session to default")
    #         self.generate.session_control(id, 0x01)
    #         self._passive_response(SESSION_CONTROL, "Error changing session control")

    #         self._authentication(id)

    #         #Read each data from identifier
    #         log_info_message(logger, "Reading data..")
    #         data_collected = []
    #         for identifier in identifiers:
    #             data_collected.append(self._read_by_identifier(id,identifier))

    #         module = ElementToJSON()
    #         response_json = self._to_json(module, data_collected)
    #         # Shutdown the CAN bus interface
    #         self.bus.shutdown()

    #         log_info_message(logger, "Sending JSON")
    #         return response_json

    #     except CustomError as e:
    #         self.bus.shutdown()
    #         return e.message

    def read_from_engine(self):

        """
        Method to read information from the engine module.

        Returns:
        - data response.
        """

        id_engine = self.id_ecu[1]
        id = self.my_id * 0x100 + id_engine

        try:
            log_info_message(logger, "Changing session to default")
            self.generate.session_control(id, 0x01)
            self._passive_response(SESSION_CONTROL, "Error changing session control")
            self._authentication(id)
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
        log_info_message(logger, "Changing session to default")
        self.generate.session_control(id, 0x01)
        self._passive_response(SESSION_CONTROL, "Error changing session control")
        log_info_message(logger, "Changing session to default")
        self.generate.session_control(id, 0x01)
        self._passive_response(SESSION_CONTROL, "Error changing session control")
        self._authentication(id)

        log_info_message(logger, "Reading data from doors")

        Door = self._read_by_identifier(id, IDENTIFIER_DOOR)
        serial_number = self._read_by_identifier(id, IDENTIFIER_DOOR_SERIALNUMBER)
        cigarette_lighter_voltage = self._read_by_identifier(id, IDENTIFIER_LIGHTER_VOLTAGE)
        light_state = self._read_by_identifier(id, IDENTIFIER_LIGHT_STATE)
        belt = self._read_by_identifier(id, IDENTIFIER_BELT_STATE)
        windows_closed = self._read_by_identifier(id, IDENTIFIER_WINDOWS_CLOSED)
        data = [Door, serial_number, cigarette_lighter_voltage, light_state, belt, windows_closed]

        module = DoorsToJSON()
        response = module._to_json(data)
        # Shutdown the CAN bus interface
        self.bus.shutdown()

        log_info_message(logger, "Sending JSON")
        return response
    except CustomError as e:
        self.bus.shutdown()
        return e.message

    # def _to_json(self, module: ToJSON, data: list):
    #     """
    #     Private method to create a JSON response with status and error information.

    #     Args:
    #     - module: An instance of ToJSON or its subclasses.
    #     - data: Data collected from the ECU.
    #     """
    #     return module._to_json(data)

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

import json
import datetime
from actions.base_actions import *

class ToJSON:
    """Open-Close principle. Base class for different JSON formats."""
    def _to_json(self, data):
        pass

class BatteryToJSON(ToJSON):
    def _to_json(self, data: list):
        response_to_frontend = {
            "battery_level": data[0],
            "voltage": data[1],
            "battery_state_of_charge": data[2],
            "temperature": data[3],
            "life_cycle": data[4],
            "fully_charged": data[5],
            "serial_number": data[6],
            "range_battery": data[7],       # new item from front 
            "charging_time": data[8],       # new item from front
            "device_consumption": data[9],   # new item from front
            "time_stamp": datetime.datetime.now().isoformat() # Item not existing at front
        }
        return (response_to_frontend)

class ElementToJSON(ToJSON):
    def _to_json(self, data: list):
        response_to_frontend = {}
        for index, element in enumerate(data, start=1):
            response_to_frontend[f"Element{index}"] = element
        return json.dumps(response_to_frontend)

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

        id_battery = self.id_ecu[1]
        id = self.my_id * 0x100 + id_battery

        try:
            log_info_message(logger, "Changing session to default")
            self.generate.session_control(id, 0x01)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            self._authentication(id)

            log_info_message(logger, "Reading data from battery")
            level = self._read_by_identifier(id,IDENTIFIER_BATTERY_ENERGY_LEVEL)
            voltage = self._read_by_identifier(id,IDENTIFIER_BATTERY_VOLTAGE)
            state_of_charge = self._read_by_identifier(id,IDENTIFIER_BATTERY_STATE_OF_CHARGE)
            temperature = self._read_by_identifier(id,IDENTIFIER_BATTERY_TEMPERATURE)
            life_cycle = self._read_by_identifier(id,IDENTIFIER_BATTERY_LIFE_CYCLE)
            fully_charged = self._read_by_identifier(id,IDENTIFIER_BATTERY_FULLY_CHARGED)
            serial_number = self._read_by_identifier(id,IDENTIFIER_ECU_SERIAL_NUMBER)
            range_battery = self._read_by_identifier(id,IDENTIFIER_BATTERY_RANGE)
            charging_time = self._read_by_identifier(id,IDENTIFIER_BATTERY_CHARGING_TIME)
            device_consumption = self._read_by_identifier(id,IDENTIFIER_DEVICE_CONSUMPTION)
            data = [level, voltage, state_of_charge, temperature, life_cycle,fully_charged, serial_number,range_battery,charging_time,device_consumption]
            module = BatteryToJSON()

            response_json = self._to_json(module, data)
            # Shutdown the CAN bus interface
            self.bus.shutdown()

            log_info_message(logger, "Sending JSON")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message
    
    def read_from_custom(self, identifiers:list):
        """
        Method to read information from specific identifier.

        Returns:
        - JSON response.
        """
        id_battery = self.id_ecu[1]
        id = self.my_id * 0x100 + id_battery
        try:
            log_info_message(logger, "Changing session to default")
            self.generate.session_control(id, 0x01)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            self._authentication(id)

            #Read each data from identifier
            log_info_message(logger, "Reading data..")         
            data_collected = []
            for identifier in identifiers:
                data_collected.append(self._read_by_identifier(id,identifier))

            module = ElementToJSON()
            response_json = self._to_json(module, data_collected)
            # Shutdown the CAN bus interface
            self.bus.shutdown()

            log_info_message(logger, "Sending JSON")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message
        
    def _to_json(self, module: ToJSON, data: list):
        """
        Private method to create a JSON response with status and error information.

        Args:
        - module: An instance of ToJSON or its subclasses.
        - data: Data collected from the ECU.
        """
        return module._to_json(data)
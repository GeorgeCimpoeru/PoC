import datetime
from actions.base_actions import *
from configs.data_identifiers import *

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
    def _auth_mcu(self, ecu_type):

        id_mcu = self.id_ecu[ecu_type]
        id = self.my_id * 0x100 + id_mcu

        try:
            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(id, 0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")
            id = (self.id_ecu[ecu_type] << 16) + (self.my_id << 8) + self.id_ecu[MCU]
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
    
    def _interpret_status(self, status, context):
        """Interprets various status values based on a predefined mapping."""
        door_mapping = {
        "00": "Closed",  
        "01": "Open"
        }
        
        lock_mapping = {
            "00": "Locked",    
            "01": "Unlocked"
        }
        
        ajar_mapping = {
            "00": "No warning", 
            "01": "Warning"
        }
            
        
        if context == "door":
            return door_mapping.get(status, "Unknown status")
        elif context == "lock":
            return lock_mapping.get(status, "Unknown status")
        elif context == "ajar":
            return ajar_mapping.get(status, "Unknown status")
        else:
            return "Invalid context"


    def read_from_battery(self, item=None):
        """
        Method to read information from the battery module.

        Args:
        - item: Optional identifier for a specific data item to read.

        Returns:
        - JSON response.
        """
        # auth_result = self._auth_mcu(ECU_BATTERY)
        # if isinstance(auth_result, str):
        #     return auth_result

        try:
            log_info_message(logger, "Reading data from battery")
            id = self.my_id * 0x100 + self.id_ecu[ECU_BATTERY]

            identifiers = data_identifiers["Battery_Identifiers"]
            results = {}

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
        
            response_json = {
                "battery_level": self.hex_to_dec(results.get("battery_level")),
                "voltage": self.hex_to_dec(results.get("voltage")),
                "percentage": self.hex_to_dec(results.get("percentage")),
                "battery_state_of_charge": results.get("state_of_charge"),
                # "life_cycle": self.hex_to_dec(results.get("life_cycle", "No data")),
                # "fully_charged": self.hex_to_dec(results.get("fully_charged", "No data")),
                # "serial_number": self.hex_to_dec(results.get("serial_number", "No data")),
                # "range_battery": self.hex_to_dec(results.get("range", "No data")),
                # "charging_time": self.hex_to_dec(results.get("charging_time", "No data")),
                # "device_consumption": self.hex_to_dec(results.get("device_consumption", "No data")),
                "time_stamp": datetime.datetime.now().isoformat()
            }

            self.bus.shutdown()
            log_info_message(logger, "Sending JSON response")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def read_from_doors(self, item=None):
        """
        Method to read information from the door module.

        Args:
        - item: Optional identifier for a specific door status to read.

        Returns:
        - JSON response.
        """
        try:
            # auth_result = self._auth_mcu(ECU_DOORS)
            # if isinstance(auth_result, str):
            #     return auth_result

            log_info_message(logger, "Reading data from doors")
            id = self.my_id * 0x100 + self.id_ecu[ECU_DOORS]

            identifiers = data_identifiers["Doors_Identifiers"]
            results = {}

            if item:
                if item in identifiers:
                    identifier = identifiers[item]
                    result_value = self._read_by_identifier(id, int(identifier, 16))

                    results[item] = self._interpret_status(result_value) if result_value else "No data"

                else:
                    return {"error": f"Invalid parameter '{item}'. Use /get_identifiers to see valid parameters."}
            else:
                for key, identifier in identifiers.items():
                    result_value = self._read_by_identifier(id, int(identifier, 16))

                    if key in ["door", "passenger", "driver"]:
                        context = "door"
                    elif key == "passenger_lock":
                        context = "lock"
                    elif key == "ajar":
                        context = "ajar"
                    else:
                        context = "unknown"


                    results[key] = self._interpret_status(result_value, context) if result_value else "No data"


            response_json = {
                "door": self.hex_to_dec(results.get("door", "No data")),
                "passenger": self.hex_to_dec(results.get("passenger", "No data")),
                "passenger_lock": self.hex_to_dec (results.get("passenger_lock", "No data")),
                "driver": self.hex_to_dec (results.get("driver", "No data")),
                "ajar": self.hex_to_dec (results.get("ajar", "No data")),
                "time_stamp": datetime.datetime.now().isoformat()
            }

            self.bus.shutdown()
            log_info_message(logger, "Sending JSON response")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message
        

    def read_from_engine(self, item=None):
        """
        Method to read information from the engine module.

        Args:
        - item: Optional identifier for a specific data item to read.

        Returns:
        - JSON response with engine data.
        """
        # auth_result = self._auth_mcu(ECU_ENGINE)
        # if isinstance(auth_result, str):
        #     return auth_result

        try:
            log_info_message(logger, "Reading data from engine")
            id = self.my_id * 0x100 + self.id_ecu[ECU_ENGINE]

            identifiers = data_identifiers["Engine_Identifiers"]
            results = {}

            if item:
                if item in identifiers:
                    identifier = identifiers[item]
                    result_value = self._read_by_identifier(id, int(identifier, 16))

                    results[item] = result_value if result_value else "No data"
                else:
                    return {"error": f"Invalid parameter '{item}'. Use /get_identifiers to see valid parameters."}
            else:
                for key, identifier in identifiers.items():
                    result_value = self._read_by_identifier(id, int(identifier, 16))
                    results[key] = result_value if result_value else "No data"

            response_json = {
                # "engine_rpm": self.hex_to_dec(results.get("engine_rpm", "No data")),
                "coolant_temperature": self.hex_to_dec(results.get("coolant_temperature", "No data")),
                "throttle_position": self.hex_to_dec(results.get("throttle_position", "No data")),
                "vehicle_speed": self.hex_to_dec(results.get("vehicle_speed", "No data")),
                "engine_load": self.hex_to_dec(results.get("engine_load", "No data")),
                "fuel_level": self.hex_to_dec(results.get("fuel_level", "No data")),
                "oil_temperature": self.hex_to_dec(results.get("oil_temperature", "No data")),
                "fuel_pressure": self.hex_to_dec(results.get("fuel_pressure", "No data")),
                "intake_air_temperature": self.hex_to_dec(results.get("intake_air_temperature", "No data")),
                "time_stamp": datetime.datetime.now().isoformat()
            }   


            self.bus.shutdown()
            log_info_message(logger, "Sending JSON response")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message
        
    def read_from_hvac(self, item=None):
        """
        Method to read information from the HVAC module.

        Args:
        - item: Optional identifier for a specific data item to read.

        Returns:
        - JSON response with HVAC data.
        """
        try:
            log_info_message(logger, "Reading data from HVAC")
            id = self.my_id * 0x100 + self.id_ecu[ECU_HVAC]  

            identifiers = data_identifiers["HVAC_Identifiers"]  
            results = {}

            if item:
                if item in identifiers:
                    identifier = identifiers[item]
                    result_value = self._read_by_identifier(id, int(identifier, 16))

                    results[item] = result_value if result_value else "No data"
                else:
                    return {"error": f"Invalid parameter '{item}'. Use /get_identifiers to see valid parameters."}
            else:
                for key, identifier in identifiers.items():
                    result_value = self._read_by_identifier(id, int(identifier, 16))
                    results[key] = result_value if result_value else "No data"

            response_json = {
                "mass_air_flow": self.hex_to_dec(results.get("mass_air_flow", "No data")),
                "ambient_air_temperature": self.hex_to_dec(results.get("ambient_air_temperature", "No data")),
                "cabin_temperature": self.hex_to_dec(results.get("cabin_temperature", "No data")),
                "cabin_temperature_driver_set": self.hex_to_dec(results.get("cabin_temperature_driver_set", "No data")),
                "fan_speed": self.hex_to_dec(results.get("fan_speed", "No data")),
                "hvac_modes": self.hex_to_dec(results.get("hvac_modes", "No data")),
                "time_stamp": datetime.datetime.now().isoformat()
            }

            self.bus.shutdown()
            log_info_message(logger, "Sending JSON response")
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return {"error": str(e)}


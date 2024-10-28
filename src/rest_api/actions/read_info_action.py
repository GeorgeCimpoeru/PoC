import datetime
from actions.base_actions import *
from configs.data_identifiers import *


class ReadInfo(Action):
    """
    ReadInfo class to read information from different ECUs.

    Attributes:
    - my_id: Identifier for the device initiating the update.
    - id_ecu: Identifier for the specific ECU being updated.
    - g: Instance of GenerateFrame for generating CAN bus frames.
    """
    @staticmethod
    def _get_battery_state_of_charge(state_of_charge):
        if state_of_charge.startswith("0x"):
            state_of_charge = state_of_charge[2:]

        state_mapping = {
            "00": "Unknown state",
            "01": "Charging",
            "02": "Discharging",
            "03": "Empty",
            "04": "Fully charged",
            "05": "Pending charge",
            "06": "Pending discharge"
        }

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

    def _interpret_hvac_modes(self, value):
        """Interpret HVAC operating modes from a bitmask value."""

        modes = {
            "AC Status": bool(value & (1 << 0)),
            "Legs": bool(value & (1 << 1)),
            "Front": bool(value & (1 << 2)),
            "Air Recirculation": bool(value & (1 << 3)),
            "Defrost": bool(value & (1 << 4))
        }

        return modes

    def read_from_battery(self, item=None):
        """
        Method to read information from the battery module.

        Args:
        - item: Optional identifier for a specific data item to read.

        Returns:
        - JSON response.

        Endpoint test (external flow):

        curl -X GET "http://127.0.0.1:5000/api/read_info_battery?is_manual_flow=false" -H "Content-Type: application/json"

        """
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

                    response_json = {
                            item: self.hex_to_dec(result_value) if result_value else "No data",
                            "time_stamp": datetime.datetime.now().isoformat()
                        }
                    return response_json

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

            log_info_message(logger, "Sending JSON response")
            return response_json

        except CustomError:
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            return {
                "message": "ssue encountered during Read by ID",
                "negative_response": negative_response
            }

    def read_from_doors(self, item=None):
        """
        Method to read information from the door module.

        Args:
        - item: Optional identifier for a specific door status to read.

        Returns:
        - JSON response.

        Endpoint test (external flow):

        curl -X GET "http://127.0.0.1:5000/api/read_info_doors?is_manual_flow=false" -H "Content-Type: application/json"

        """
        try:
            log_info_message(logger, "Reading data from doors")
            id = self.my_id * 0x100 + self.id_ecu[ECU_DOORS]

            identifiers = data_identifiers["Doors_Identifiers"]
            results = {}

            def get_context(key):
                if key in ["door", "passenger", "driver"]:
                    return "door"
                elif key == "passenger_lock":
                    return "lock"
                elif key == "ajar":
                    return "ajar"
                else:
                    return "unknown"

            if item:
                if item in identifiers:
                    identifier = identifiers[item]
                    result_value = self._read_by_identifier(id, int(identifier, 16))
                    context = get_context(item)

                    results[item] = self._interpret_status(result_value, context) if result_value else "No data"

                    response_json = {
                        item: self.hex_to_dec(results.get(item, "No data")),
                        "time_stamp": datetime.datetime.now().isoformat()
                    }
                    return response_json
                else:
                    return {"error": f"Invalid parameter '{item}'. Use /get_identifiers to see valid parameters."}

            else:
                for key, identifier in identifiers.items():
                    result_value = self._read_by_identifier(id, int(identifier, 16))
                    context = get_context(key)

                    results[key] = self._interpret_status(result_value, context) if result_value else "No data"

                response_json = {
                    "door": self.hex_to_dec(results.get("door", "No data")),
                    "passenger": self.hex_to_dec(results.get("passenger", "No data")),
                    "passenger_lock": self.hex_to_dec(results.get("passenger_lock", "No data")),
                    "ajar": self.hex_to_dec(results.get("ajar", "No data")),
                    "time_stamp": datetime.datetime.now().isoformat()
                }

                log_info_message(logger, "Sending JSON response")
                return response_json

        except CustomError:
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            return {
                "message": "ssue encountered during Read by ID",
                "negative_response": negative_response
            }
        except Exception as e:
            return {"error": f"Unexpected error: {str(e)}"}

    def read_from_engine(self, item=None):
        """
        Method to read information from the engine module.

        Args:
        - item: Optional identifier for a specific data item to read.

        Returns:
        - JSON response with engine data or the data for a specific item if provided.

        Endpoint test (external flow):

        curl -X GET "http://127.0.0.1:5000/api/read_info_engine?is_manual_flow=false" -H "Content-Type: application/json"

        """
        try:
            log_info_message(logger, "Reading data from engine")
            id = self.my_id * 0x100 + self.id_ecu[ECU_ENGINE]

            identifiers = data_identifiers["Engine_Identifiers"]
            results = {}

            if item:

                if item in identifiers:
                    identifier = identifiers[item]
                    result_value = self._read_by_identifier(id, int(identifier, 16))
                    interpreted_value = self.hex_to_dec(result_value) if result_value else "No data"

                    response_json = {
                        item: interpreted_value,
                        "time_stamp": datetime.datetime.now().isoformat()
                    }

                    log_info_message(logger, "Sending JSON response")
                    return response_json
                else:
                    return {"error": f"Invalid parameter '{item}'. Use /get_identifiers to see valid parameters."}
            else:
                for key, identifier in identifiers.items():
                    result_value = self._read_by_identifier(id, int(identifier, 16))
                    results[key] = self.hex_to_dec(result_value) if result_value else "No data"

                response_json = {
                    **results,
                    "time_stamp": datetime.datetime.now().isoformat()
                }

                log_info_message(logger, "Sending JSON response")
                return response_json

        except CustomError:
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            return {
                "message": "ssue encountered during Read by ID",
                "negative_response": negative_response
            }

    def read_from_hvac(self, item=None):
        """
        Method to read information from the HVAC module.

        Args:
        - item: Optional identifier for a specific data item to read.

        Returns:
        - JSON response with HVAC data or the data for a specific item if provided.

        Endpoint test (external flow):
        curl -X GET "http://127.0.0.1:5000/api/read_info_hvac?is_manual_flow=false" -H "Content-Type: application/json"
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

                    if item == "hvac_modes":
                        interpreted_value = self._interpret_hvac_modes(self.hex_to_dec(result_value)) if result_value else "No data"
                    else:
                        interpreted_value = self.hex_to_dec(result_value) if result_value else "No data"

                    response_json = {
                        item: interpreted_value,
                        "time_stamp": datetime.datetime.now().isoformat()
                    }
                    log_info_message(logger, "Sending JSON response")
                    return response_json
                else:
                    return {"error": f"Invalid parameter '{item}'. Use /get_identifiers to see valid parameters."}
            else:
                for key, identifier in identifiers.items():
                    result_value = self._read_by_identifier(id, int(identifier, 16))

                    if key == "hvac_modes":
                        results[key] = self._interpret_hvac_modes(self.hex_to_dec(result_value)) if result_value else "No data"
                    else:
                        results[key] = self.hex_to_dec(result_value) if result_value else "No data"

                response_json = {
                    **results,
                    "time_stamp": datetime.datetime.now().isoformat()
                    }
                log_info_message(logger, "Sending JSON response")
                return response_json

        except CustomError:
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            return {
                "message": "Issue encountered during Read by ID",
                "negative_response": negative_response
            }

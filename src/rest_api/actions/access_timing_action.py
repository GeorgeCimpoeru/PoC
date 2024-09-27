from actions.base_actions import *
from configs.data_identifiers import *


class ReadAccessTiming(Action):
    def _read_timing_info(self, id, sub_funct=1):
        """
        Reads timing parameters of the ECU.
        Args:
        - id: The identifier of the target ECU or device.
        - sub_funct: The sub-function code to specify which timing parameters to read.
        Returns:
        - A dictionary with the status, message, and timing parameters (if successful).
        """
        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu

        try:
            id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + self.id_ecu[MCU]

            log_info_message(logger, "Changing session to programming")
            self.generate.access_timing_parameters(id, sub_funct)
            frame_response = self._passive_response(ACCESS_TIMING_PARAMETERS, "Error reading timing parameters")

            if len(frame_response.data) < 4:
                return {
                    "status": "error",
                    "message": "Unexpected or insufficient response length while reading timing parameters"
                }

            if frame_response.data[1] == 0x7F:
                nrc_msg = frame_response.data[3]
                sid_msg = frame_response.data[2]
                negative_response = self.handle_negative_response(nrc_msg, sid_msg)
                return {
                    "status": "error",
                    "message": "Negative response received while reading timing parameters",
                    "negative_response": negative_response
                }

            if frame_response.data[1] == 0xC3:
                log_info_message(logger, "Timing parameters read successfully")

                timing_values = list(frame_response.data[3:])

                if len(timing_values) >= 4:
                    value1_decimal = (timing_values[0] << 8) + timing_values[1]
                    value2_decimal = (timing_values[2] << 8) + timing_values[3]

                    if sub_funct == 1:
                        timing_values_dict = {
                            "P2_MAX_TIME_DEFAULT": f"{value1_decimal} seconds",
                            "P2_STAR_MAX_TIME_DEFAULT": f"{value2_decimal} milliseconds"
                        }
                    elif sub_funct == 3:
                        timing_values_dict = {
                            "p2_max_time": f"{value1_decimal} seconds",
                            "p2_star_max_time": f"{value2_decimal} milliseconds"
                        }
                    else:
                        return {
                            "status": "error",
                            "message": "Unexpected sub_function value"
                        }

                    return {
                        "status": "success",
                        "message": "Timing parameters accessed successfully",
                        "timing_values": timing_values_dict
                    }
                else:
                    return {
                        "status": "error",
                        "message": "Insufficient data length to read timing parameters"
                    }
            else:
                return {
                    "status": "error",
                    "message": "Unexpected response while reading timing parameters"
                }
        except Exception as e:
            logger.error(f"Error accessing timing parameters: {e}")
            return {"status": "error", "message": str(e)}


class WriteAccessTiming(Action):
    def _write_timing_info(self, id, timing_values):
        """
        Writes timing parameters to the ECU.

        Args:
        - id: The identifier of the target ECU or device.
        - timing_values: A dictionary containing the timing values to be written.

        Returns:
        - A dictionary with the status and message.
        """
        try:
            id_mcu = self.id_ecu[MCU]
            id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + id_mcu

            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(id, 0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            p2_max = timing_values.get("p2_max", 0)
            p2_star_max = timing_values.get("p2_star_max", 0)

            self.generate.write_timming_parameters(id, 0x04, p2_max, p2_star_max)
            frame_response = self._passive_response(ACCESS_TIMING_PARAMETERS, "Error writing timing parameters")

            if frame_response.data[1] == 0xC3:
                log_info_message(logger, "Timing parameters written successfully")

                return {
                    "status": "success",
                    "message": "Timing parameters written successfully",
                    "written_values": {
                        "New P2 Max Time": p2_max,
                        "New P2 Star Max": p2_star_max
                    }
                }
            else:
                return {
                    "status": "error",
                    "message": "Unexpected response while writing timing parameters"
                }
        except Exception as e:
            logger.error(f"Error writing timing parameters: {e}")
            return {"status": "error", "message": str(e)}

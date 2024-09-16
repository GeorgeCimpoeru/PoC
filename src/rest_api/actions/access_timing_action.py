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
            self.generate.session_control(id, 0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            self.generate.access_timing_parameters(id, sub_funct)
            frame_response = self._passive_response(ACCESS_TIMING_PARAMETERS, "Error reading timing parameters")

            if frame_response.data[1] == 0xC3:
                log_info_message(logger, "Timing parameters read successfully")

                timing_values = list(frame_response.data[3:])

                if len(timing_values) >= 4:
                    # Convert bytes to decimal
                    value1_decimal = (timing_values[0] << 8) + timing_values[1]
                    value2_decimal = (timing_values[2] << 8) + timing_values[3]

                    if sub_funct == 1:
                        # Sub-function 1 specific keys
                        timing_values_dict = {
                            "P2_MAX_TIME_DEFAULT": f"{value1_decimal} seconds",
                            "P2_STAR_MAX_TIME_DEFAULT": f"{value2_decimal} milliseconds"
                        }
                    elif sub_funct == 3:
                        # Sub-function 3 specific keys
                        timing_values_dict = {
                            "p2_max_time": f"{value1_decimal} seconds",
                            "p2_star_max_time": f"{value2_decimal} milliseconds"
                        }
                    else:
                        # Handle unexpected sub_function values
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

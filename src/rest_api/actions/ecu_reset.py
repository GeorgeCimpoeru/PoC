from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class Reset(Action):
    """ curl -X POST http://127.0.0.1:5000/api/reset_ecu \
    -H "Content-Type: application/json" \
    -d '{
        "type_reset": "hard_reset",
        "ecu_id": "0x10"
    }'
    """
    def reset_ecu(self, ecu_id, type_reset):
        """
        Resets the ECU based on the provided ECU ID and reset type.

        Args:
        - ecu_id: The ID of the ECU to reset.
        - type_reset: The type of reset to perform ("hard" or "soft").

        Returns:
        - A response JSON indicating the reset status and CAN ID.
        """

        id = None
        if ecu_id == "10":
            id = self.my_id * 0x100 + self.id_ecu[0]
        elif ecu_id == "11":
            id = self.my_id * 0x100 + self.id_ecu[1]
        else:
            log_error_message(logger, f"Invalid ECU ID: {ecu_id}")
            return {"status": "error", "message": f"Invalid ECU ID: {ecu_id}"}

        try:
            if type_reset == "hard":
                self.ecu_reset(id, 0x01, False)
            elif type_reset == "soft":
                self.ecu_reset(id, 0x02, False)
            else:
                log_error_message(logger, f"Invalid reset type: {type_reset}")
                return {"status": "error", "message": f"Invalid reset type: {type_reset}"}

            frame_response = self._passive_response(RESET_ECU, f"Error resetting device {hex(id)}")

            if frame_response.data[1] == 0x51 and frame_response.data[2] == 0x02:
                response_json = {
                    "status": "success",
                    "message": "ECU reset successful",
                    "can_id": f"0x{id:03X}",
                }

            log_info_message(logger, f"Success resetting device {id}")
            return response_json

        except CustomError as e:
            log_error_message(logger, f"Error during ECU reset: {e.message}")
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            return {
                "status": "error",
                "message": "Error during ECU reset",
                "negative_response": negative_response
            }

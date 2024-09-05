from actions.base_actions import *
from configs.data_identifiers import *


class SessionManager(Action):
    def _change_session(self, id, sub_funct=1):
        """
        Changes the session control based on a given sub-function.

        Args:
        - id: The identifier of the target ECU or device.
        - sub_funct: The sub-function code to change the session.

        Returns:
        - A dictionary with the status and message of the operation.
        """
        
        id = (API_ID << 8) + 0x10

        try:
            
            session_type = "DEFAULT" if sub_funct == 0x01 else "PROGRAMMING" if sub_funct == 0x02 else "unknown"
            log_info_message(logger, f"Changing session to {session_type} (sub_function: {sub_funct})")

            self.generate.session_control(id, 0x01)
            frame_response = self._passive_response(SESSION_CONTROL, "Error changing session control")

            if frame_response.data[1] == 0x50:
                log_info_message(logger, f"Session changed to {session_type}")
                return {"status": "success", "message": f"Session changed to {session_type} successfully"}
        except CustomError as e:
            return {"status": "error", "message": str(e)}
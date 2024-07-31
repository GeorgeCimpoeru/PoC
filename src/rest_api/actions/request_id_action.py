from database.can_frame_database_handler import CanFrameDatabaseHandler
from database.mcu_ids_database_handler import McuIdsDatabaseHandler
from actions.base_actions import *
import time

class RequestIdAction(Action):
    def run(self):
        """
        Executes the ID request action and processes the response.

        Returns:
        - JSON response containing the status and any collected data or errors.
        """

        self.can_db_handler = CanFrameDatabaseHandler()
        self.mcu_db_handler = McuIdsDatabaseHandler()
        self.id = self.my_id
        try:
            self._send_request_frame()
            response_data = self._read_response_frames()
            response_json = self._to_json(response_data)
            self._close_db_handler()
            self.bus.shutdown()
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def _send_request_frame(self):
        """
        Sends the request frame to the ECU and logs the action.
        """
        log_info_message(logger, "Sending request frame for ECU IDs")
        self.generate.send_frame(self.id, [0x02, 0x99])
        self.can_db_handler.save_frame("request_ids", self.id, str([0x02, 0x99]))
        log_info_message(logger, "Request frame sent")

    def _read_response_frames(self, timeout=10):
        """
        Reads and processes response frames within a specified timeout.

        Args:
        - timeout: Time in seconds to wait for responses (default: 5.0).

        Returns:
        - Dictionary containing the status and any collected data.
        """
        log_info_message(logger, "Waiting for response on service: request_ids")
        end_time = time.time() + timeout
        while time.time() < end_time:
            response = self.bus.recv(timeout)
            if response:
                data = response.data
                if len(data) < 8:
                    return {"status": "Incomplete response received"}

                pcl = data[0]
                sid = data[1]
                if sid != 0x99:  # Check for SID 0x99 (153 in decimal)
                    return {"status": f"Invalid SID received: {sid}"}

                id_mcu = data[2]
                id_bat = data[3]
                id_dos = data[4]
                id_eng = data[5] if len(data) > 5 else None

                self.mcu_db_handler.save_mcu_id(response.arbitration_id, pcl, sid, id_mcu, id_bat, id_dos, id_eng)

                result = {
                    "status": "Success",
                    "arbitration_id": response.arbitration_id,
                    "pcl": pcl,
                    "sid": sid,
                    "id_mcu": id_mcu,
                    "id_bat": id_bat,
                    "id_dos": id_dos,
                }
                if id_eng is not None:
                    result["id_eng"] = id_eng

                return result
        return {"status": "No response received within timeout"}

    def _to_json(self, data):
        """
        Creates a JSON response with the given data and timestamp.

        Args:
        - data: Data to be included in the JSON response.

        Returns:
        - JSON-formatted response containing the data and timestamp.
        """
        data["time_stamp"] = datetime.datetime.now().isoformat()
        return json.dumps(data)

    def _close_db_handler(self):
        """
        Closes the database handlers.
        """
        self.can_db_handler.close()
        self.mcu_db_handler.close()

from database.mcu_ids_database_handler import McuIdsDatabaseHandler
from actions.base_actions import *
import time
from config import Config
import datetime

class IDsToJson():
    def _to_json(self, data):
            """
            Creates a JSON response with the given data and timestamp.

            Args:
            - data: Data to be included in the JSON response.

            Returns:
            - JSON-formatted response containing the data and timestamp.
            """
            response = {
                "status": data.get("status"),
                "mcu_id": data.get("mcu_id"),
                "ecu_ids": data.get("ecu_ids", []),
                "time_stamp": datetime.datetime.now().isoformat()
            }
            if "reason" in data:
                response["reason"] = data["reason"]
            return response

class RequestIdAction(Action):
    def read_ids(self):
        """
        Executes the ID request action and processes the response.

        Returns:
        - JSON response containing the status and any collected data or errors.
        """
        self.mcu_db_handler = McuIdsDatabaseHandler()
        self.id = self.my_id
        try:
            self._send_request_frame()
            response_data = self._read_response_frames()
            response_json = IDsToJson()._to_json(response_data)

            # Log the contents of the MCU IDs database before closing the handler
            self.mcu_db_handler.read_all_mcu_ids()
            self._close_db_handler()
            self.bus.shutdown()

            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return {"status": "Error", "message": str(e)}

    def _send_request_frame(self):
        """
        Sends the request frame to the ECU and logs the action.
        """
        log_info_message(logger, "Sending request frame for ECU IDs")
        self.generate.send_frame(self.id, [0x01, 0x99])
        log_info_message(logger, "Request frame sent")

    def _read_response_frames(self, timeout=10):
        """
        Reads and processes response frames within a specified timeout.

        Args:
        - timeout: Time in seconds to wait for responses (default: 10.0).

        Returns:
        - Dictionary containing the status and any collected data.
        """
        log_info_message(logger, "Waiting for response on service: request_ids")
        end_time = time.time() + timeout
        while time.time() < end_time:
            response = self.bus.recv(Config.BUS_RECEIVE_TIMEOUT)
            if response:
                data = response.data
                hex_data = [f"{byte:02X}" for byte in data]
                log_info_message(logger, f"Raw Data (hex): {hex_data}")

                if len(data) < 3:
                    return {"status": "Invalid response length"}

                if data[1] == 0xD9:
                    # Positive response
                    mcu_id = int(f"{data[2]:02X}", 16)
                    ecu_ids = [int(f"{byte:02X}", 16) for byte in data[3:]]
                    data_dict = {
                        "status": "Success",
                        "mcu_id": f"{mcu_id:02X}",
                        "ecu_ids": [f"{ecu_id:02X}" for ecu_id in ecu_ids]
                    }
                    # Save data to the database
                    id_mcu = mcu_id
                    id_ecu1 = ecu_ids[0] if len(ecu_ids) > 0 else None
                    id_ecu2 = ecu_ids[1] if len(ecu_ids) > 1 else None
                    id_ecu3 = ecu_ids[2] if len(ecu_ids) > 2 else None
                    id_ecu4 = ecu_ids[3] if len(ecu_ids) > 3 else None
                    self.mcu_db_handler.save_mcu_id(id_mcu, id_ecu1, id_ecu2, id_ecu3, id_ecu4)
                else:
                    # Negative response or unexpected data
                    data_dict = {
                        "status": "Failure",
                        "reason": f"Unexpected response code: {data[1]:02X}"
                    }

                log_info_message(logger, f"Data Dict: {data_dict}")

                return data_dict

        return {"status": "No response received within timeout"}

    def _close_db_handler(self):
        """
        Closes the database handlers.
        """
        self.mcu_db_handler.close()

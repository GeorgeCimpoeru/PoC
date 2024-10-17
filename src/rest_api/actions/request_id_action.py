from actions.base_actions import *
import time
from config import Config
import datetime
from .secure_auth import Auth

class IDsToJson():
    def _to_json(self, data):
        response = {
            "status": data.get("status"),
            "mcu_id": data.get("mcu_id"),
            "ecu_ids": data.get("ecu_ids", []),
            "time_stamp": datetime.datetime.now().isoformat(),
            "versions": data.get("versions, {}")
        }
        if "reason" in data:
            response["reason"] = data["reason"]
        return response


class RequestIdAction(Action):
    def __init__(self):
        super().__init__()

    def read_ids(self):
        """ curl -X GET http://127.0.0.1:5000/api/request_ids """
        self.arb_id = (0x00 << 16) + (self.my_id << 8) + 0x99
        try:
            self._send_request_frame()
            response_data = self._read_response_frames()
            versions = self._verify_version(response_data["ecu_ids"])
            response_data["versions"] = versions
            response_json = IDsToJson()._to_json(response_data)
            return response_json

        except CustomError as e:
            return {"status": "Error", "message": str(e)}

    def _send_request_frame(self):
        log_info_message(logger, "Sending request frame for ECU IDs")
        self.send(self.arb_id, [0x01, 0x99])
        log_info_message(logger, "Request frame sent")

    def _read_response_frames(self, timeout=10):
        end_time = time.time() + timeout
        while time.time() < end_time:
            response = self.bus.recv(Config.BUS_RECEIVE_TIMEOUT)
            if response:
                data = response.data
                if len(data) < 3:
                    return {"status": "Invalid response length"}

                if data[1] == 0x7F:
                    nrc_msg = data[3]
                    sid_msg = data[2]
                    negative_response = self.handle_negative_response(nrc_msg, sid_msg)
                    return {
                        "status": "error",
                        "message": f"Negative response received while resetting device {id}",
                        "negative_response": negative_response
                    }

                if data[1] == 0xD9:
                    # Positive response
                    mcu_id = int(f"{data[2]:02X}", 16)
                    ecu_ids = [int(f"{byte:02X}", 16) for byte in data[3:]]
                    data_dict = {
                        "status": "Success",
                        "mcu_id": f"{mcu_id:02X}",
                        "ecu_ids": [f"{ecu_id:02X}" for ecu_id in ecu_ids]
                    }
                return data_dict
        return {"status": "No response received within timeout"}

    def _verify_version(self, ecu_ids):
        """
        Verify the software versions for each ECU ID.
        Returns a dictionary with ECU IDs and their respective versions.
        """

        # ses_id = (0x00 << 16) + (self.my_id << 8) + 0x10
        # self.session_control(ses_id, 0x02)
        # auth = Auth()
        # auth._auth_to()
        # frame_response = self._passive_response(SESSION_CONTROL, "Error changing session control")

        # if frame_response.data[1] == 0x50:
            # log_info_message(logger, "Verifying versions for each ECU ID")

        versions = {}
        for ecu_id in ecu_ids:
            if ecu_id == "00":
                versions[ecu_id] = "no version read"
                continue

            formatted_ecu_id = f"0x{ecu_id}"
            version = self._read_version(formatted_ecu_id)
            if version:
                versions[ecu_id] = version
            else:
                versions[ecu_id] = "no version read"

        return versions

    def _read_version(self, ecu_id):
        """
        Read the software version for a given ECU ID.
        """
        try:
            id_ecu_int = int(ecu_id, 16)
            ID_ECU = (0x00 << 16) + (self.my_id << 8) + id_ecu_int
            log_info_message(logger, f"Reading IDss: {ID_ECU}")
            version = self._read_by_identifier(ID_ECU, IDENTIFIER_SYSTEM_SUPPLIER_ECU_SOFTWARE_VERSION_NUMBER)

            if version:
                log_info_message(logger, f"Version found for ECU ID {hex(id_ecu_int)}: {version}")
                return version

        except CustomError as e:
            log_error_message(logger, f"Error reading version for ECU ID {id_ecu_int}: {str(e)}")
            return None

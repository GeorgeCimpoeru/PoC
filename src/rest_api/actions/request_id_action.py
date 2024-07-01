import can
from database.can_frame_database_handler import CanFrameDatabaseHandler
from database.mcu_ids_database_handler import McuIdsDatabaseHandler
import time
import datetime
import json
from .generate_frames import GenerateFrame
import subprocess

class CustomError(Exception):
    def __init__(self, message):
        self.message = message

class FrameRequester:
    def __init__(self, bus):
        self.bus = bus
        self.generator = GenerateFrame(self.bus)
        self.can_db_handler = CanFrameDatabaseHandler()
        self.mcu_db_handler = McuIdsDatabaseHandler()

    def _send_request_frame(self, service_name, arbitration_id, data):
        self.generator.send_frame(arbitration_id, data)
        self.can_db_handler.save_frame(service_name, arbitration_id, data)
        return "Request frame sent."

    def _read_response_frames(self, service_name, timeout=5.0):
        print(f"Waiting for response on service: {service_name}")
        end_time = time.time() + timeout
        while time.time() < end_time:
            response = self.generator.read_frame(timeout=timeout)
            if response:
                data = response.data
                if len(data) < 8:
                    return "Incomplete response received."

                pcl = data[0]
                sid = data[1]
                if sid != 0x99:  # Check for SID 0x99 (153 in decimal)
                    return f"Invalid SID received: {sid}"

                id_mcu = data[2]  # 1 byte
                id_bat = data[3]  # 1 byte
                id_dos = data[4]  # 1 byte
                id_eng = data[5] if len(data) > 5 else None  # 1 byte

                self.mcu_db_handler.save_mcu_id(response.arbitration_id, pcl, sid, id_mcu, id_bat, id_dos, id_eng)

                result = f"ID: {response.arbitration_id}\nDATA: PCL: {pcl}, SID: {sid}, IDMCU: {id_mcu}, IDBAT: {id_bat}, IDDOS: {id_dos}"
                if id_eng is not None:
                    result += f", IDENG: {id_eng}"
                
                return result
        return "No response received within timeout."

    def request_ids(self):
        frame_status = self._send_request_frame("request_ids", 0xFA, [0x02, 0x99])
        time.sleep(0.5)  # Ensure there's a short delay before reading the response
        response_status = self._read_response_frames("request_ids")
        return frame_status + "\n" + response_status

    def get_last_frame_for_service(self, service_name):
        return self.can_db_handler.get_last_frame_for_service(service_name)

    def version_to_int(self, array):
        n = 0
        for element in array[::-1]:
            n = n * 100 + element
        return n

    def close(self):
        self.can_db_handler.close()
        self.mcu_db_handler.close()

import can
from generate_frames import GenerateFrame
from can_frame_database_handler import CanFrameDatabaseHandler
from mcu_ids_database_handler import McuIdsDatabaseHandler
import time
import datetime
import json

class CustomError(Exception):
    def __init__(self, message):
        self.message = message

class FrameRequester:
    def __init__(self, can_interface):
        self.generator = GenerateFrame(can_interface)
        self.can_db_handler = CanFrameDatabaseHandler()
        self.mcu_db_handler = McuIdsDatabaseHandler()
        self.bus = can.interface.Bus(channel=can_interface, interface='socketcan')

    def check_interface(self):
        if self.generator.check_interface_is_up():
            return "Interface is up."
        else:
            return "Interface is down."

    def send_request_frame(self, service_name, arbitration_id, data):
        self.generator.send_frame(arbitration_id, data)
        self.can_db_handler.save_frame(service_name, arbitration_id, data)
        return "Request frame sent."

    def read_response_frames(self, service_name, timeout=5.0):
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
        interface_status = self.check_interface()
        if "up" in interface_status:
            frame_status = self.send_request_frame("request_ids", 0xFA, [0x02, 0x99])
            time.sleep(0.5)  # Ensure there's a short delay before reading the response
            response_status = self.read_response_frames("request_ids")
            return interface_status + "\n" + frame_status + "\n" + response_status
        else:
            return interface_status

    def get_last_frame_for_service(self, service_name):
        return self.can_db_handler.get_last_frame_for_service(service_name)

    def update_to(self, version, data=[]):
        self.data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
        self.id = self.my_id + self.id_ecu
        try:
            # Verify Version of the software in the ECU
            self.verify_version(version)
            print("Proceed to download new version")
            # Change Session
            self.g.session_control(self.id, 0x02)
            self._passive_response(0x10, "Error changing session control")

            # Download
            print("Downloading... please wait")
            self.download_data(data)

            # Change session & reset
            print("Download finished, restarting ECU...")
            self.g.session_control(self.id, 0x01)
            self._passive_response(0x10, "Error changing session control")

            self.g.ecu_reset(self.id)
            self._passive_response(0x11, "Error trying to reset ECU")

            # Add sleep to wait until the ECU is on again. Adjust seconds as necessary
            time.sleep(1)

            # Check for errors
            no_errors = self.check_errors()
            if no_errors != -1:
                response_json = self._to_json("downloaded", no_errors)
            else:
                response_json = self._to_json("interrupted", 1)

            self.bus.shutdown()
            return response_json
        
        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def version_to_int(self, array):
        n = 0
        for element in array[::-1]:
            n = n * 100 + element
        return n

    def download_data(self, data):
        self.g.request_download(self.id, 0x01, 0x01, 0xFFFF)
        self._passive_response(0x34, "Error requesting download")

        self.g.transfer_data_long(self.id, 0x01, data)
        self.g.transfer_data_long(self.id, 0x01, data, False)
        self._passive_response(0x36, "Error transferring data")

        self.g.request_transfer_exit(self.id)
        self._passive_response(0x37, "Error requesting transfer exit")

    def verify_version(self, version):
        self.g.read_data_by_identifier(self.id, IDENTIFIER_VERSION_SOFTWARE_MCU)
        response = self._collect_response(0x22)
        if response is not None:
            current_version = self.version_to_int(response.data[4:response.data[0] + 2])
            print(current_version)
            if current_version == version:
                print("Already installed latest version")
                response_json = self._to_json("already installed", 0)
                raise CustomError(json.dumps(response_json))
            else:
                return ""
        else:
            print("Error verifying version")
            response_json = self._to_json("interrupted", 1)
            raise CustomError(json.dumps(response_json))

    def check_errors(self):
        self.g.request_read_dtc_information(self.id, 0x01, 0x01)
        response = self._collect_response(0x19)
        if response is not None:
            number_of_dtc = response.data[5]
            print("There are {} errors found after download".format(number_of_dtc))
            return number_of_dtc
        else:
            print("Error reading DTC")
            return -1

    def _to_json(self, status: str, no_errors):
        response_to_frontend = {
            "status_download": status,
            "errors": no_errors,
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return json.dumps(response_to_frontend)

    def close(self):
        self.can_db_handler.close()
        self.mcu_db_handler.close()

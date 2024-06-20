'''
Authot: Mujdei Ruben
How to use?
    u = Updates("vcan0", 0x23, 0x12)
    u.update_to(0x12)
'''

from actions import *
from generate_frames import GenerateFrame as GF
import time
import json
import datetime

class Updates(Action):

    #Public
    def update_to(self, version, data=[]):
        self.data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
        self.id = self.my_id + self.id_ecu
        try:
            # Verify Version of the software in the ECU
            self._verify_version(version)
            print("Proceed to download new version")
            # Change Session
            self.g.session_control(self.id, 0x02)
            self._passive_response(0x10, "Error changing session control")

            # Download
            print("Downloading... please wait")
            self._download_data(data)

            # Change session & reset
            print("Download finished, restarting ECU...")
            self.g.session_control(self.id, 0x01)
            self._passive_response(0x10, "Error changing session control")

            self.g.ecu_reset(self.id)
            self._passive_response(0x11, "Error trying to reset ECU")

            # Add sleep to wait until the ECU is on again. Adjust seconds as necessary
            time.sleep(1)

            # Check for errors
            no_errors = self._check_errors()
            response_json = self._to_json("downloaded",no_errors)

            self.bus.shutdown()
            return response_json
        
        except CustomError as e:

            self.bus.shutdown()
            return e.message

    #Private
    def _download_data(self, data):
        self.g.request_download(self.id, 0x01, 0x01, 0xFFFF)
        self._passive_response(0x34, "Error requesting download")

        self.g.transfer_data_long(self.id, 0x01, data)
        self.g.transfer_data_long(self.id, 0x01, data, False)
        self._passive_response(0x36, "Error transferring data")

        self.g.request_transfer_exit(self.id)
        self._passive_response(0x37, "Error requesting transfer exit")

    def _verify_version(self, version):
        self.g.read_data_by_identifier(self.id, IDENTIFIER_VERSION_SOFTWARE_MCU)
        response = self._passive_response(0x22, "Error verifying version")
        current_version = self._version_to_int(self._data_from_frame(response))
        if current_version == version:
            print("Already installed latest version")
            response_json = self._to_json("allready installed",0)
            raise CustomError(response_json)

    def _check_errors(self):
        self.g.request_read_dtc_information(self.id, 0x01, 0x01)
        response = self._passive_response(0x19, "Error reading DTC")
        if response is not None:
            number_of_dtc = response.data[5]
            print("There are {} errors found after download".format(number_of_dtc))
            return number_of_dtc

    def _to_json(self,status:str, no_errors):
        response_to_frontend = {
            "status_download": status,
            "errors": no_errors,
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return json.dumps(response_to_frontend)
    def _version_to_int(self, array):
        n = 0
        for element in array[::-1]:
            n = n * 100 + element
        return n

u = Updates("vcan0", 0x23, 0x12)
print(u.update_to(0x12))
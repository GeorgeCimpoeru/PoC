'''
Authot: Mujdei Ruben 6/2024
Use class Update to update the software of an ECU
Future implementations:
    Download from cloud
    Start Routines
How to use?
    u = Updates(bus, 0x23, 0x12)
    u.update_to(0x12)
'''

import json
import datetime
import time
from .actions import *  # Assuming this imports necessary actions
from .generate_frames import GenerateFrame as GF

class Updates(Action):
    """
    Update class for managing software updates on an Electronic Control Unit (ECU).
    
    Attributes:
    - bus: CAN bus interface for communication.
    - my_id: Identifier for the device initiating the update.
    - id_ecu: Identifier for the specific ECU being updated.
    - g: Instance of GenerateFrame for generating CAN bus frames.
    """

    def update_to(self, version, data=[]):
        """
        Method to update the software of the ECU to a specified version.

        Args:
        - version: Desired version of the software.
        - data: Optional data to be used in the update process (default is an empty list).

        Returns:
        - JSON response indicating the status of the update and any errors encountered.

        Raises:
        - CustomError: If the current software version matches the desired version,
          indicating that the latest version is already installed.
        """
        self.data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
        self.id = self.my_id + self.id_ecu
        
        try:
            # Verify if the current version matches the desired version
            self._verify_version(version)
            print("Proceeding to download new version")

            # Change session to initiate download process
            self.g.session_control(self.id, 0x02)
            self._passive_response(0x10, "Error changing session control")

            # Download the software update data
            print("Downloading... Please wait")
            self._download_data(data)

            # Change session back and reset the ECU to apply the update
            print("Download finished, restarting ECU...")
            self.g.session_control(self.id, 0x01)
            self._passive_response(0x10, "Error changing session control")

            self.g.ecu_reset(self.id)
            self._passive_response(0x11, "Error trying to reset ECU")

            # Add a delay to wait until the ECU completes the reset process
            time.sleep(1)

            # Check for errors in the updated ECU
            no_errors = self._check_errors()

            # Generate a JSON response indicating the success of the update
            response_json = self._to_json("downloaded", no_errors)

            # Shutdown the CAN bus interface
            # self.bus.shutdown()

            return response_json
        
        except CustomError as e:
            # Handle custom errors, shutdown the CAN bus, and return the error message
            # self.bus.shutdown()
            return e.message

    def _download_data(self, data):
        """
        Private method to handle the download process of software update data.

        Args:
        - data: Data to be transferred during the download process.

        Raises:
        - CustomError: If any error occurs during the download process.
        """
        self.g.request_download(self.id, 0x01, 0x01, 0xFFFF)
        self._passive_response(0x34, "Error requesting download")

        self.g.transfer_data_long(self.id, 0x01, data)
        self.g.transfer_data_long(self.id, 0x01, data, False)
        self._passive_response(0x36, "Error transferring data")

        self.g.request_transfer_exit(self.id)
        self._passive_response(0x37, "Error requesting transfer exit")

    def _verify_version(self, version):
        """
        Private method to verify if the current software version matches the desired version.

        Args:
        - version: Desired version of the software.

        Raises:
        - CustomError: If the current software version matches the desired version,
          indicating that the latest version is already installed.
        """
        self.g.read_data_by_identifier(self.id, IDENTIFIER_VERSION_SOFTWARE_MCU)
        response = self._passive_response(0x22, "Error verifying version")
        
        current_version = self._version_to_int(self._data_from_frame(response))
        
        if current_version == version:
            print("Already installed latest version")
            response_json = self._to_json("already installed", 0)
            raise CustomError(response_json)

    def _check_errors(self):
        """
        Private method to check for Diagnostic Trouble Codes (DTCs) in the updated ECU.

        Returns:
        - Number of errors (DTCs) found in the ECU.

        Raises:
        - CustomError: If any error occurs during the error checking process.
        """
        self.g.request_read_dtc_information(self.id, 0x01, 0x01)
        response = self._passive_response(0x19, "Error reading DTC")
        
        if response is not None:
            number_of_dtc = response.data[5]
            print(f"There are {number_of_dtc} errors found after download")
            return number_of_dtc

    def _to_json(self, status: str, no_errors):
        """
        Private method to create a JSON response with status and error information.

        Args:
        - status: Status of the download process ("downloaded" or "already installed").
        - no_errors: Number of errors found in the ECU after the update.

        Returns:
        - JSON-formatted response containing status, errors, and timestamp.
        """
        response_to_frontend = {
            "status_download": status,
            "errors": no_errors,
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return json.dumps(response_to_frontend)

    def _version_to_int(self, array):
        """
        Private method to convert an array of bytes representing version information to an integer.

        Args:
        - array: Array of bytes representing version information.

        Returns:
        - Integer representation of the version.
        """
        n = 0
        for element in array[::-1]:
            n = n * 100 + element
        return n
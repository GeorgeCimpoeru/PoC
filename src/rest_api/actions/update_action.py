'''
Authot: Mujdei Ruben 6/2024
Use class Update to update the software of an ECU
Future implementations:
    Download from cloud
    Start Routines

How to use?
    u = Updates( 0x23, [0x11,0x12,0x13])
    u.update_to(id_ecu, "12")
'''

from actions.base_actions import *  # Assuming this imports necessary actions
import time

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class ToJSON():
    def _to_json(self, status: str, no_errors):
        response = {
            "status_download": status,
            "errors": no_errors,
            "time_stamp": datetime.datetime.now().isoformat()
        }
        return response


class Updates(Action):
    # def _auth_mcu(self):
    #     id = self.my_id * 0x100 + self.id_ecu[0]
    #     try:
    #         log_info_message(logger, "Changing session to default")
    #         self.generate.session_control(id, 0x02)
    #         self._passive_response(SESSION_CONTROL, "Error changing session control")
    #         self._authentication(id)

    #     except CustomError as e:
    #         self.bus.shutdown()
    #         return e.message

    """
    Update class for managing software updates on an Electronic Control Unit (ECU).

    Attributes:
    - my_id: Identifier for the device initiating the update.
    - id_ecu: Identifier for the specific ECU being updated.
    - g: Instance of GenerateFrame for generating CAN bus frames.
    """

    def update_to(self, ecu_id, version: str, data=[]):
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
        # self.data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]

        try:
            self.id = (self.my_id * 0x100) + int(ecu_id, 16)
            log_info_message(logger, "Reading data from battery")
            current_version = self._verify_version(version)
            if current_version == version:
                response_json = ToJSON()._to_json(f"Version {version} already installed", 0)
                self.bus.shutdown()
                return response_json

            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(self.id, sub_funct=0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")
            self._authentication(self.my_id * 0x100 + self.id_ecu[0])

            log_info_message(logger, "Downloading... Please wait")
            self._download_data(ecu_id, data)
            log_info_message(logger, "Download finished, restarting ECU...")

            log_info_message(logger, "Changing session to default")
            # self.id = self.my_id * 0x100 + self.id_ecu[0]
            self.generate.session_control(self.id, 0x01)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            # Reset the ECU to apply the update
            # self.id = (self.my_id * 0x100) + int(ecu_id, 16)
            self.generate.ecu_reset(self.id)
            self._passive_response(RESET_ECU, "Error trying to reset ECU")

            # Add a delay to wait until the ECU completes the reset process
            log_info_message(logger, "Waiting until ECU is up")
            time.sleep(1)

            # Check for errors in the updated ECU
            log_info_message(logger, "Checking for errors..")
            no_errors = self._check_errors()

            # Generate a JSON response indicating the success of the update
            response_json = ToJSON()._to_json("downloaded", no_errors)

            # Shutdown the CAN bus interface
            self.bus.shutdown()

            log_info_message(logger, "Sending JSON")
            return response_json

        except CustomError as e:
            # Handle custom errors, shutdown the CAN bus, and return the error message
            self.bus.shutdown()
            return e.message

    def _download_data(self, ecu_id, data=[]):
        """
        Private method to handle the download process of software update data.
        Data format identifier:
            0x00 means that no compression/encryption method is used
            0x01 means that only encryption is used
            0x10 means that only compression is used
            0x11 means that both encryption and compression are used
        Download_type = 1 byte
            0x00 => 0b 000 0000 0
                                ^ this bit is used to determine the download type(0-> manual, 1-> auto)
                           ^^^^ these bits are used to determine update iteration(ranges between 0 and 15)
                       ^^^ these bits are used to determine update version(ranges between 0 and 7)
            -> for example 0x00 -> 0b 000 0000 0 -> this value represents manual update because the first bit is 0,
                the next 4 bits are representing the iteration of update like 1.x, and the next 3 bits are representing the version, in this case 1.0
            -> 000 0000 -> version 1.0
            -> 000 0001 -> version 1.1
            …
            -> 001 0000 -> version 2.0
            -> 001 0001 -> version 2.1
            …
        Args:
        - data: Data to be transferred during the download process.

        Raises:
        - CustomError: If any error occurs during the download process.
        """
        id = self.my_id * 0x100 + self.id_ecu[0]
        self.generate.request_download(id,
                                       data_format_identifier=0x00,
                                       memory_address=0x01,
                                       memory_size=0xFFFF,
                                       size=0x01)
        frame = self._passive_response(REQUEST_DOWNLOAD, "Error requesting download")
        # max_number_block = frame[9]
        log_info_message(logger, f"Max block: {frame[2]}")
        log_info_message(logger, f"Max block: {frame[3]}")
        log_info_message(logger, f"Max block: {frame[4]}")
        log_info_message(logger, f"Max block: {frame[5]}")
        log_info_message(logger, f"Max block: {frame[6]}")
        log_info_message(logger, f"Max block: {frame[7]}")
        log_info_message(logger, f"Max block: {frame[8]}")
        log_info_message(logger, f"Max block: {frame[9]}")
        # self.generate.transfer_data_long(self.id, 0x01, data)
        # self.generate.transfer_data_long(self.id, 0x01, data, False)
        # self._passive_response(TRANSFER_DATA, "Error transferring data")

        # self.generate.request_transfer_exit(self.id)
        # self._passive_response(REQUEST_TRANSFER_EXIT, "Error requesting transfer exit")

    def _verify_version(self, version):
        """
        Private method to verify if the current software version matches the desired version.

        Args:
        - version: Desired version of the software.

        Raises:
        - CustomError: If the current software version matches the desired version,
          indicating that the latest version is already installed.
        """
        log_info_message(logger, "Reading current version")
        self._write_by_identifier(self.id, IDENTIFIER_SYSTEM_ECU_FLASH_SOFTWARE_VERSION_NUMBER, value=12)
        current_version = self._read_by_identifier(self.id, IDENTIFIER_SYSTEM_ECU_FLASH_SOFTWARE_VERSION_NUMBER)
        return current_version

    def _check_errors(self):
        """
        Private method to check for Diagnostic Trouble Codes (DTCs) in the updated ECU.

        Returns:
        - Number of errors (DTCs) found in the ECU.

        Raises:
        - CustomError: If any error occurs during the error checking process.
        """
        self.generate.request_read_dtc_information(self.id, 0x01, 0x01)
        response = self._passive_response(READ_DTC, "Error reading DTC")

        if response is not None:
            number_of_dtc = response.data[5]
            log_info_message(logger, f"There are {number_of_dtc} errors found after download")
            return number_of_dtc

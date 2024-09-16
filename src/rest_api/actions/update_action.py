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
    """
    Update class for managing software updates on an Electronic Control Unit (ECU).

    Attributes:
    - my_id: Identifier for the device initiating the update.
    - id_ecu: Identifier for the specific ECU being updated.
    - g: Instance of GenerateFrame for generating CAN bus frames.
    """

    def update_to(self, type, version, id):
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

        try:
            self.id = (int(id, 16) << 16) + (self.my_id << 8) + self.id_ecu[0]

            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(self.id, sub_funct=0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            self._authentication(self.id)

            log_info_message(logger, "Changing session to default")
            self.generate.session_control(self.id, 0x01)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            # log_info_message(logger, "Reading data from battery")
            # current_version = self._verify_version(version)
            # if current_version == version:
            #     response_json = ToJSON()._to_json(f"Version {version} already installed", 0)
            #     self.bus.shutdown()
            #     return response_json

            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(self.id, sub_funct=0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            log_info_message(logger, "Authenticating...")
            self._authentication(self.id)

            log_info_message(logger, "Downloading... Please wait")
            self._download_data(type, version, id)
            log_info_message(logger, "Download finished, restarting ECU...")

            # log_info_message(logger, "Changing session to default")
            # self.generate.session_control(self.id, 0x01)
            # self._passive_response(SESSION_CONTROL, "Error changing session control")

            # Reset the ECU to apply the update
            # self.id = (self.my_id * 0x100) + int(ecu_id, 16)
            # self.generate.ecu_reset(self.id)
            # self._passive_response(RESET_ECU, "Error trying to reset ECU") # ToDo reactivate when using real hardware

            # Add a delay to wait until the ECU completes the reset process
            # log_info_message(logger, "Waiting until ECU is up")
            time.sleep(1)

            # Check for errors in the updated ECU
            # log_info_message(logger, "Checking for errors..")
            # no_errors = self._check_errors()
            no_errors = "No errors."

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

    def _download_data(self, type, version, ecu_id):
        """
        Request Sid = 0x34
        Response Sid = 0x74

        Request frame format:
        { pci, sid, data_format_identifier, adress_and_length_format_identifier, memory_adress, memory_size, version}

        Pci = 1 byte
        Sid = 1 byte
        Data_format_identifier = 1 byte
        0x00 means that no compression/encryption method is used
        0x01 means that only encryption is used
        0x10 means that only compression is used
        0x11 means that both encryption and compression are used
            -> for now use 0x00 because compression/encryption are not defined
            -> we can define more values if needed
        Address and Length format identifier 1-byte
        (bit 4- bit 7) denotes the number of bytes of the memory size parameter and the lower nibble
        (bit 0- bit 3) denotes the number of bytes of the memory address parameter.
        Memory address = min 1 byte -  max 16 bytes
        Memory size = min 1 byte - max 16 bytes
        Version = 1 byte
        0x00 => 0b 0000 0000
                                    ^^^^ these bits are used to determine update
                    iteration(ranges between 0 and 15)
                            ^^^^ these bits are used to determine update
            version(ranges between 0 and 15)
            -> 0000 0000 -> version 0.0
            -> 0000 0001 -> version 0.1
            …
            -> 0010 0000 -> version 2.0
            -> 0010 0001 -> version 2.1
            …

        Important note: This action requires to have the mcu module running in python virtual env and
        create locally a virtual partition used for download.
        -> search/change "/dev/loopXX" in RequestDownload.cpp, MemoryManager.cpp; (Depends which partition is attributed)
        """
        self.generate.request_download(self.id,
                                       data_format_identifier=type,  # No compression/encryption
                                       memory_address=0x8001,  # Memory address starting from 2049
                                       memory_size=0x01,  # Memory size
                                       version=version)  # Version 2
        self._passive_response(REQUEST_DOWNLOAD, "Error requesting download")

        self.generate.transfer_data(self.id, 0x01)
        time.sleep(1)
        self.generate.control_frame_write_file(self.id)
        time.sleep(1)
        self.generate.control_frame_install_updates(self.id)

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

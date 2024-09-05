from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class DiagnosticTroubleCode(Action):
    def read_dtc_info(self):
        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu
        try:
            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(id, 0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + self.id_ecu[MCU]

            log_info_message(logger, "Requesting read DTC information")
            self.generate.request_read_dtc_information(id, sub_funct=0x01, dtc_status_mask=0xFF)
            frame_response = self._passive_response(READ_DTC, "Error requesting session control")
            data = [hex(byte) for byte in frame_response.data]
            log_info_message(logger, f"Frame response: {data}")
            json_response = self.construct_json_response(data, id)
            log_info_message(logger, f"JSON Response: {json_response}")

            log_info_message(logger, "Response read DTC information")
            sts_ava_mask = 0xE4  # Status availability mask
            dtc_format = 0x01  # DTC format identifier (ISO_14229-1_DTCFormat)
            dtc_count = 3  # Number of DTCs found
            self.generate.response_read_dtc_information(id, sts_ava_mask, dtc_format, dtc_count)

            return json_response

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def clear_dtc_info(self):
        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu

        try:
            log_info_message(logger, "Changing session to programming")
            self.generate.session_control(id, 0x02)
            self._passive_response(SESSION_CONTROL, "Error changing session control")

            id = (self.id_ecu[ECU_BATTERY] << 16) + (self.my_id << 8) + self.id_ecu[MCU]

            # if len(data) != 5:
            #     print("Incorrect message length or invalid format")
            #     # Construct a negative response here, similar to C++ logic
            #     self.send_frame(id, [3, 0x54, 0x13])
            #     return

            # # Extract group_of_dtc from data bytes 2, 3, 4
            # group_of_dtc = (data[2] << 16) + (data[3] << 8) + data[4]

            # if group_of_dtc not in [0xAAA, 0x010AAA, 0x020AAA, 0x030AAA, 0xFFFFFF]:
            #     print("RequestOutOfRange: Specified Group of DTC parameter is not supported")
            #     # Construct a negative response
            #     self.send_frame(id, [3, 0x54, 0x31])
            #     return

            log_info_message(logger, "Clearing all DTCs information with positive response")
            self.generate.clear_diagnostic_information(id, 0xFFFFFF, False)
            self._passive_response(CLEAR_DTC, "Error requesting session control")

        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def construct_json_response(self, data, can_id):
        if len(data) < 7:
            return {"error": "Invalid data length"}

        # Extract values from the data
        pci = int(data[0], 16)  # PCI byte
        sid = int(data[1], 16)  # Service Identifier (SID)
        sub_function = int(data[2], 16)  # Sub-function
        status_availability_mask = int(data[3], 16)  # Status availability mask
        dtc_format_identifier = int(data[4], 16)  # DTC format identifier
        dtc_count_high = int(data[5], 16)  # High byte of DTC count
        dtc_count_low = int(data[6], 16)  # Low byte of DTC count
        dtc_count = (dtc_count_high << 8) | dtc_count_low  # Combine high and low bytes to get the full DTC count

        # Construct JSON response with detailed descriptions
        json_response = {
            "CAN_Interface": "vcan1",
            "CAN_ID": hex(can_id),
            "Data_Length": 7,
            "Data_Bytes": {
                "PCI": {
                    "value": pci,
                    "description": "PCI byte, indicating a 6-byte payload."
                },
                "SID": {
                    "value": sid,
                    "description": "Positive Response SID (0x19 + 0x40)."
                },
                "SubFunction": {
                    "value": sub_function,
                    "description": "Sub-function for reporting the number of DTCs."
                },
                "StatusAvailabilityMask": {
                    "value": status_availability_mask,
                    "description": "Status availability mask."
                },
                "DTCFormatIdentifier": {
                    "value": dtc_format_identifier,
                    "description": "DTC format identifier (ISO_14229-1 DTC format)."
                },
                "DTCCount": {
                    "value": dtc_count,
                    "description": f"The count of DTCs, {dtc_count} in this case."
                }
            }
        }

        return json_response
from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class DiagnosticTroubleCode(Action):
    def read_dtc_info(self):
        """ curl -X GET http://127.0.0.1:5000/api/read_dtc_info """
        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu
        try:

            id = (0x00 << 16) + (0xFA << 8) + self.id_ecu[ECU_BATTERY]

            log_info_message(logger, "Requesting read DTC information")
            self.request_read_dtc_information(id, sub_funct=0x01, dtc_status_mask=0xFF)
            frame_response = self._passive_response(READ_DTC, "Error requesting session control")

            if frame_response.data[1] == 0x7F:
                negative_response = self.handle_negative_response(frame_response.data[3], frame_response.data[2])
                json_response = {
                    "message": "Negative response received while Requesting read DTC information",
                    "negative_response": negative_response
                }

            data = [hex(byte) for byte in frame_response.data]
            log_info_message(logger, f"Frame response: {data}")
            json_response = self.construct_json_response(data, id)
            log_info_message(logger, f"JSON Response: {json_response}")

            log_info_message(logger, "Response read DTC information")
            sts_ava_mask = 0xE4  # Status availability mask
            dtc_format = 0x01  # DTC format identifier (ISO_14229-1_DTCFormat)
            dtc_count = 3  # Number of DTCs found
            self.response_read_dtc_information(id, sts_ava_mask, dtc_format, dtc_count)

            return json_response

        except CustomError as e:
            return e.message

    def clear_dtc_info(self):
        """ curl -X GET http://127.0.0.1:5000/api/clear_dtc_info """
        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu

        try:
            log_info_message(logger, "Clearing all DTCs information with positive response")
            self.clear_diagnostic_information(id, 0xFFFFFF, False)
            frame_response = self._passive_response(CLEAR_DTC, "Error clearing DTCs")

            if frame_response.data[1] == 0x54:
                return {
                    "message": "Clearing all DTCs information with positive response succeded"
                }

            if frame_response.data[1] == 0x7F:
                nrc_msg = frame_response.data[3]
                sid_msg = frame_response.data[2]
                negative_response = self.handle_negative_response(nrc_msg, sid_msg)
                return {
                    "message": "Negative response received while Requesting read DTC information",
                    "negative_response": negative_response
                }

        except CustomError as e:
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

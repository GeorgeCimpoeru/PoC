from actions.base_actions import *
from configs.data_identifiers import *
from flask import jsonify, make_response

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3

DTC_STATUS_BITS = {
    "testFailed" : 0,
    "testFailedThisOperationCycle" : 1,
    "pendingDTC" : 2,
    "confirmedDTC" : 3,
    "testNotCompletedSinceLastClear" : 4,
    "testFailedSinceLastClear" : 5,
    "testNotCompletedThisOperationCycle" : 6,
    "warningIndicatorRequested" : 7
}

DTC_FORMAT_IDENTIFIER = {
    0x00: "SAE_J2012-DA_DTCFormat_00",
    0x01: "ISO_14229-1_DTCFormat",
    0x02: "ISO_19399-73_DTCFormat",
    0x03: "ISO_11992-4_DTCFormat",
    0x04: "SAE_J2012-DA_DTCFormat_04"
}


class DiagnosticTroubleCode(Action):
    def __init__(self):
        super().__init__()

    def _get_dtc_mask_from_bits(self, bit_names):
        """
        Converts a list of bit names into a hexdecimal mask.
        """
        if not bit_names:
            return 0xFF

        mask = 0
        for bit_name in bit_names:
            if bit_name in DTC_STATUS_BITS:
                mask |= 1 << DTC_STATUS_BITS[bit_name]
            else:
                return make_response(jsonify(f"Invalid DTC status bit name: {bit_name}"), 400)
        return mask

    def read_dtc_info(self, subfunc, dtc_mask_bits, ecu_id):
        """ Byte 2 (Sub-function): Defines what kind of DTC report is requested.
                0x01 for the number of DTCs by status mask.
                0x02 for reporting DTCs by the status mask.

        curl -X GET 'http://127.0.0.1:5000/api/read_dtc_info?subfunc=1&dtc_mask_bits=testFailed&dtc_mask_bits=confirmedDTC&ecu_id=0x12'

        """
        try:

            id = (0x00 << 16) + (0xFA << 8) + ecu_id

            log_info_message(logger, "Requesting read DTC information")
            dtc_mask = self._get_dtc_mask_from_bits(dtc_mask_bits)
            if isinstance(dtc_mask, tuple):
                return dtc_mask
            self.request_read_dtc_information(id, subfunc, dtc_mask)
            frame_response = self._passive_response(READ_DTC,
                                                    "Error requesting session control")

            if frame_response.data[1] == 0x7F:
                negative_response = self.handle_negative_response(frame_response.data[3], frame_response.data[2])
                json_response = {
                    "message": "Negative response received while Requesting read DTC information",
                    "negative_response": negative_response
                }
                return jsonify(json_response), 400  # Return 400 Bad Request

            data = [hex(byte) for byte in frame_response.data]
            log_info_message(logger, f"Frame response: {data}")
            json_response = self.construct_json_response(data, id)
            log_info_message(logger, f"JSON Response: {json_response}")

            log_info_message(logger, "Response read DTC information")
            return jsonify(json_response)

        except CustomError as e:
            return make_response(jsonify({"error": e.message}), 500)  # Return 500 Internal Server Error

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
        if len(data) < 1:
            return {"error": "No data provided"}

        # Extract values from the data
        sid = int(data[1], 16) if len(data) > 1 else None  # Service Identifier (SID)
        sub_function = int(data[2], 16) if len(data) > 2 else None  # Sub-function
        status_availability_mask = int(data[3], 16) if len(data) > 3 else None  # Status availability mask
        dtc_format_identifier = int(data[4], 16) if len(data) > 4 else None  # DTC format identifier
        dtc_count_high = int(data[5], 16) if len(data) > 5 else 0  # High byte of DTC count
        dtc_count_low = int(data[6], 16) if len(data) > 6 else 0  # Low byte of DTC count
        dtc_count = (dtc_count_high << 8) | dtc_count_low  # Combine high and low bytes to get the full DTC count

        if sub_function == 1:
            sub_function_description = "reporNumberOfDTCByStatusMask"
        elif sub_function == 2:
            sub_function_description = "reportDTCsByStatusMask"
        else:
            sub_function_description = "Unknown sub-function"

        dtc_format_desc = DTC_FORMAT_IDENTIFIER.get(dtc_format_identifier, "Unknown DTC format identifier")
        json_response = {
            "CAN_ID": hex(can_id),
            "Data_Bytes": {
                "SID": {
                    "value": hex(sid),
                    "description": "Positive Response SID" if sid is not None else "Not provided"
                },
                "SubFunction": {
                    "value": f"0x0{sub_function}",
                    "description": sub_function_description if sub_function is not None else "Not provided"
                },
                "StatusAvailabilityMask": {
                    "value": hex(status_availability_mask),
                    "description": "Status availability mask." if status_availability_mask is not None else "Not provided"
                },
                "DTCFormatIdentifier": {
                    "value": hex(dtc_format_identifier),
                    "description": dtc_format_desc if dtc_format_identifier is not None else "Not provided"
                },
                "DTCCount": {
                    "value": dtc_count,
                    "description": f"The count of DTCs." if len(data) >= 7 else "Not provided"   # noqa: F541
                }
            }
        }

        return json_response

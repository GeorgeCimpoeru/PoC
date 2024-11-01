from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class Auth(Action):
    """ curl -X GET http://127.0.0.1:5000/api/authenticate """
    def _auth_to(self):
        try:
            id = (0x00 << 16) + (self.my_id << 8) + self.id_ecu[MCU]
            self.authentication_seed(id,
                                     sid_send=AUTHENTICATION_SEND,
                                     sid_recv=AUTHENTICATION_RECV,
                                     subf=AUTHENTICATION_SUBF_REQ_SEED)
            frame_response = self._passive_response(AUTHENTICATION_SEND,
                                                    "Error requesting seed")

            if frame_response.data[1] == 0x7F:
                nrc_msg = frame_response.data[3]
                sid_msg = frame_response.data[2]
                negative_response = self.handle_negative_response(nrc_msg, sid_msg)
                return {
                    "message": "Negative response received while requesting seed",
                    "negative_response": negative_response
                }

            # Check if the initial response is successful
            # if frame_response.data[1] == 0x67 and frame_response.data[2] == 0x01 and frame_response.data[3] == 0x00:
            if frame_response.data[1] == 0x67 and frame_response.data[2] == 0x01 and len(frame_response.data) == len([0x02, 0x67, 0x01]):
                log_info_message(logger, "Authentication successful")
                return {
                    "message": "Authentication successful"
                }

            else:
                # Extract seed and compute key
                seed = self._data_from_frame(frame_response)
                key = self._algorithm(seed)
                log_info_message(logger, f"Key: {key}")

                # Send the key for authentication
                self.authentication_key(id,
                                        key=key,
                                        sid_send=AUTHENTICATION_RECV,
                                        sid_recv=AUTHENTICATION_SEND,
                                        subf=AUTHENTICATION_SUBF_SEND_KEY)
                frame_response = self._passive_response(AUTHENTICATION_SEND,
                                                        "Error sending key")

                if frame_response.data[1] == 0x7F:
                    nrc_msg = frame_response.data[3]
                    sid_msg = frame_response.data[2]
                    negative_response = self.handle_negative_response(nrc_msg, sid_msg)
                    return {
                        "message": "Negative response received while sending key",
                        "negative_response": negative_response
                    }

                if frame_response.data[1] == 0x67 and frame_response.data[2] == 0x02:
                    log_info_message(logger, "Authentication successful")
                    return {
                        "message": "Authentication successful",
                    }
                else:
                    log_warning_message(logger, "Authentication failed")
                    return {
                        "message": "Authentication failed",
                    }

        except CustomError:
            nrc_msg = self.last_msg.data[3] if self.last_msg and len(self.last_msg.data) > 3 else 0x00
            sid_msg = self.last_msg.data[2] if self.last_msg and len(self.last_msg.data) > 2 else 0x00
            negative_response = self.handle_negative_response(nrc_msg, sid_msg)
            return {
                "message": "Error during authentication",
                "negative_response": negative_response
            }

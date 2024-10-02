import os
import sys
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__),'..',))
sys.path.append(PROJECT_ROOT)
import unittest
from unittest.mock import patch
from unittest import TestCase, main
from flask import Flask
from flask import request, jsonify, Blueprint 

api_bp = Blueprint('api', __name__)
class TestAPIs(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        # Set up the Flask test client
        app = Flask(__name__)
        app.register_blueprint(api_bp)
        cls.client = app.test_client()
        
    #Test for request_ids
    def test_request_ids_success(self):
        with patch('actions.request_id_action.RequestIdAction') as mock_get_request_id:
            # Arrange: Set up the mock response for read_ids()
            mock_get_request_id.read_ids.return_value = {
                "ecu_ids": ["11", "00", "00", "00"],
                "mcu_id": "10",
                "status": "Success",
                "time_stamp": "2024-08-02T10:12:26.308454"
            }

            # Act: Simulate a GET request to the /request_ids API endpoint
            response = self.client.get('/request_ids')

            # Assert: Verify the response status and data
            self.assertEqual(response.status_code, 200, "The HTTP response is not the expected one")
            self.assertEqual(response.json, {
                "ecu_ids": ["11", "00", "00", "00"],
                "mcu_id": "10",
                "status": "Success",
                "time_stamp": "2024-08-02T10:12:26.308454"
            }, "The obtained data are not correct")

    '''
        Tests for covering some specific scenarios for
        /send_frame endpoint route:
        1. Send frame successfully
        2. Missing CanId
        3. Invalid CanData
    '''
    def test_send_frame_success(self):
        # Arrange: Define the payload for the POST request
        payload = {
            "can_id": "0xFA10",
            "can_data": "0x04,0x27,0x02,0x0E,0x37"
        }

        with patch('actions.manual_send_frame.manual_send_frame') as mock_manual_send_frame:

            # Arrange: Define the mock return value for manual_send_frame
            mock_manual_send_frame.return_value = {
                "can_id": "0xFA10",
                "can_data": ["0x04", "0x67", "0x01", "0x00", "0x00"],
                "auth_status": "success",
                "error_text": None,
                "retry_timeout_ms": 1000
            }

            # Act: Send a POST request to the /send_frame API
            response = self.client.post('/send_frame', json=payload)

            # Assert: Verify the response status and data
            self.assertEqual(response.status_code, 200)
            self.assertEqual(response.json, {
                "can_id": "0xFA10",
                "can_data": ["0x04", "0x67", "0x01", "0x00", "0x00"],
                "auth_status": "success",
                "error_text": None,
                "retry_timeout_ms": 5000
            })

    def test_send_frame_missing_can_id(self):
        # Arrange: Payload missing the 'can_id'
        payload = {
            "can_data": "0x04,0x27,0x02,0x0E,0x37"
        }

        # Act: Send a POST request to the /send_frame API
        response = self.client.post('/send_frame', json=payload)

        # Assert: Verify the response status and error message
        self.assertEqual(response.status_code, 404,"The HTTP response is not the expected one")

    def test_send_frame_invalid_data_format(self):
        # Arrange: Payload with invalid can_data format (should be list or string)
        payload = {
            "can_id": "0xFA10",
            "can_data": None  # Invalid data format
        }

        # Act: Send a POST request to the /send_frame API
        response = self.client.post('/send_frame', json=payload)

        # Assert: Verify the response status and error message
        self.assertEqual(response.status_code, 404,"The HTTP response is not the expected one")

    # Test case for GET API for read battery module endpoint
    def test_read_info_battery(self):

        with patch('actions.read_info_action.ReadInfo') as mock_get_read_info_battery:

            #mock_instance = 
            # Act: Simulate a GET request to the /request_ids API endpoint
            
            mock_get_read_info_battery.return_value={
            "battery_level": "75",
            "voltage": "12.34V",
            "battery_state_of_charge": "Charging",
            "percentage": "80.00%",
            "life_cycle": "NA",
            "fully_charged": "NA",
            "serial_number": "NA",
            "range_battery": "NA",
            "charging_time": "NA",
            "device_consumption": "NA",
            "time_stamp": "2024-07-26T12:34:56.789Z"
            }

            response = self.requests.get('/read_info_battery')

            # Assert: Verify the response status and data
            self.assertEqual(response.status_code, 200,"The HTTP response is not the expected one")
            self.assertEqual(response.json, {
            "battery_level": "75",
            "voltage": "12.34V",
            "battery_state_of_charge": "Charging",
            "percentage": "80.00%",
            "life_cycle": "NA",
            "fully_charged": "NA",
            "serial_number": "NA",
            "range_battery": "NA",
            "charging_time": "NA",
            "device_consumption": "NA",
            "time_stamp": "2024-07-26T12:34:56.789Z"
            }, "The obtained data are not correct")

    ''' Test case scenarios for /authenticate
        API Endponts:
        1.Authentification succesfull
        2.Authentification failed
        3.Unexpected error '''
    
    # Test for successful authentication
    def test_authenticate_success(self):
        with patch('actions.secure_auth.Auth') as mock_auth_class:
            # Arrange: Mock the Auth class and its _auth_to method
            mock_instance = mock_auth_class.return_value
            mock_instance._auth_to.return_value = {"status": "Success"}

            # Act: Simulate a GET request to /authenticate
            response = self.client.get('/authenticate')

            # Assert: Verify the response status and data
            self.assertEqual(response.status_code, 200)
            self.assertEqual(response.json, {"status": "Success"})

    # Test for handling CustomError
    def test_authenticate_custom_error(self):
        with patch('actions.secure_auth.Auth') as mock_auth_class:
            # Arrange: Set up the mock to raise a CustomError
            mock_instance = mock_auth_class.return_value
            mock_instance._auth_to.return_value = {"status": "Authentication failed"}

            # Act: Simulate a GET request to /authenticate
            response = self.client.get('/authenticate')

            # Assert: Verify the response status and error message
            self.assertEqual(response.status_code, 400)
            self.assertEqual(response.json, "Authentication failed")

    # Test for handling general exceptions
    def test_authenticate_general_exception(self):
        with patch('actions.secure_auth.Auth') as mock_auth_class:
            # Arrange: Set up the mock to raise a general exception
            mock_instance = mock_auth_class.return_value
            mock_instance._auth_to.side_effect = Exception("Some unexpected error")

            # Act: Simulate a GET request to /authenticate
            response = self.client.get('/authenticate')

            # Assert: Verify the response status and error message
            self.assertEqual(response.status_code, 500)
            self.assertEqual(response.json, {"error": "Some unexpected error"})

    #Test for clear DTC info
    def test_clear_dtc_info(self):
        with patch('actions.dtc_info.DiagnosticTroubleCode') as mock_clear_dtc_info:
            mock_instance = mock_clear_dtc_info.return_value
            mock_instance.read_dtc_info.return_value={
            "CAN_ID": "0x11fa10",
            "CAN_Interface": "vcan1",
            "Data_Bytes": {
                "DTCCount": {
                "description": "The count of DTCs, 5 in this case.",
                "value": 5
                },
                "DTCFormatIdentifier": {
                "description": "DTC format identifier (ISO_14229-1 DTC format).",
                "value": 1
                },
                "PCI": {
                "description": "PCI byte, indicating a 6-byte payload.",
                "value": 6
                },
                "SID": {
                "description": "Positive Response SID (0x19 + 0x40).",
                "value": 89
                },
                "StatusAvailabilityMask": {
                "description": "Status availability mask.",
                "value": 63
                },
                "SubFunction": {
                "description": "Sub-function for reporting the number of DTCs.",
                "value": 1
                }
            },
            "Data_Length": 7
            }
            response = self.client.get('/read_dtc_info')
            # Assert: Verify the response status and data
            self.assertEqual(response.status_code, 200,"The HTTP response is not the expected one")
            self.assertEqual(response.json,{
            "CAN_ID": "0x11fa10",
            "CAN_Interface": "vcan1",
            "Data_Bytes": {
                "DTCCount": {
                "description": "The count of DTCs, 5 in this case.",
                "value": 5
                },
                "DTCFormatIdentifier": {
                "description": "DTC format identifier (ISO_14229-1 DTC format).",
                "value": 1
                },
                "PCI": {
                "description": "PCI byte, indicating a 6-byte payload.",
                "value": 6
                },
                "SID": {
                "description": "Positive Response SID (0x19 + 0x40).",
                "value": 89
                },
                "StatusAvailabilityMask": {
                "description": "Status availability mask.",
                "value": 63
                },
                "SubFunction": {
                "description": "Sub-function for reporting the number of DTCs.",
                "value": 1
                }
            },
            "Data_Length": 7
            },"The data are not correct")

if __name__ == '__main__':
    unittest.main()
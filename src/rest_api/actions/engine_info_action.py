# engine_info_action.py
"""
Author: Mujdei Ruben

The ReadInfo class is designed to fetch engine information from the ECU or provide dummy values for testing purposes. 
It inherits from the Action class, utilizing its methods to interact with the CAN bus and extract relevant data.
"""

from actions.base_actions import *
import json

class ReadInfo(Action):
    def __init__(self, my_id, id_ecu=[]):
        super().__init__(my_id, id_ecu)
    
    def read_dummy(self):
        """
        Returns dummy values for testing purposes.
        
        Returns:
        - A JSON string containing dummy values for various engine parameters.
        """
        dummy_data = {
            "power_output": 150,               
            "weight": 1200,                    
            "fuel_consumption": 8.5,           
            "torque": 320,                     
            "fuel_used": 50,                   
            "state_of_running": "normal",
            "current_speed": 80,               
            "engine_state": "running",
            "serial_number": "ABC123456789"
        }
        return dummy_data
    
    def read_from_ECU(self):
        """
        Requests real values from the ECU and returns them in a JSON format.
        
        Returns:
        - A JSON string containing real values for various engine parameters fetched from the ECU.
        """
        try:
            # Authenticate with ECU
            for id in self.id_ecu:
                self._authentication(id)

            # Request engine RPM
            log_info_message(logger, "Changing session to default") #Todo 
            engine_rpm = self._read_by_identifier(self.my_id, IDENTIFIER_ENGINE_RPM)
            engine_rpm_value = int(engine_rpm, 16) / 4  # Convert from hex and apply scaling factor

            # Request engine coolant temperature
            engine_coolant_temp = self._read_by_identifier(self.my_id, IDENTIFIER_ENGINE_COOLANT_TEMPERATURE)
            engine_coolant_temp_value = int(engine_coolant_temp, 16) - 40  # Convert from hex and apply offset

            # Generate JSON response
            response_json ={
                    "power_output": engine_rpm_value / 100, 
                    "weight": 1200,  
                    "fuel_consumption": 8.5, 
                    "torque": engine_rpm_value / 10, 
                    "fuel_used": 50, 
                    "state_of_running": "normal",
                    "current_speed": 80,
                    "engine_state": "running",
                    "serial_number": "ABC123456789"
                }

            self.bus.shutdown()
            return response_json

        except CustomError as e:
            self.bus.shutdown()
            return e.message


from actions.base_actions import *
from configs.data_identifiers import *

MCU = 0
ECU_BATTERY = 1
ECU_ENGINE = 2
ECU_DOORS = 3


class WriteInfo(Action):
    """
    Base class for writing data to various ECUs.
    """
    def __init__(self, my_id, id_ecu_list, data):
        super().__init__(my_id, id_ecu_list)
        self.data = data

    def _auth_mcu(self):
        id_mcu = self.id_ecu[MCU]
        id = self.my_id * 0x100 + id_mcu

        try:
            log_info_message(logger, "Changing session to default")
            self._authentication(id)
        except CustomError as e:
            self.bus.shutdown()
            return e.message

    def _write_by_identifier(self, id, identifier, value):
        log_info_message(logger, f"Write by identifier {identifier}")
        value_list = self._number_to_byte_list(value)

        if isinstance(value_list, list) and len(value_list) > 4:
            self.generate.write_data_by_identifier_long(id, identifier, value_list)
        else:
            self.generate.write_data_by_identifier(id, identifier, value_list)
            self._passive_response(WRITE_BY_IDENTIFIER, f"Error writing {identifier}")

        return True

    def _write_data(self, id_ECU):
        """
        Write data to the specified ECU ID.

        Args:
        - id_ECU: The ID of the ECU to write to.
        """
        id = self.my_id * 0x100 + id_ECU
        log_info_message(logger, f"Writing data to ECU ID: {id_ECU}")

        # Get specific data parameters from subclass method
        data_params = self._prepare_data_for_write()

        for identifier, data_param in data_params:
            if data_param is not None:
                self._write_by_identifier(id, identifier, data_param)

        log_info_message(logger, f"Data written successfully to ECU ID: {id_ECU}")

    def _prepare_data_for_write(self):
        """
        Abstract method to be implemented by subclasses.
        Provides specific data parameters for writing.
        """
        raise NotImplementedError("Subclasses must implement this method")

    def get_ecu_id(self):
        """
        Abstract method to be implemented by subclasses.
        Provides the specific ECU ID for writing.
        """
        raise NotImplementedError("Subclasses must implement this method")

    def run(self):
        try:
            self._auth_mcu()
            self._write_data(self.get_ecu_id())
            response_json = self._to_json("success", 0)
            return response_json
        except CustomError as e:
            self.bus.shutdown()
            return e.message


class WriteToDoors(WriteInfo):
    def get_ecu_id(self):
        return 0x12  # TBD by the back-end team

    def _prepare_data_for_write(self):
        return [
            (IDENTIFIER_DOOR, int(self.data.get('door'))),
            (IDENTIFIER_DOOR_SERIALNUMBER, int(self.data.get('serial_number'))),
            (IDENTIFIER_LIGHTER_VOLTAGE, int(self.data.get('lighter_voltage'))),
            (IDENTIFIER_LIGHT_STATE, int(self.data.get('light_state'))),
            (IDENTIFIER_BELT_STATE, int(self.data.get('belt'))),
            (IDENTIFIER_WINDOWS_CLOSED, int(self.data.get('windows_closed')))
        ]


class WriteToBattery(WriteInfo):
    def get_ecu_id(self):
        return 0x11

    def _prepare_data_for_write(self):
        return [
            (IDENTIFIER_BATTERY_ENERGY_LEVEL, int(self.data.get('battery_level'))),
            (IDENTIFIER_BATTERY_VOLTAGE, int(self.data.get('voltage'))),
            (IDENTIFIER_BATTERY_PERCENTAGE, int(self.data.get('percentage'))),
            (IDENTIFIER_BATTERY_STATE_OF_CHARGE, int(self.data.get('battery_state_of_charge'))),
            (IDENTIFIER_BATTERY_TEMPERATURE, int(self.data.get('temperature'))),
            (IDENTIFIER_BATTERY_LIFE_CYCLE, int(self.data.get('life_cycle'))),
            (IDENTIFIER_BATTERY_FULLY_CHARGED, int(self.data.get('fully_charged'))),
            (IDENTIFIER_BATTERY_RANGE, int(self.data.get('range_battery'))),
            (IDENTIFIER_BATTERY_CHARGING_TIME, int(self.data.get('charging_time'))),
            (IDENTIFIER_DEVICE_CONSUMPTION, int(self.data.get('device_consumption')))
        ]

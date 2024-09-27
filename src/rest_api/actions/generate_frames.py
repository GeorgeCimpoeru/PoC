import can
from utils.logger import SingletonLogger
from config import Config
import threading
import subprocess

can_lock = threading.Lock()

logger_singleton = SingletonLogger('logger.log')
logger = logger_singleton.logger
logger_frame = logger_singleton.logger_frame


@logger_frame
class GenerateFrame:
    def __init__(self, bus=None):
        if bus is None:
            if self.is_interface_up(Config.CAN_CHANNEL):
                self.bus = can.interface.Bus(channel=Config.CAN_CHANNEL, bustype='socketcan')
                logger.info(f"Connected to CAN interface {Config.CAN_CHANNEL}.")
            else:
                raise RuntimeError(f"CAN interface {Config.CAN_CHANNEL} is not up. Please bring it up before initializing.")
        else:
            self.bus = bus

    @staticmethod
    def is_interface_up(interface: str) -> bool:
        """
        Check if the given network interface is up.
        """
        try:
            # Run the 'ip link show' command to check the status of the interface
            result = subprocess.run(['ip', 'link', 'show', interface], capture_output=True, text=True)
            # Check if the result contains "state UP"
            return "state UP" in result.stdout
        except Exception as e:
            logger.info(f"Error checking interface status: {e}")
            return False

    def send_frame(self, id, data):
        with can_lock:
            message = can.Message(arbitration_id=id, data=data, is_extended_id=True)
            try:
                self.bus.send(message)
            except can.CanError as e:
                logger.error(f"Message not sent: {e}")

    def control_frame(self, id):
        data = [0x30, 0x00, 0x00, 0x00]
        self.send_frame(id, data)

    def control_frame_write_file(self, id):
        data = [0x03, 0x31, 0x01, 0x03, 0x01, 0x00]
        self.send_frame(id, data)

    def control_frame_install_updates(self, id):
        data = [0x03, 0x31, 0x01, 0x02, 0x01, 0x00]
        self.send_frame(id, data)

    def request_id_mcu(self, api_id):
        data = [1, 0x99]
        self.send_frame(api_id, data)

    def session_control(self, id, sub_funct, response=False):
        data = [2, 0x10, sub_funct] if response is False else [2, 0x50, sub_funct]
        self.send_frame(id, data)

    def ecu_reset(self, id, subfunct, response=False):
        data = [2, 0x11, subfunct] if response is False else [2, 0x51, subfunct]
        self.send_frame(id, data)

    def read_data_by_identifier(self, id, identifier, response=[]):
        if len(response) == 0:
            data = [3, 0x22, identifier // 0x100, identifier % 0x100]
        else:
            if len(response) <= 4:
                data = [len(response) + 3, 0x62, identifier // 0x100, identifier % 0x100] + response
            else:
                return
        self.send_frame(id, data)

    def read_data_by_identifier_long(self, id, identifier, response, first_frame=True):
        self.__generate_long_response(id, 0x62, identifier, response, first_frame)

    def request_read_dtc_information(self, id, sub_funct, dtc_status_mask):
        data = [3, 0x19, sub_funct, dtc_status_mask]
        self.send_frame(id, data)

    def response_read_dtc_information(self, id, sts_ava_mask, dtc_format, dtc_count):
        data = [5, 0x59, 0x01, sts_ava_mask, dtc_format, dtc_count]
        self.send_frame(id, data)

    def read_memory_by_adress(self, id, memory_address, memory_size, response=[]):
        address_length = (self.__count_digits(memory_address) + 1) // 2
        size_length = (self.__count_digits(memory_size) + 1) // 2
        address_size_length = address_length + size_length * 0x10
        pci_l = address_length + size_length + 2

        if len(response) == 0:
            data = [pci_l, 0x23, address_size_length]
            self.__add_to_list(data, memory_address)
            self.__add_to_list(data, memory_size)
        else:
            if len(response) + pci_l + 1 <= 8:
                data = [pci_l + len(response), 0x63, address_size_length]
                self.__add_to_list(data, memory_address)
                self.__add_to_list(data, memory_size)
                data = data + response
            else:
                logger.error("Error, please use considering read_memory_by_adress_long")
                return
        self.send_frame(id, data)

    def read_memory_by_adress_long(self, id, memory_address, memory_size, response=[], first_frame=True):
        address_length = (self.__count_digits(memory_address) + 1) // 2
        size_length = (self.__count_digits(memory_size) + 1) // 2

        address_size_length = address_length + size_length * 0x10
        pci_l = address_length + size_length + 2 + len(response)
        bytes_first_frame = 8 - address_length - size_length - 4
        if first_frame:
            data = [0x10, pci_l, 0x63, address_size_length]
            self.__add_to_list(data, memory_address)
            self.__add_to_list(data, memory_size)
            data = data + response[:bytes_first_frame]
        else:
            last_data = response[bytes_first_frame:]
            for i in range(0, len(last_data) // 7):
                data = [0x21 + i] + last_data[i * 7: (i+1) * 7]
                self.send_frame(id, data)
            # Send remaining data
            if len(last_data) % 7:
                data = [0x21 + len(last_data) // 7] + last_data[len(last_data) - len(last_data) % 7:]
        self.send_frame(id, data)

    def request_transfer_exit(self, id, response=False):
        data = [1, 0x77] if response is False else [1, 0x37]
        self.send_frame(id, data)

    def clear_diagnostic_information(self, id, group_of_dtc=0xFFFFFF, response=False):
        group_of_dtc_bytes = [(group_of_dtc >> 16) & 0xFF, (group_of_dtc >> 8) & 0xFF, group_of_dtc & 0xFF]
        pci_l = len(group_of_dtc_bytes) + 1
        data = [pci_l, 0x14] + group_of_dtc_bytes if not response else [1, 0x54]
        self.send_frame(id, data)

    def negative_response(self, id, sid, nrc):
        data = [3, 0x7F, sid, nrc]
        self.send_frame(id, data)

    def routine_control(self, id, sub_funct, routine_id, response=False):
        data = [4, 0x71, sub_funct, routine_id // 0x100, routine_id % 0x100] \
            if response is False else [4, 0x31, sub_funct, routine_id // 0x100, routine_id % 0x100]
        self.send_frame(id, data)

    def authentication_seed(self, id, sid_send, sid_recv, subf, seed=[]):
        length_seed = len(seed)
        data = [length_seed + 2, sid_recv, subf] + seed if length_seed > 0 else [2, sid_send, subf]
        self.send_frame(id, data)

    def authentication_key(self, id, sid_send, sid_recv, subf, key=[]):
        length_key = len(key)
        data = [length_key + 2, sid_recv, subf] + key if length_key > 0 else [2, sid_send, subf]
        self.send_frame(id, data)

    def tester_present(self, id, response=False):
        data = [2, 0x3E, 0] if response is False else [2, 0x7E, 0]
        self.send_frame(id, data)

    def access_timing_parameters(self, id, sub_function, response=False):
        data = [2, 0x83, sub_function] if response is False else [2, 0xC3, sub_function]
        self.send_frame(id, data)

    def write_timming_parameters(self, id, sub_function, new_p2_max_time, new_p2_star_max_time, response=False):
        new_p2_max_time_bytes = [(new_p2_max_time >> 8) & 0xFF, new_p2_max_time & 0xFF]
        new_p2_star_max_time_bytes = [(new_p2_star_max_time >> 8) & 0xFF, new_p2_star_max_time & 0xFF]

        if not response:
            data = [6, 0x83, sub_function] + new_p2_max_time_bytes + new_p2_star_max_time_bytes
        else:
            data = [6, 0xC3, sub_function] + new_p2_max_time_bytes + new_p2_star_max_time_bytes

        self.send_frame(id, data)

    def request_download(self, id, data_format_identifier, memory_address, memory_size, version):
        # Define the data format identifier mapping
        DATA_FORMAT_IDENTIFIER_MAP = {
            0x00: "No compression/encryption",
            "zip": 0x10,
            0x01: "Only encryption",
            0x10: "Only compression",
            0x11: "Both encryption and compression"
        }

        # Constants
        sid = 0x34  # Service Identifier for Request Download
        pci = 0x07  # Protocol Control Information (PCI) - 1 byte

        # Handle data_format_identifier
        if isinstance(data_format_identifier, str):
            if data_format_identifier.lower() not in DATA_FORMAT_IDENTIFIER_MAP:
                raise ValueError(f"Invalid data format identifier: {data_format_identifier}")
            data_format_identifier = DATA_FORMAT_IDENTIFIER_MAP[data_format_identifier.lower()]
        elif isinstance(data_format_identifier, int):
            if data_format_identifier not in DATA_FORMAT_IDENTIFIER_MAP:
                raise ValueError(f"Invalid data format identifier: {data_format_identifier}")
        else:
            raise ValueError(f"Invalid data format identifier type: {type(data_format_identifier)}")

        # Handle memory address and size
        memory_address_bytes = memory_address.to_bytes((memory_address.bit_length() + 7) // 8, byteorder='big') if isinstance(memory_address, int) else memory_address
        memory_size_bytes = memory_size.to_bytes((memory_size.bit_length() + 7) // 8, byteorder='big') if isinstance(memory_size, int) else memory_size

        # Calculate the Address and Length Format Identifier
        address_length = len(memory_address_bytes)  # noqa: F841
        size_length = len(memory_size_bytes)  # noqa: F841
        # address_and_length_format_identifier = (size_length << 4) | address_length
        address_and_length_format_identifier = 0x21

        # Handle version
        if isinstance(version, str):
            if '.' not in version:
                version += '.0'
            major, minor = map(int, version.split('.'))
            major -= 1
            if major < 0 or major > 15 or minor < 0 or minor > 15:
                raise ValueError(f"Invalid version: {version}. Major and minor must be between 0 and 15.")
            version_byte = (major << 4) | minor  # Encode directly without reduction
        elif isinstance(version, int):
            # Assume the int is already in the correct format
            version_byte = version
        else:
            raise ValueError(f"Invalid version format: {version}")

        # Constructing the data list
        data = [pci, sid, data_format_identifier, address_and_length_format_identifier]
        data.extend(memory_address_bytes)
        data.extend(memory_size_bytes)
        data.append(version_byte)

        # Sending the frame using the send_frame function
        self.send_frame(id, data)

    def request_download_response(self, id, max_number_block):
        mnb_length = (self.__count_digits(max_number_block) + 1) // 2
        data = [mnb_length + 2, 0x74, mnb_length * 0x10]
        self.__add_to_list(data, max_number_block)

        self.send_frame(id, data)

    def transfer_data(self, id, block_sequence_counter):
        data = [0x02, 0x36, block_sequence_counter]
        self.send_frame(id, data)

    def transfer_data_long(self, id, block_sequence_counter, transfer_data, first_frame=True):
        if first_frame:
            data = [0x10, len(transfer_data) + 2, 0x36, block_sequence_counter] + transfer_data[:4]
            self.send_frame(id, data)
        else:
            # Delete first 3 byts of data
            last_data = transfer_data[4:]
            for i in range(0, len(last_data) // 7):
                data = [0x21 + i] + last_data[i * 7: (i + 1) * 7]
                self.send_frame(id, data)
            # Send remaining data
            if len(last_data) % 7:
                data = [0x21 + len(last_data)//7] + last_data[len(last_data) - len(last_data) % 7:]
                self.send_frame(id, data)

    def write_data_by_identifier(self, id, identifier, data_parameter):
        if len(data_parameter) > 0:
            if len(data_parameter) <= 4:
                data = [len(data_parameter) + 3 , 0x2E, identifier // 0x100, identifier % 0x100] + data_parameter
            else:
                # "ERROR: To many data parameters, consider using write_data_by_identifier_long!"
                return
        else:
            data = [3, 0x6E, identifier // 0x100, identifier % 0x100]
        self.send_frame(id, data)

    def write_data_by_identifier_long(self, id, identifier, data, first_frame=True):
        self.__generate_long_response(id, 0x2E, identifier, data, first_frame)

    def __generate_long_response(self, id, sid, identifier, response , first_frame):
        if first_frame:
            data = [0x10, len(response) + 3, sid, identifier // 0x100, identifier % 0x100] + response[:3]
            self.send_frame(id, data)
        else:
            last_data = response[3:]
            for i in range(0, len(last_data)//7):
                data = [(0x21 + i % 8)] + last_data[i * 7 : (i+1) * 7]
                self.send_frame(id, data)
            if len(last_data) % 7:
                data = [0x21 + len(last_data) // 7] + last_data[len(last_data) - len(last_data) % 7:]
                self.send_frame(id, data)

    def __add_to_list(self, data_list, number):
        temp_list = [(number >> (8 * i)) & 0xFF for i in range((number.bit_length() + 7) // 8)]
        data_list.extend(temp_list[::-1])

    def __count_digits(self, number):
        digits = 0
        while number:
            digits += 1
            number //= 10
        return digits

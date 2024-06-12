import can 

class GenerateFrame:
    def __init__(self, can_interface):
        self.bus = can.interface.Bus(channel=can_interface, bustype='socketcan')

    
    def send_frame(self, id, data):
        message = can.Message(arbitration_id=id, data=data)
        try: 
            self.bus.send(message)
        except can.CanError:
            print("Message not sent")

    def session_control(self, id, sub_funct, response = False):
        
        if response == False:
            data = [2, 0x10, sub_funct]
        else:
            data = [2, 0x50, sub_funct]

        self.send_frame(id, data) 
    
    def ecu_reset(self, id, response = False):

        if response == False:
            data = [2, 0x11, 0x03]
        else:
            data = [2, 0x51, 0x03]

        self.send_frame(id, data)

    def read_data_by_identifier(self, id, identifier, response = []):

        if len(response) == 0:
            data = [3, 0x22, identifier//0x100, identifier%0x100]
        else:
            data = [len(response) + 3, 0x62, identifier//0x100, identifier%0x100] + response

        self.send_frame(id, data)


    def request_read_dtc_information(self, id, sub_funct, dtc_status_mask):
        
        sub_funct = 0x01
       
        data = [3, 0x19, sub_funct, dtc_status_mask]

        self.send_frame(id, data)


    def response_read_dtc_information(self, id, sts_ava_mask, dtc_format, dtc_count):

        data = [3, 0x19, 0x01, sts_ava_mask, dtc_format, dtc_count]

        self.send_frame(id, data)

    def read_memory_by_adress(self, id, memory_address, memory_size, response = []):
        #to be implemented
        address_length = (self.__count_digits(memory_address)+1)//2
        size_length = (self.__count_digits(memory_size)+1)//2

        address_size_length = address_length + size_length*0x10
        pci_l = address_length + size_length + 2
        
        if len(response) == 0:

            data =[ pci_l, 0x23, address_size_length, memory_address, memory_size]
        else:
            data =[ pci_l + len(response), 0x63, address_size_length, memory_address, memory_size] + response

        self.send_frame(id, data)

    #ruben
    def routine_control(self, id, sub_funct, routine_id, response = False):
        if response:
            data = [4, 0x71, sub_funct, routine_id / 0x100, routine_id % 0x100]
        else:
            data = [4, 0x31, sub_funct, routine_id / 0x100, routine_id % 0x100]

        self.send_frame(id, data)
        
    def authntication_seed(self, id, seed = []):
        length_seed = len(seed)
        if length_seed > 0:
            data = [length_seed + 2, 0x69, 0x1] + seed
        else:
            data = [2, 0x29, 0x1]
            

        self.send_frame(id, data)

    def authntication_key(self, id, key = []):
        length_key = len(key)
        if length_key > 0:
            data = [length_key + 2, 0x29, 0x2] + key
        else:
            data = [2, 0x69, 0x2]

        self.send_frame(id, data)
    
    def tester_present(self, id, response = False):
        if response:
            data = [2, 0x7E, 0]
        else:
            data = [2, 0x3E, 0]

        self.send_frame(id, data)
    
    def access_timing_parameters(self, id, sub_function, response = False):
        if response:
            data = [2, 0x83, sub_function]
        else:
            data = [2, 0xC3, sub_function]

        self.send_frame(id, data)

    def request_download(self, id, data_format_identifier, memory_address, memory_size):

        memory_length = self.__count_digits(memory_size) * 0x100 + self.__count_digits(memory_address)
        data = [2,0x34, data_format_identifier, memory_length]

        self.send_frame(id, data)


    def __count_digits(self, number):
        digits = 0
        while number:
            digits += 1
            number //=10
        return digits


can_interface = "vcan0"
id = 0x1234
data = [0,1,2]

generateFrame = GenerateFrame(can_interface)

generateFrame.read_memory_by_adress(id, 0x12, 0x01, data)

generateFrame.bus.shutdown()


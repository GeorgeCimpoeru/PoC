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
            if len(response) <= 4:

                data = [len(response) + 3, 0x62, identifier//0x100, identifier%0x100] + response

            else:
                print("Error")
                return

        self.send_frame(id, data)

    def read_data_by_identifier_long(self, id, identifier, response, first_frame=True):
        self.__generate_long_response(id, 0x62, identifier, response, first_frame)

    def request_read_dtc_information(self, id, sub_funct, dtc_status_mask):
        
        sub_funct = 0x01
       
        data = [3, 0x19, sub_funct, dtc_status_mask]

        self.send_frame(id, data)


    def response_read_dtc_information(self, id, sts_ava_mask, dtc_format, dtc_count):

        data = [3, 0x19, 0x01, sts_ava_mask, dtc_format, dtc_count]

        self.send_frame(id, data)

    def read_memory_by_adress(self, id, memory_address, memory_size, response = []):
        address_length = (self.__count_digits(memory_address)+1)//2
        size_length = (self.__count_digits(memory_size)+1)//2

        address_size_length = address_length + size_length*0x10
        pci_l = address_length + size_length + 2


        
        if len(response) == 0:

            data =[ pci_l, 0x23, address_size_length]
            self.__add_to_list(data, memory_address)
            self.__add_to_list(data, memory_size)
        else:
            data =[ pci_l + len(response), 0x63, address_size_length]
            self.__add_to_list(data, memory_address)
            self.__add_to_list(data, memory_size)
            data = data + response

        self.send_frame(id, data)

    def request_transfer_exit(self, id, response = False):
        if response:
            data = [2, 0x37, 0]
        else:
             data = [2, 0x77, 0]

        self.send_frame(id, data)

    def clear_diagnostic_information(self, id, group_of_dtc, response=False):
        pci_l = len(group_of_dtc) + 1
        if response:
            data = [pci_l, 0x14] + group_of_dtc
        else:
            data = [1, 0x54]
        
        self.send_frame(id, data)

    def negative_response(self, id, sid, nrc): 
        data = [3, 0x7F, sid, nrc]
      
    
        self.send_frame(id, data)

    #ruben
    def routine_control(self, id, sub_funct, routine_id, response = False):
        if response:
            data = [4, 0x71, sub_funct, routine_id // 0x100, routine_id % 0x100]
        else:
            data = [4, 0x31, sub_funct, routine_id // 0x100, routine_id % 0x100]

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
        address_length = (self.__count_digits(memory_address)+1)//2
        size_length = (self.__count_digits(memory_size)+1)//2
        memory_length = size_length * 0x10 + address_length
        data = [size_length+ address_length + 3,0x34, data_format_identifier, memory_length]
        self.__add_to_list(data, memory_address)
        self.__add_to_list(data, memory_size)

        self.send_frame(id, data)

    def request_download_response(self, id, max_number_block):
        mnb_length = (self.__count_digits(max_number_block)+1)//2
        data = [mnb_length + 2, 0x74, mnb_length * 0x10]
        self.__add_to_list(data, max_number_block)

        self.send_frame(id, data)

    def transfer_data(self, id, block_sequence_counter, transfer_data = {}):
        if len(transfer_data):
            if len(transfer_data) <=5:
                data = [len(transfer_data) + 2, 0x36, block_sequence_counter] + transfer_data
            else:
                print("ERROR: To many data to transfer, consider using Transfer_data_long!")
                return
        else:
            data =[0x2, 0x76, block_sequence_counter]
        self.send_frame(id, data)

    def transfer_data_long (self, id, block_sequence_counter, transfer_data, first_frame = True):
        if first_frame:
            data = [0x10,len(transfer_data) + 2,0x36,block_sequence_counter] + transfer_data[:4]
            self.send_frame(id, data)
        else:
            #Delete first 3 byts of data
            last_data = transfer_data[4:]
            for i in range(0, len(last_data)//7):
                data = [0x21 + i ] + last_data[i*7:(i+1)*7]
                self.send_frame(id, data)
            #Send remaining data
            if len(last_data)%7:
                data = [0x21 + len(last_data)//7] + last_data[len(last_data)- len(last_data)%7:]
                self.send_frame(id, data)

    def write_data_by_identifier(self, id, identifier, data_parameter):
        if len(data_parameter) > 0:
            if len(data_parameter) <=4:
                data =[len(data_parameter) +3 , 0x2E, identifier // 0x100, identifier%0x100] + data_parameter
            else:
                print("ERROR: To many data parameters, consider using write_data_by_identifier_long!")
                return
        else:
            data =[3, 0x6E, identifier // 0x100, identifier%0x100]
        
        self.send_frame(id,data)
            
    def write_data_by_identifier_long(self, id, identifier, data, first_frame=True):
        self.__generate_long_response(id, 0x2E, identifier, data, first_frame)


    def __generate_long_response(self, id, sid, identifier, response , first_frame):
        if first_frame:
            data = [0x10,len(response) + 3, sid, identifier//0x100, identifier%0x100] + response[:3]
            self.send_frame(id, data)
        else:
            last_data = response[3:]
            for i in range(0, len(last_data)//7):
                data = [(0x21 + i%8)] + last_data[i*7:(i+1)*7]
                self.send_frame(id, data)
            if len(last_data)%7:
                data = [0x21 + len(last_data)//7] + last_data[len(last_data)- len(last_data)%7:]
                self.send_frame(id, data) 

    def __add_to_list(self, data_list, number):
        temp_list = []
        while number:
            temp_list.append(number%0x100)
            number //=0x100
        data_list.extend(temp_list[::-1])


    def __count_digits(self, number):
        digits = 0
        while number:
            digits += 1
            number //=10
        return digits


can_interface = "vcan0"
id = 0x1234
data = [0,1,2,3,4,5]

generateFrame = GenerateFrame(can_interface)

generateFrame.read_data_by_identifier(id, 0x1234, data)
generateFrame.read_data_by_identifier_long(id, 0x1234, data)
generateFrame.read_data_by_identifier_long(id, 0x1234, data, False)

generateFrame.bus.shutdown()


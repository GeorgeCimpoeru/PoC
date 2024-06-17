#include "../include/HandleFrames.h"
/* Services to be included here */

bool HandleFrames::checkReceivedFrame(int nbytes, const struct can_frame &frame)
{
    if (nbytes < 0) 
    {   
        /* No data available or error occurred */ 
        if (errno == EWOULDBLOCK || errno == EAGAIN) 
        {
            /* No data available, wait for a short duration */ 
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            return false;
        } 
        else 
        {
            /* Other error occurred, handle it */
            std::cerr << "Read error: " << strerror(errno) << std::endl;
            return false;
        }
    } 
    else if (nbytes < sizeof(struct can_frame)) 
    {
        std::cerr << "Incomplete frame read\n";
        return false;
    } 
    else if (nbytes == 0) 
    {
        std::cerr << "Connection closed by peer" << std::endl;
        return false;
    }

    /* Check if the received frame is empty */
    if (frame.can_dlc == 0) 
    {
        std::cerr << "Received empty frame\n";
        return false;
    }

    processReceivedFrame(frame);
    return true;
}

void HandleFrames::processReceivedFrame(const struct can_frame &frame) 
{
    int id = frame.can_id;
    std::vector<uint8_t> data(frame.data, frame.data + frame.can_dlc);

    /* Extract the PCI byte */ 
    pci = data[0];
    uint8_t pci_type = pci & 0xF0;

    /* Extract the Service Identifier (SID) */ 
    sid = -1;
    sid_position = -1;

    /* Handle First Frame or Single Frame */ 
    if (flag == 0) 
    {
        /* First frame */ 
        if (pci_type == 0x10)  
        {
            std::cout << "\nFirst frame received\n";
            
            /* Expected total size of data */ 
            expected_data_size = data[1]; 
            std::cout << "data size " << expected_data_size << std::endl;
            std::cout << "Data size before inserting: ";
            for (int value : data) 
            {
                std::cout << std::hex << value << " ";
            }
            std::cout << std::endl;
            /* Calculate the number of consecutive frames expected */ 
            flag = expected_data_size / 7; 
            if (expected_data_size % 7 > 0)
            {
                flag++;
            }
            std::cout << "no of frames " << flag << std::endl;

            stored_data.clear();
            stored_sid = data[2];
            /* Concatenate data excluding PCI byte */ 
            stored_data.insert(stored_data.end(), data.begin() + 5, data.end()); 
            std::cout << "Data size after inserting: ";
            for (int value : stored_data) 
            {
                std::cout << std::hex << value << " ";
            }
            std::cout << std::endl;
            first_frame = true;
            flag--;
            std::cout << "no of frames " << static_cast<int>(flag) << std::endl;
            std::cout << "SID: 0x" << std::hex << sid << " found at position: " << sid_position << std::dec << "\n"<<std::flush;
        } 
        /* Handle Single Frames */ 
        else 
        {
            std::cout << "\nSingle frame received\n";
            if (data[1] == 0x7F)
            {
                sid = data[2];
                sid_position = 2;
            }
            else
            {
                sid = data[1]; 
                sid_position = 1; 
            }
            /* Store the data including the PCI byte */ 
            stored_data = data; 
            /* Process the complete data */ 
            handleCompleteData(id, stored_data, true); 
            stored_data.clear();
            expected_data_size = 0;
            std::cout << "SID: 0x" << std::hex << sid << " found at position: " << sid_position << std::dec << "\n"<<std::flush;
            return;
        }
    } 
    /* Handle Consecutive Frames */ 
    else 
    {   
        /* Consecutive frame */ 
        if ((pci_type & 0xF0) >= 0x20 && (pci_type & 0xF0) < 0x30 )  
        {
            if (!first_frame)
            {
                return;
            }
            std::cout << "\nConsecutive frame received\n";
            /* Print stored data before inserting the second frame */
            std::cout << "Data size before inserting: ";
            for (int value : data) 
            {
                std::cout << std::hex << value << " ";
            }
            std::cout << std::endl;
            /* Concatenate data excluding PCI byte */ 
            stored_data.insert(stored_data.end(), data.begin() + 1, data.end()); 
            /* Print stored data before inserting the second frame */ 
            std::cout << "Data size after inserting: ";
            for (int value : stored_data) 
            {
                std::cout << std::hex << value << " ";
            }
            std::cout << std::endl;
            flag--;
            std::cout << "no of frames " << static_cast<int>(flag) << std::endl;
        }
        else
        {
            std::cerr << "Unexpected frame format\n";
            return;
        }
    }

    /* If all frames have been received */ 
    if (flag == 0) 
    {
        std::cout << "All frames received\n";
        
        /* SID is at position 2 in the data vector after excluding PCI byte */ 
        sid = stored_sid;
        sid_position = 2;
    
        /* Print stored data before inserting the second frame */ 
        std::cout << "Data size before inserting: ";
        for (int value : stored_data) 
        {
            std::cout << std::hex << value << " ";
        }
        std::cout << std::endl;
        /* Process the complete data */ 
        handleCompleteData(id, stored_data, false);

        /* Print stored data before inserting the second frame */ 
        std::cout << "Data size before inserting: ";
        for (int value : stored_data) 
        {
            std::cout << std::hex << value << " ";
        }
        std::cout << std::endl;

        stored_data.clear();
        expected_data_size = 0;
    }
}

/* Method for handling complete data after reception */
void HandleFrames::handleCompleteData(int id,const std::vector<uint8_t>& stored_data,bool is_single_frame) 
{
    /* Extract the Service Identifier (SID) */ 
    std::cout << "Handling complete data for SID: 0x" << std::hex << sid << std::dec << "\n";

    switch (sid) 
    {
        /* UDS */
        case 0x10: 
            {
                /** SessionControl -request method --to be implemented 
                 *  Expected request: pci_l + sid + sub_funct
                 *  Index              [0]    [1]     [2]   
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x10 SessionControl\n"<<std::flush;
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    sub_function = stored_data[sid_position + 1];
                    std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                    /* sessionControlRequest(id, sub_function); */
                }
                break;
            }
        case 0x50: 
            {
                /** SessionControl -response handle --to be implemented  
                 *  Expected response: pci_l + sid + sub_funct
                 *  Index              [0]    [1]     [2]   
                 */
                std::cout << "Response 0x50 for SessionControl\n"<<std::flush;
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;   
                sub_function = stored_data[sid_position + 1];
                std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* sessionControlResponse(id, sub_function); */
                break;
            }
        case 0x11: 
            {
                /** EcuReset -request method --to be implemented 
                 *  Expected request: pci_l + sid + sub_funct
                 *  Index              [0]    [1]     [2]   
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x11 EcuReset\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;  
                    sub_function = stored_data[sid_position + 1];
                    std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                    /* ecuResetRequest(id, sub_function); */
                }
                break;
            }
        case 0x51: 
            {
                /** EcuReset -response handle --to be implemented
                 *  Expected response: pci_l + sid + sub_funct
                 *  Index              [0]    [1]     [2]   
                 */
                std::cout << "Response 0x51 for EcuReset\n"; 
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;  
                sub_function = stored_data[sid_position + 1];
                std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                /* ecuResetResponse(id, sub_function); */
                /* the request succesfully received-> store the data; */ 
                break;
            }
        case 0x22: 
            {
                /** ReadDataByIdentifier -request method --to be implemented  
                 *  Expected request: pci_l + sid + identifier[2]
                 *  Index              [0]    [1]     [2][3]   
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x22 ReadDataByIdentifier\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    
                    /* Combine the two bytes into a single 16-bit identifier */ 
                    uint16_t identifier = (stored_data[2] << 8) | stored_data[3];
                    std::cout << "Stored data: " << static_cast<int>(identifier) << std::endl;
                    /* readDataByIdentifierRequest(id, identifier); */
                }
                break;
            }
        case 0x62: 
            {
                /* ReadDataByIdentifier -response handle --to be implemented */ 
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;
                
                if (is_single_frame) 
                {
                    /** Expected response: pci_l + sid + identifier[2] + data[]
                     *  Index              [0]    [1]     [2][3]         [4]   
                     *  Combine the two bytes into a single 16-bit identifier 
                     */ 
                    uint16_t identifier = (stored_data[2] << 8) | stored_data[3];
                    std::cout << "Stored data: " << static_cast<int>(identifier) << std::endl;
                    std::vector<uint8_t> rdata(stored_data.begin() + 4, stored_data.end());
                    std::cout << "RData: ";
                    for (const auto& data : rdata) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " "; 
                    }
                    std::cout << std::endl;
                
                    std::cout << "Response 0x62 for ReadDataByIdentifier\n";
                    /* readDataByIdentifierResponse(id, identifier, rdata); */
                    /* the request succesfully received-> store the data; */ 
                } 
                else 
                {   
                    /** First frame:
                     *  Expected response: 0x10 + pci_l + sid + identifier[2] + data[]
                     *  Index              [0]    [1]     [2]    [3][4]         [5]   
                     *  Consecutive frame:
                     *  Expected response: 0x2X + data[]
                     *  Index              [0]    [1]    
                     *  Combine the two bytes into a single 16-bit identifier 
                     */ 
                    std::cout << "Response 0x62 for ReadDataByIdentifierLong\n";
                    uint16_t identifier = (stored_data[3] << 8) | stored_data[4];
                    std::cout << "Stored data: " << static_cast<int>(identifier) << std::endl;
                    std::vector<uint8_t> rdata(stored_data.begin() + 5, stored_data.end());
                    std::cout << "RData: ";
                    for (const auto& data : rdata) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                    /* the request succesfully received-> store the data; */ 
                    /* readDataByIdentifierLongResponse(id, identifier, rdata); */
                }
                break;
            }
        case 0x29: 
            {
                /* AuthenticationRequestSeed& AuthenticationSendKey - request methods --to be implemented */ 
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    if (stored_data[2] == 0x1)
                    {
                        /** AuthenticationRequestSeed -request method --to be implemented 
                         *  Expected request: pci_l + sid + sub_funct
                         *  Index              [0]    [1]     [2]   
                         */
                        std::cout << "Service 0x29 AuthenticationRequestSeed\n";
                        std::cout << "SID pos: " << sid_position << std::endl;
                        std::cout << "Data size: " << stored_data.size() << std::endl;
                        sub_function = stored_data[sid_position + 1];
                        std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                        /* authenticationRequestSeedRequest(id, sub_function); */
                    }
                    else if (stored_data[2] == 0x2)
                    {
                        /** AuthenticationAuthenticationSendKey -request method --to be implemented 
                         *  Expected request: pci_l + sid + sub_funct + key
                         *  Index              [0]    [1]     [2]       [3] 
                         */
                        std::cout << "Service 0x29 AuthenticationSendKey\n";
                        std::cout << "SID pos: " << sid_position << std::endl;
                        std::cout << "Data size: " << stored_data.size() << std::endl;
                        sub_function = stored_data[sid_position + 1];
                        std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                        uint8_t key = stored_data[3];
                        std::cout << "key: " << static_cast<int>(key) << std::endl;
                        /* authenticationSendKeyRequest(id, sub_function, key); */
                    }
                }
                break;
            }
        case 0x69: 
            {
                /* AuthenticationRequestSeed& AuthenticationSendKey -response handle --to be implemented */ 
                if (stored_data[2] == 0x1)
                {
                    /** AuthenticationRequestSeed -response method --to be implemented  
                     *  Expected response: pci_l + sid + sub_funct + seed 
                     *  Index              [0]    [1]     [2]        [3] 
                     */
                    std::cout << "Response 0x69 for AuthenticationRequestSeed\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    sub_function = stored_data[sid_position + 1];
                    std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                    uint8_t seed = stored_data[3];
                    std::cout << "seed: " << static_cast<int>(seed) << std::endl;
                    /* the request succesfully received-> store the data; */ 
                    /* authenticationRequestSeedResponse(id, sub_function, seed); */

                }
                else if (stored_data[2] == 0x2)
                {
                    /** AuthenticationAuthenticationSendKey- response handle --to be implemented 
                     *  Expected response: pci_l + sid + sub_funct
                     *  Index              [0]    [1]     [2]     
                     */
                    std::cout << "Response 0x69 for AuthenticationSendKey\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    sub_function = stored_data[sid_position + 1];
                    std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                    /* the request succesfully received-> store the data; */ 
                    /* authenticationSendKeyResponse(id, sub_function); */
                }
                break;
            }
        case 0x31:
            { 
                /* RoutineControl -request method --to be implemented */ 
                /** Expected request: pci_l + sid + sub_funct + Routin_id[2] 
                 *  Index              [0]    [1]     [2]        [3][4]     
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x31 RoutineControl\n"; 
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    sub_function = stored_data[sid_position + 1];
                    std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                    std::vector<uint8_t> routin_id(stored_data.begin() + 3, stored_data.end());
                    std::cout << "Routin_id: ";
                        for (const auto& data : routin_id) {
                            std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                        }
                        std::cout << std::endl;
                    
                    /* routineControlRequest(id, sub_function, routin_id); */ 
                }
                break;
            }
        case 0x71: 
            {
                /* RoutineControl -response handle --to be implemented */ 
                std::cout << "Response 0x71 for RoutineControl\n";
                /** Expected response: pci_l + sid + sub_funct + Routin_id[2] 
                 *  Index              [0]    [1]     [2]        [3][4]     
                 */
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;
                sub_function = stored_data[sid_position + 1];
                std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                std::vector<uint8_t> routin_id(stored_data.begin() + 3, stored_data.end());
                std::cout << "Routin_id: ";
                    for (const auto& data : routin_id) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* routineControlResponse(id, sub_function, routin_id); */ 
                break;
            }
        case 0x3E: 
            {
                /* TesterPresent -request method --to be implemented */ 
                /** Expected request: pci_l + sid + sub_funct = 0x00 
                 *  Index              [0]    [1]     [2]     
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x3E TesterPresent\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;  
                    sub_function = stored_data[sid_position + 1];
                    std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                    /* testerPresentRequest(id, sub_function); */
                }
                break;
            }
        case 0x7E: 
            {
                /* TesterPresent -response handle --to be implemented */ 
                std::cout << "Response 0x7E for TesterPresent\n";
                /** Expected response: pci_l + sid + sub_funct = 0x00 
                 *  Index              [0]    [1]     [2]     
                 */
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;  
                sub_function = stored_data[sid_position + 1];
                std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* testerPresentResponse(id, sub_function); */
                break;
            }
        case 0x23: 
            {
                /* ReadMemoryByAddress -request method --to be implemented */ 
                /** Expected request: pci_l + sid + address_memory_lenght + memory_address[] + memory_size[] 
                 *  Index              [0]    [1]         [2]                       [3]          [3 + size(memory_adress)]      
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x23 ReadMemoryByAddress\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    
                    /* Retrieve the address_memory_length */ 
                    uint8_t address_memory_length = stored_data[2];
                    /* Determine the length of memory address and memory size */ 
                    /* High nibble */ 
                    uint8_t length_memory_address = (address_memory_length & 0xF0) >> 4; 
                    /* Low nibble */ 
                    uint8_t length_memory_size = address_memory_length & 0x0F; 
                    /* Retrieve the memory_address and memory_size based on their lengths */
                    std::vector<int> memory_address(stored_data.begin() + 3, stored_data.begin() + 3 + length_memory_address);
                    std::vector<int> memory_size(stored_data.begin() + 3 + length_memory_address, stored_data.begin() + 3 + length_memory_address + length_memory_size);
                    std::cout << "Valid data length\n";
                    std::cout << "address_memory_length: " << std::hex << static_cast<int>(address_memory_length) << std::endl;
                    std::cout << "length_memory_address: " << static_cast<int>(length_memory_address) << std::endl;
                    std::cout << "length_memory_size: " << static_cast<int>(length_memory_size) << std::endl;
                    std::cout << "Memory_address: ";
                    for (const auto& data : memory_address) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                    std::cout << "Memory_size: ";
                    for (const auto& data : memory_size) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                    /* readMemoryByAdressRequest(id, address_memory_length, memory_address, memory_size); */
                }
                break;
            }
        case 0x63: 
            {
                /* ReadMemoryByAddress -response handle --to be implemented */ 
                if (is_single_frame) 
                {
                    /** Expected response: pci_l + sid + address_memory_lenght + memory_address[] + memory_size[] +            data[] 
                     *  Index              [0]    [1]         [2]                       [3]       [3 + size(memory_adress)]    [3 + size(memory_adress)+size(memory_size)] 
                     */
                    std::cout << "Response 0x63 for ReadMemoryByAddress\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    
                    /* Retrieve the address_memory_length */ 
                    uint8_t address_memory_length = stored_data[2];
                    /* Determine the length of memory address and memory size */ 
                    /* High nibble */ 
                    uint8_t length_memory_address = (address_memory_length & 0xF0) >> 4; 
                    /* Low nibble */ 
                    uint8_t length_memory_size = address_memory_length & 0x0F; 
                    /* Retrieve the memory_address and memory_size based on their lengths */
                    std::vector<uint8_t> memory_address(stored_data.begin() + 3, stored_data.begin() + 3 + length_memory_address);
                    std::vector<uint8_t> memory_size(stored_data.begin() + 3 + length_memory_address, stored_data.begin() + 3 + length_memory_address + length_memory_size);
                    int start_pos = 3 + length_memory_address + length_memory_size;
                    std::vector<uint8_t> rdata(stored_data.begin() + start_pos, stored_data.end());
                    std::cout << "Valid data length\n";
                    std::cout << "address_memory_length: " << std::hex << static_cast<int>(address_memory_length) << std::endl;
                    std::cout << "length_memory_address: " << static_cast<int>(length_memory_address) << std::endl;
                    std::cout << "length_memory_size: " << static_cast<int>(length_memory_size) << std::endl;
                    std::cout << "Memory_address: ";
                    for (const auto& data : memory_address) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                    std::cout << "Memory_size: ";
                    for (const auto& data : memory_size) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                    /* the request succesfully received-> store the data; */ 
                    /* readMemoryByAdressResponse(id, address_memory_length, memory_address, memory_size, rdata); */
                } 
                else 
                {   
                    /** First frame:
                     *  Expected response: 0x10 + pci_l + sid + address_memory_lenght + memory_address[] + memory_size[] +            data[]
                     *  Index              [0]    [1]    [2]              [3]            [4]            [4 + size(memory_adress)]    [4 + size(memory_adress)+size(memory_size)]
                     *  Consecutive frame:
                     *  Expected response: 0x2X + data[]
                     *  Index              [0]    [1]     
                     */
                    std::cout << "Reponse 0x63 for ReadMemoryByAddressLong\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    
                    /* Retrieve the address_memory_length */ 
                    uint8_t address_memory_length = stored_data[3];
                    /* Determine the length of memory address and memory size */ 
                    /* High nibble */ 
                    uint8_t length_memory_address = (address_memory_length & 0xF0) >> 4; 
                    /* Low nibble */ 
                    uint8_t length_memory_size = address_memory_length & 0x0F; 
                    /* Retrieve the memory_address and memory_size based on their lengths */
                    std::vector<int> memory_address(stored_data.begin() + 4, stored_data.begin() + 4 + length_memory_address);
                    std::vector<int> memory_size(stored_data.begin() + 4 + length_memory_address, stored_data.begin() + 4 + length_memory_address + length_memory_size);
                    int start_pos = 4 + length_memory_address + length_memory_size;
                    std::vector<uint8_t> rdata(stored_data.begin() + start_pos, stored_data.end());
                    std::cout << "Valid data length\n";
                    std::cout << "address_memory_length: " << std::hex << static_cast<int>(address_memory_length) << std::endl;
                    std::cout << "length_memory_address: " << static_cast<int>(length_memory_address) << std::endl;
                    std::cout << "length_memory_size: " << static_cast<int>(length_memory_size) << std::endl;
                    std::cout << "Memory_address: ";
                    for (const auto& data : memory_address) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                    std::cout << "Memory_size: ";
                    for (const auto& data : memory_size) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* readMemoryByAdressLongResponse(id, address_memory_length, memory_address, memory_size, rdata); */
                }
                break;
            }
        case 0x2E: 
            {
                /* WriteDataByIdentifier&WriteDataByIdentifierLongRequest--request methods -- to be implemented */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    
                    if (is_single_frame) 
                    {
                        /** WriteDataByIdentifier -request method --to be implemented 
                         *  Expected request: pci_l + sid + identifier[2] + data_parameter[]
                         *  Index              [0]    [1]     [2][3]         [4]   
                         *  Combine the two bytes into a single 16-bit identifier 
                         */ 
                        uint16_t identifier = (stored_data[2] << 8) | stored_data[3];
                        std::cout << "Stored data: " << static_cast<int>(identifier) << std::endl;
                        std::vector<uint8_t> rdata(stored_data.begin() + 4, stored_data.end());
                        std::cout << "RData: ";
                        for (const auto& data : rdata) {
                            std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                        }
                        std::cout << std::endl;
                    
                        std::cout << "Service 0x2E WriteDataByIdentifier\n";
                        /* writeDataByIdentifier(id, identifier, rdata); */
                    } 
                    else 
                    {   
                        /** writeDataByIdentifierLong -request method --to be implemented
                         *  First frame:
                         *  Expected request: 0x10 + pci_l + sid + identifier[2] + data_parameter[]
                         *  Index              [0]    [1]     [2]    [3][4]         [5]   
                         *  Consecutive frame:
                         *  Expected response: 0x2X + data_parameter[]
                         *  Index              [0]     [1]    
                         *  Combine the two bytes into a single 16-bit identifier 
                         */ 
                        std::cout << "Service 0x2E WriteDataByIdentifierLongRequest\n";
                        uint16_t identifier = (stored_data[3] << 8) | stored_data[4];
                        std::cout << "Stored data: " << static_cast<int>(identifier) << std::endl;
                        std::vector<uint8_t> rdata(stored_data.begin() + 5, stored_data.end());
                        std::cout << "RData: ";
                        for (const auto& data : rdata) {
                            std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                        }
                        std::cout << std::endl;
                        /* writeDataByIdentifierLongRequest(id, identifier, rdata); */
                    }
                }
                break;
            }
        case 0x6E: 
            {
                /** WriteDataByIdentifier- response handle --to be implemented 
                 *  Expected response: pci_l + sid + identifier[2]
                 *  Index              [0]    [1]     [2][3]   
                 */
                std::cout << "Response 0x6E for WriteDataByIdentifier\n";
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;
                
                /* Combine the two bytes into a single 16-bit identifier */ 
                uint16_t identifier = (stored_data[2] << 8) | stored_data[3];
                std::cout << "identifier: " << identifier << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* writeDataByIdentifierResponse(id, identifier); */
                break;
            }
        case 0x19: 
            {
                /** ReadDtcInformation -request method --to be implemented
                 *  Expected request: pci_l + sid + sub_funct + dtc_status_mask
                 *  Index              [0]    [1]     [2]         [3]
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x19 ReadDtcInformation\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    sub_function = stored_data[sid_position + 1];
                    std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                    uint8_t dtc_status_mask = stored_data[sid_position + 2];
                    std::cout << "mask: " << static_cast<int>(dtc_status_mask) << std::endl;
                    /* readDtcInformationRequest(id, sub_function, dtc_status_mask); */
                }
                break;
            }
        case 0x59:
            {
                /** ReadDtcInformation -response handle --to be implemented
                 *  Expected response: pci_l + sid + 0x01 + sts_ava_mask + dtc_format + dtc_count
                 *  Index              [0]    [1]     [2]     [3]            [4]         [5]
                 */
                std::cout << "Response 0x59 for ReadDtcInformation\n"<<std::flush;
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;   
                sub_function = stored_data[sid_position + 1];
                std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                uint8_t sts_ava_mask = stored_data[3];
                std::cout << "mask availab: " << static_cast<int>(sts_ava_mask) << std::endl;
                uint8_t dtc_format = stored_data[4];
                std::cout << "dtc format: " << static_cast<int>(dtc_format) << std::endl;
                uint8_t dtc_count = stored_data[5];
                std::cout << "dtc count: " << static_cast<int>(dtc_count) << std::endl;
                /* the request succesfully received-> store the data; */  
                /* readDtcInformationResponse(id, sub_function, sts_ava_mask, dtc_format, dtc_count); */ 
                break;
            }
        case 0x14: 
            {
                /** ClearDiagnosticInformation- request method --to be implemented 
                 *  Expected request: pci_l + sid + empty_vector[]
                 *  Index              [0]    [1]     [2]   
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x14 ClearDiagnosticInformation\n"<<std::flush;
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    std::vector<uint8_t> empty_vector(stored_data.begin() + 2, stored_data.end());
                    std::cout << "Empty_vector: ";
                        for (const auto& data : empty_vector) {
                            std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                        }
                        std::cout << std::endl;
                    /* clearDiagnosticInformationRequest(id, empty_vector); */
                }
                break;
            }
        case 0x54: 
            {
                /** ClearDiagnosticInformation -response handle --to be implemented 
                 *  Expected response: pci_l + sid + group_of_dtc[]
                 *  Index              [0]    [1]     [2]   
                 */
                std::cout << "Response 0x54 for ClearDiagnosticInformation\n"<<std::flush;
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;   
                uint8_t group_of_dtc = stored_data[sid_position + 1];
                std::cout << "group_of_dtc: " << static_cast<int>(group_of_dtc) << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* clearDiagnosticInformationResponse(id, group_of_dtc); */
                break;
            }
        case 0x83: 
            {
                /** AccessTimingParameters request method --to be implemented 
                 *  Expected request: pci_l + sid + sub_funct
                 *  Index              [0]    [1]     [2]   
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {
                    std::cout << "Service 0x83 AccessTimingParameters\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    sub_function = stored_data[sid_position + 1];
                    std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                    /* accessTimingParametersRequest(id, sub_function); */
                }
                break;
            }
        case 0xC3: 
            {
                /** AccessTimingParameters -response handle --to be implemented  
                 *  Expected response: pci_l + sid + sub_funct
                 *  Index              [0]    [1]     [2]   
                 */
                std::cout << "Response 0xC3 for AccessTimingParameters\n"<<std::flush;
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;   
                sub_function = stored_data[sid_position + 1];
                std::cout << "sub_function: " <<  static_cast<int>(sub_function) << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* accessTimingParametersResponse(id, sub_function); */
                break;
            }
        /* OTA */ 
        case 0x34: 
            {
                /* RequestDownload- request method --to be implemented */ 
                /** Expected request: pci_l + sid + data_format_identifier  +  address_memory_lenght + memory_address[] +  memory_size[] 
                 *  Index              [0]     [1]         [2]                       [3]                   [4]           [4 + size(memory_adress)]    
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {   
                    std::cout << "Service 0x34 RequestDownload\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    
                    uint8_t data_format_identifier = stored_data[2];
                    /* Retrieve the address_memory_length */ 
                    uint8_t address_memory_length = stored_data[3];
                    /* Determine the length of memory address and memory size */ 
                    /* High nibble */ 
                    uint8_t length_memory_address = (address_memory_length & 0xF0) >> 4; 
                    /* Low nibble */ 
                    uint8_t length_memory_size = address_memory_length & 0x0F; 
                    /* Retrieve the memory_address and memory_size based on their lengths */
                    std::vector<int> memory_address(stored_data.begin() + 4, stored_data.begin() + 4 + length_memory_address);
                    std::vector<int> memory_size(stored_data.begin() + 4 + length_memory_address, stored_data.begin() + 4 + length_memory_address + length_memory_size);
                    std::cout << "Valid data length\n";
                    std::cout << "address_memory_length: " << std::hex << static_cast<int>(address_memory_length) << std::endl;
                    std::cout << "length_memory_address: " << static_cast<int>(length_memory_address) << std::endl;
                    std::cout << "length_memory_size: " << static_cast<int>(length_memory_size) << std::endl;
                    std::cout << "Memory_address: ";
                    for (const auto& data : memory_address) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                    std::cout << "Memory_size: ";
                    for (const auto& data : memory_size) {
                        std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                    }
                    std::cout << std::endl;
                    /* requestDownloadRequest(id, data_format_identifier, address_memory_length, memory_address, memory_size); */
                }
                break;
            }
        case 0x74: 
            {
                /* RequestDownload -response handle --to be implemented */ 
                std::cout << "Response 0x74 for RequestDownload\n";
                /** Expected response: pci_l + sid +  length_max_number_block*0x10  +  max_number_block
                 *  Index              [0]     [1]         [2]                       [3]                  
                 */
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;
                
                uint8_t length_max_number_block = stored_data[2];
                std::cout << "length max no blocks: " << length_max_number_block << std::endl;
                int max_number_block = stored_data[3];
                std::cout << "max no blocks: " << max_number_block << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* requestDownloadResponse(id, length_max_number_block, max_number_block); */
                break;
            }
        case 0x36: 
            {
                /* TransferData&TransferDataLong -request methods --to be implemented */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {   
                    if (is_single_frame) 
                    {
                        /** TransferData -request method --to be implemented 
                         *  Expected request: pci_l + sid + block_sequence_counter + transfer_request[]
                         *  Index              [0]    [1]      [2]                     [3]
                         */
                        std::cout << "Service 0x36 TransferDataRequest\n";
                        std::cout << "SID pos: " << sid_position << std::endl;
                        std::cout << "Data size: " << stored_data.size() << std::endl;
                        uint8_t block_sequence_counter = stored_data[2];
                        std::cout << "block_counter: " << static_cast<int>(block_sequence_counter) << std::endl;
                        std::vector<uint8_t> transfer_request(stored_data.begin() + 3, stored_data.end());
                        std::cout << "Transfer Request Data: ";
                        for (const auto& data : transfer_request) {
                            std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                        }
                        std::cout << std::endl;
                        /* transferDataRequest(id, block_sequence_counter, transfer_request); */
                    }
                    else
                    {
                        /** TransferDataLong -request method --to be implemented 
                         *  First frame:
                         *  Expected request: 0x10 + pci_l + sid + block_sequence_counter + transfer_request[] 
                         *  Index              [0]    [1]    [2]              [3]            [4]      
                         *  Consecutive frame:
                         *  Expected response: 0x2X + transfer_request[]
                         *  Index              [0]      [1]     
                         */
                        std::cout << "Service 0x36 TransferDataLongRequest\n";
                        std::cout << "SID pos: " << sid_position << std::endl;
                        std::cout << "Data size: " << stored_data.size() << std::endl;
                        uint8_t block_sequence_counter = stored_data[3];
                        std::cout << "block_counter: " << block_sequence_counter << std::endl;
                        std::vector<uint8_t> transfer_request(stored_data.begin() + 4, stored_data.end());
                        std::cout << "Transfer Request Data: ";
                        for (const auto& data : transfer_request) {
                            std::cout << std::hex << "0x" << static_cast<int>(data) << " ";
                        }
                        std::cout << std::endl;
                        /* transferDataLongRequest(id, block_sequence_counter, transfer_request); */
                    }
                }
                break;
            }
        case 0x76: 
            {
                /** TransferData - response handle --to be implemented 
                 *  Expected response: pci_l + sid + block_sequence_counter 
                 *  Index              [0]    [1]      [2]                   
                 */
                std::cout << "Response 0x76 for TransferData\n";
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;
                uint8_t block_sequence = stored_data[2];
                std::cout << "block_counter: " << block_sequence << std::endl;
                /* the request succesfully received-> store the data; */ 
                /* transferDataResponse(id, block_sequence_counter); */
                break;
            }
        case 0x37: 
            {
                /** RequestTransferExit - request method --to be implemented
                 *  Expected request: pci_l + sid + empty_vector
                 *  Index              [0]    [1]      [2]                    
                 */
                if (stored_data[1] == 0x7F)
                {
                    processNrc(id, sid, stored_data[3]);
                }
                else
                {   
                    std::cout << "Service 0x37 RequestTransferExit\n";
                    std::cout << "SID pos: " << sid_position << std::endl;
                    std::cout << "Data size: " << stored_data.size() << std::endl;
                    uint8_t empty_vector = stored_data[2];
                    /* requestTransferExitRequest(id, empty_vector); */
                }
                break;
            }
        case 0x77: 
            {
                /** RequestTransferExit -response handle --to be implemented
                 *  Expected response: pci_l + sid + empty_vector
                 *  Index              [0]    [1]      [2]                    
                 */
                std::cout << "Response 0x77 for RequestTransferExit\n";
                std::cout << "SID pos: " << sid_position << std::endl;
                std::cout << "Data size: " << stored_data.size() << std::endl;
                uint8_t empty_vector = stored_data[2];
                /* the request succesfully received-> store the data; */ 
                /* requestTransferExitResponse(id, empty_vector); */
                break;
            }
        case 0x32: 
            {
                /* requestUpdateStatus - request method --to be implemented */ 
                std::cout << "Service 0x32 RequestUpdateStatus\n";
                /* requestUpdateStatus(id); */
                break;
            }
        default:
            std::cerr << "Unknown service" << std::endl;
            break;
    }
}

void HandleFrames::processNrc(int id, uint8_t sid, uint8_t nrc)
{
    switch(nrc)
    {
        case 0x11:
            /* Service not supported */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: Service not supported for service: " << sid << std::endl;
        break;
        case 0x13:
            /* Incorrect message length or invalid format */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: Incorrect message length or invalid format for service: " << sid << std::endl;
        break;
        case 0x14:
            /*  Response too long */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: Response too long for service: " << sid << std::endl;
        break;
        case 0x25:
            /* No response from subnet component */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: No response from subnet component for service: " << sid << std::endl;
        break;
        case 0x34:
            /* Authentication failed */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: Authentication failed for service: " << sid << std::endl;
        break;
        case 0x94:
            /* Resource temporarily unavailable */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: Resource temporarily unavailable for service: " << sid << std::endl;
        break;
        case 0x70:
            /* Upload download not accepted */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: Upload download not accepted for service: " << sid << std::endl;
        break;
        case 0x71:
            /* Transfer data suspended */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: Transfer data suspended for service: " << sid << std::endl;
        break;
        default:
            /* Unknown negative response code */
            /* negativeResponse(can_id, sid, nrc); */
            std::cout << "Error: Unknown negative response code for service: " << sid << std::endl;
        break;
    }
}
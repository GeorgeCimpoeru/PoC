#include "../include/ReadDtcInformation.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../ecu_simulation/DoorsModule/include/DoorsModule.h"
#include "../../../ecu_simulation/HVACModule/include/HVACModule.h"
#include "../../../mcu/include/MCUModule.h"


ReadDTC::ReadDTC(Logger logger, std::string path_folder, int socket)
{
    this->path_folder = path_folder;
    this->logger = logger;
    this->socket = socket;
}

ReadDTC::~ReadDTC()
{
    delete this->generate;
}

void ReadDTC::read_dtc(int id, std::vector<uint8_t> data)
{
    LOG_INFO(logger.GET_LOGGER(), "Read DTC Service");
    this->generate = new GenerateFrames(socket,logger);
    uint8_t lowerbits = id & 0xFF;
    uint8_t upperbits = id >> 8 & 0xFF;
    int new_id = ((lowerbits << 8) | upperbits);
    if (data.size() != 4)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Incorrect message length or invalid format");
        this->generate->negativeResponse(new_id, 0x19, 0x13);
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x19);
        return;
    }
    int sub_function = data[2];
    int dtc_status_mask = data[3];
    
    switch (sub_function)
    {
        case 0x01:
            LOG_INFO(logger.GET_LOGGER(), "Sub-function 1");
            number_of_dtc(id,dtc_status_mask);
            break;
        case 0x02:
            LOG_INFO(logger.GET_LOGGER(), "Sub-function 2");
            report_dtcs(id,dtc_status_mask);
            break;
        default:
            this->generate->negativeResponse(new_id, 0x19, 0x12);
            LOG_ERROR(logger.GET_LOGGER(), "Sub-function not supported");
            AccessTimingParameter::stopTimingFlag(lowerbits, 0x19);
    }
}

void ReadDTC::number_of_dtc(int id, int dtc_status_mask)
{
    int new_id = ((id & 0xFF) << 8) | ((id >> 8) & 0xFF);
    std::ifstream MyReadFile;
    try
    {
        MyReadFile.open(this->path_folder);
        if (!MyReadFile.is_open())
        {
            throw std::runtime_error("Unable to open file");
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Unable to read DTCs");
        /* NRC Resource temporarily unavailable */
        this->generate->negativeResponse(new_id, 0x19, 0x94);
        uint8_t lowerbits = id & 0xFF;
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x19);
        return;
    }

    uint8_t status_availability_mask=0;
    int number_of_dtc = 0;
    std::string line;
    /* Read from the file all dtcs */
    LOG_INFO(logger.GET_LOGGER(), "Reading DTCs...");
    while (std::getline (MyReadFile, line))
    {
        int status = to_int(line[9]) * 0x10 + to_int(line[10]);
        status_availability_mask |= status;
        /* Filter by status mask */
        if ((status & dtc_status_mask) || dtc_status_mask == 0)
        {
            number_of_dtc++;
        }
    }
    MyReadFile.close();
    /* dtc format_idtf 0x01 -> ISO_14229-1_DTCFormat */
    int dtc_format_identifier = 0x01;

    uint8_t receiver_id = id & 0xFF;

    this->generate->readDtcInformationResponse01(new_id,status_availability_mask,dtc_format_identifier,number_of_dtc);
    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x19);

    AccessTimingParameter::stopTimingFlag(receiver_id, 0x19);
}

void ReadDTC::report_dtcs(int id, int dtc_status_mask)
{
    int new_id = ((id & 0xFF) << 8) | ((id >> 8) & 0xFF);
    std::ifstream MyReadFile;
    try
    {
        MyReadFile.open(this->path_folder);
        if (!MyReadFile.is_open())
        {
            throw std::runtime_error("Unable to open file");
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Unable to read DTCs");
        /* NRC Resource temporarily unavailable */
        this->generate->negativeResponse(new_id, 0x19, 0x94);
        uint8_t lowerbits = id & 0xFF;
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x19);
        return;
    }
    
    uint8_t status_availability_mask=0;
    std::vector<std::pair<int, int>> dtc_and_status_list;
    std::string line;
    /* Read from the file all dtcs */
    LOG_INFO(logger.GET_LOGGER(), "Reading DTCs...");
    while (std::getline (MyReadFile, line))
    {
        int status = to_int(line[9]) * 0x10 + to_int(line[10]);
        status_availability_mask |= status;
        /* Filter by status mask */
        if ((status & dtc_status_mask) || dtc_status_mask == 0)
        {
            int dtc_hex = dtc_to_hex(line);
            dtc_and_status_list.push_back({dtc_hex,status});
        }
    }
    MyReadFile.close();
    /* Send a frame/frames with all dtcs founded */
    if (dtc_and_status_list.size() > 1)
    {   
        uint8_t lowerbits = id & 0xFF;

        this->generate->readDtcInformationResponse02Long(new_id,status_availability_mask,dtc_and_status_list,true);
        this->generate->readDtcInformationResponse02Long(new_id,status_availability_mask,dtc_and_status_list,false);
        LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent the first response frame.", 0x19);

        AccessTimingParameter::stopTimingFlag(lowerbits, 0x19);
                
/** flow control frame not implemented yet -> uncomment this when ready
        if (receive_flow_control(id / 0x100))
        {
            uint8_t lowerbits = id & 0xFF;
            switch(lowerbits)
            {
                case 0x10:
                    this->generate->readDtcInformationResponse02Long(id,status_availability_mask,dtc_and_status_list,false);
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent a consecutive response frame.", 0x19);
                    MCU::mcu->stop_flags[0x19] = false;
                    break;
                case 0x11:
                    this->generate->readDtcInformationResponse02Long(id,status_availability_mask,dtc_and_status_list,false);
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent a consecutive response frame.", 0x19);
                    battery->stop_flags[0x19] = false;
                    break;
                case 0x12:
                    this->generate->readDtcInformationResponse02Long(id,status_availability_mask,dtc_and_status_list,false);
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent a consecutive response frame.", 0x19);
                    engine->stop_flags[0x19] = false;
                    break;
                default:
                    LOG_ERROR(logger.GET_LOGGER(), "Module with id {:x} not supported.", lowerbits);
            }
            
        }
        else
        {
            LOG_ERROR(logger.GET_LOGGER(), "Timeout. FLow control frame not received!");
        } 
*/
    }
    else 
    {
        uint8_t lowerbits = id & 0xFF;

        this->generate->readDtcInformationResponse02(new_id,status_availability_mask,dtc_and_status_list);
        LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent the first response frame.", 0x19);
        
        AccessTimingParameter::stopTimingFlag(lowerbits, 0x19);
    }
}

int ReadDTC::dtc_to_hex(std::string dtc)
{
    std::map<char,uint8_t> first_byte =
    {
        {'P',00}, {'C',01}, {'B',2}, {'U',3}
    };
    int hex = (first_byte[dtc[0]]<<6) | (to_int(dtc [1])<<4) | to_int(dtc[2]);
    hex = hex * 0x100 + to_int(dtc[3]) * 0x10 + to_int(dtc[4]);
    hex = hex * 0x100 + to_int(dtc[6]) * 0x10 + to_int(dtc[7]);
    return hex;
}

bool ReadDTC::receive_flow_control(int id_module)
{
    /* Define a pollfd structure to monitor the socket */ 
    struct pollfd pfd;
    /* Set the socket file descriptor */ 
    pfd.fd = this->socket; 
    /* We are interested in read events -use POLLING */ 
    pfd.events = POLLIN;

    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    LOG_INFO(logger.GET_LOGGER(), "Waiting 6 sec until recive flow control frame...");
    while (true)
    {
        /* Use poll to wait for data to be available with a timeout of 1000ms (1 second) */ 
        int poll_result = poll(&pfd, 1, 1000);

        if (poll_result > 0 && pfd.revents & POLLIN) 
        {
            struct can_frame frame ;
            int nbytes = read(this->socket, &frame, sizeof(frame));

            if (nbytes > 0) 
            {
                if ((int)frame.can_id % 0x100 == id_module && frame.data[0] == 0x30)
                {
                    LOG_INFO(logger.GET_LOGGER(), "Flow controll frame recived...");
                    return true;
                }    
            }
        }
        end = std::chrono::system_clock::now();
        if((std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 6)) 
            break;
    }
    return false;
}

int ReadDTC::to_int(char c)
{
    return (c >= 'A') ? (c - 'A' + 10) : (c - '0');
}
/**
 * @file request_update_status.cpp
 * @author Iancu Daniel
 * @brief 
 * @version 0.1
 * @date 2024-06-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "../include/request_update_status.h"

RequestUpdateStatus::RequestUpdateStatus(canid_t frame_id, std::vector<uint8_t> frame_data, Logger logger) : _logger{logger}
{

}

void RequestUpdateStatus::requestUpdateStatus(canid_t frame_id, std::vector<uint8_t> frame_data)
{
    
    ReadDataByIdentifier RIDB;
    RIDB.readDataByIdentifier(canid_t frame_id, std::vector<uint8_t> frame_data, _logger);
}

RequestUpdateStatus::~RequestUpdateStatus()
{

}
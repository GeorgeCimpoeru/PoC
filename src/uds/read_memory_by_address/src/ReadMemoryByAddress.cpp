#include "../include/ReadMemoryByAddress.h"

ReadMemoryByAddress::ReadMemoryByAddress(MemoryManager* memManager, GenerateFrames& frameGen, int socket, Logger& log)
    : memoryManager(memManager), frameGenerator(frameGen), socket(socket), logger(log) {}

void ReadMemoryByAddress::handleRequest(int can_id, off_t memory_address, off_t memory_size) {
    /* Log the incoming request */
    LOG_INFO(logger.GET_LOGGER(), "Handling ReadMemoryByAddress request");
    if (memory_address < 0 || memory_size < 0) {
        LOG_INFO(logger.GET_LOGGER(), "Incorrect message length or invalid format");
        NegativeResponse nrc(socket, logger);
        nrc.sendNRC(can_id, 0x23, NegativeResponse::IMLOIF);
    }

    /* Check if the memory address and size are valid */
    if (!memoryManager->availableAddress(memory_address) || !memoryManager->availableMemory(memory_size)) {
        LOG_INFO(logger.GET_LOGGER(), "Invalid address/size");
        NegativeResponse negative_response(socket, logger);
        negative_response.sendNRC(can_id, 0x23, 0x13); /* Sending Negative Response for invalid address/size */
        return;
    }

    if (!SecurityAccess::getMcuState(logger)) {
        LOG_INFO(logger.GET_LOGGER(), "Security Access Denied");
        NegativeResponse nrc(socket, logger);
        nrc.sendNRC(can_id, 0x23, NegativeResponse::SAD);
        return;
    }

    /* Read data from memory */
    std::vector<uint8_t> data = MemoryManager::readFromAddress(memoryManager->getPath(), memory_address, memory_size, logger);
    
    if (data.empty()) {
        LOG_ERROR(logger.GET_LOGGER(), "Failed to read memory");
        NegativeResponse negative_response(socket, logger);
        negative_response.sendNRC(can_id, 0x23, 0x31); /* Sending Negative Response for read failure */
        return;
    }

    /* Send the data back as a response depending on the size (if it's more than 6 bytes, split it into multiple frames) */
    if (data.size() > 6) {
        frameGenerator.readMemoryByAddressLongResponse(can_id, memory_address, memory_size, data, true); /* First frame */
        /* !!!! IMPORTANT: Wait for the Flow control frame from the Client !!!! */
        frameGenerator.readMemoryByAddressLongResponse(can_id, memory_address, memory_size, data, false); /* Remaining frames */
    } else {
        frameGenerator.readMemoryByAddress(can_id, memory_address, memory_size, data);
    }


    /* Log the success */
    LOG_INFO(logger.GET_LOGGER(), "ReadMemoryByAddress request handled successfully");
}
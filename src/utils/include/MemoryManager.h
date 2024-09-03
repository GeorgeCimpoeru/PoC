/**
 * @file MemoryManager.h
 * @author Mujdei Ruben
 * @brief 
 * @version 0.1
 * @date 2024-07-16
 * 
 * @copyright Copyright (c) 2024
 *  * !!!!Run wtih sudo when run programm
 * 
 * Commands to create sdcard
 *  ->this creates an img, run it only once
 * truncate -s +300M /home/projectx/sdcard.img
 * 
 *  ->this creates a loop partition with the image created above
 * sudo losetup -fP sdcard.img
 * 
 *  ->this lists all loops, here you should check loop number where sdcard.img is mounted,
 *  it could be loop21 or loop25 or any number
 * sudo losetup -a
 * 
 *  ->here replace loop21 with your loop number found with the command above
 * sudo fdisk /dev/loop21
 *  ->after typing this^ command, type n->enter, select 1, select primary, select 2048 as starting adress
 *  ->then type +100M to be the ending adress, nwo you created loop21p1
 *  ->now type n->enter, select 2, select primary, enter for default start adress, enter again for default
 *  end adress, now you created loop21p2
 *  ->now type w and enter to finish changes
 *  ->now type q-> enter to quit fdisk
 *  
 *  ->after finishing commands above run the two mkfs commands below to format those partitions
 * sudo mkfs.fat -F 32 /dev/loop21p1
 * sudo mkfs.fat -F 16 /dev/loop21p2
 * 
 * -> run mkdir to create /mnt folder
 * sudo mkdir /mnt/sdcard
 * 
 * -> run mount for both partitions created
 * sudo mount -o rw,uid=1000,gid=1000 /dev/loop21p1 /mnt/sdcard
 * sudo mount -o rw,uid=1000,gid=1000 /dev/loop21p2 /mnt/sdcard
 * 
 * -> this commands prints the data found at starting with adress 118006272
 * sudo xxd -l 17168 -s 118006272 /dev/loop21
 * 
 * -> this command replaces all data with zeros(erase)
 * sudo dd if=/dev/zero of=/dev/loop21 bs=1 seek=118006272 count=17168 conv=notrunc
 * 
 * -> last two commands are not mandatory, only for testing
 * MAIN:
    int main()
    {
        Logger l("ddd","log.log");
        off_t address = 230481 * 512; // Offset for /dev/loop21p2, dummy

        // This lines are called in the request download service to set the address and the path 
        MemoryManager* install = MemoryManager::getInstance(address, "/dev/loop21",&l);
    
        // data from an executable. Simulate data from transfer data frame
        std::vector<uint8_t> data = MemoryManager::readBinary("/home/projectx/Desktop/PoC/src/mcu/main");

        // This lines are called in the transfer data service to save the data 
        MemoryManager* install2 = MemoryManager::getInstance();
        install2->writeToAddress(data);

        // line called to move from adress to the executable(Can be implemented in a routine) 
        std::vector<uint8_t> data2 = MemoryManager::readFromAddress("/dev/loop21", install2->getAddress(), data.size()); //Read from the address
        MemoryManager::writeToFile(data2, "/mnt/sdcard/test"); //Write in bin the data
    }
    */
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <stdexcept>

#include <unistd.h>
#include <fcntl.h>

#include "Logger.h"

#define DEV_LOOP "/dev/loop16"

class MemoryManager
{
    private:
        std::string path;
        off_t address = -1;
        /*  Address to track where the last write was made */
        off_t address_continue_to_write = -1;
        static MemoryManager* instance;
        Logger& logger;

        /**
         * @brief Method to transform a string number to type integer
         * 
         * @param number 
         * @return int 
         */
        int to_int(std::string number);

        /**
         * @brief Method to run a command in the bash terminal
         * 
         * @param command 
         * @return std::string 
         */
        std::string runCommand(char command[]);

    public:
        /**
         * @brief Construct a new Memory Manager object
         * 
         * @param address 
         * @param path 
         * @param logger 
         */
        MemoryManager(off_t address, std::string path, Logger& logger);
        
        /**
         * @brief Get the Instance object
         * 
         * @return MemoryManager*, the only instance
         */
        static MemoryManager* getInstance(Logger& logger);

        /**
         * @brief Get the Instance object
         * 
         * @param address 
         * @param path 
         * @param logger 
         * @return MemoryManager*, the only instance
         */
        static MemoryManager* getInstance(off_t address, std::string path, Logger& logger);

        /**
         * @brief Singletons should not be cloneable.
         * 
         * @param object MemoryManager object
         */
        MemoryManager(MemoryManager &object) = delete;

        /**
         * @brief Set the Address object
         * 
         * @param address 
         */
        void setAddress(off_t address);

        /**
         * @brief Get the Address object
         * 
         * @return off_t 
         */
        off_t getAddress();

        /**
         * @brief Set the Path object
         * 
         * @param path 
         */
        void setPath(std::string path);

        /**
         * @brief Get the Path object
         * 
         * @return std::string 
         */
        std::string getPath();

        /**
         * @brief Method to read a binary file. This is a static method.
         * 
         * @param path_to_binary 
         * @return std::vector<uint8_t> 
         */

        /**
         * @brief Method to check if the address is available
         * 
         * @param address 
         * @return true or false
         */
        bool availableAddress(off_t address);

        /**
         * @brief Method to check if the amount of memory is available
         * 
         * @param size_of_data 
         * @return true or false
         */
        bool availableMemory(off_t size_of_data);
        static std::vector<uint8_t> readBinary(std::string path_to_binary, Logger& logger);

        /**
         * @brief Method to read from an address. This is a static method.
         * 
         * @param path Path to a sd, usb, etc
         * @param address_start Start address
         * @param size Amount of size to read
         * @return std::vector<uint8_t> 
         */
        static std::vector<uint8_t> readFromAddress(std::string path, off_t address_start, off_t size, Logger& logger);

        /**
         * @brief Method to write data in a specific address. This method uses the address specified in the constructor.
         * 
         * @param data Data to be written
         * @return true -if data was written successfully or false -if data wasn't written 
         */
        bool writeToAddress(std::vector<uint8_t> &data);

        /**
         * @brief Method to write data in a specific file. This is a static method.
         * 
         * @param data 
         * @param path_file 
         * @return true or false
         */
        static bool writeToFile(std::vector<uint8_t> &data, std::string path_file, Logger& logger);
        /**
         * @brief Reset instance
         * 
         */
        void resetInstance();
};

#endif
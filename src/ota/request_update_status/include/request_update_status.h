/**
 * @file request_update_status.h
 * @author Iancu Daniel
 * @brief 
 * @version 0.1
 * @date 2024-06-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef REQUEST_UPDATE_STATUS_H
#define REQUEST_UPDATE_STATUS_H

#include <iostream>
#include <vector>
#include <linux/can.h>
#include <map>
#include "../../../utils/include/Logger.h"
// #include "../../../uds/read_data_by_identifier/include/read_data_by_identifier.h"
#include "../../../utils/include/CreateInterface.h"
#include "../../../utils/include/GenerateFrames.h"

#define REQUEST_UPDATE_STATUS_SERVICE           0x32
#define REQUEST_UPDATE_STATUS_SERVICE_SUCCESS   0x72
#define REQUEST_UPDATE_STATUS_REQUEST_SIZE      0x04
#define REQUEST_UPDATE_STATUS_RESPONSE_SIZE	    0x01

#define READ_DATA_BY_IDENTIFIER_SID 0x34

#define OTA_STATUS_UPDATE_DID 0x01E0
#define OTA_STATUS_UPDATE_DID_MSB ((OTA_STATUS_UPDATE_DID & 0xFF00) >> 8)
#define OTA_STATUS_UPDATE_DID_LSB (OTA_STATUS_UPDATE_DID & 0x00FF)

#define SET_BYTE(n, i, b) (n = ((n & ~(0xFF << (i * 8))) | ((b & 0xFF) << (i * 8))))
#define GET_BYTE(n, i) ((n >> (i * 8)) & 0xFF)



enum class OtaUpdateStatesEnum
{
	IDLE = 0x00,        /* Initial state of the FOTA Handler after the ECU startup procedure */
    INIT = 0x10,        /* The FOTA Handler is initialized, and Dcm is set into the correct state (in Dcm FOTA session and security access has been granted) */
   	READY = 0x20,       /* All FOTA data chunks have been installed, activation procedure can be triggered */
   	PROCESSING = 0x30,  /* The FOTA Handler is triggered by the Dcm callout since a new chunk has been received and is processed in the callout context */
   	PROCESSING_TRANSFER_COMPLETE = 0x31,  /* The FOTA Handler is triggered by the Dcm callout since a new chunk has been received and is processed in the callout context */
   	PROCESSING_TRANSFER_FAILED = 0x32,  /* The FOTA Handler is triggered by the Dcm callout since a new chunk has been received and is processed in the callout context */
	WAIT = 0x40,        /* The FOTA Handler has successfully processed the last received data chunk, returned the Dcm callout function, and is waiting for the next data chunk */
    WAIT_DOWNLOAD_COMPLETED = 0x41,        /* The FOTA Handler has successfully processed the last received data chunk, returned the Dcm callout function, and is waiting for the next data chunk */
    WAIT_DOWNLOAD_FAILED = 0x42,        /* The FOTA Handler has successfully processed the last received data chunk, returned the Dcm callout function, and is waiting for the next data chunk */	
	VERIFY = 0x50,      /* Optional and implementer specific step, since the FOTA Target does not specify  any details on the verification process */
    ACTIVATE = 0x60,    /* FOTA installation has finished and received a respective service job from the FOTA Master that indicates the partition switch during the next boot process */
    ACTIVATE_INSTALL_COMPLETE = 0x61,    /* FOTA installation has finished and received a respective service job from the FOTA Master that indicates the partition switch during the next boot process */
    ACTIVATE_INSTALL_FAILED = 0x62,    /* FOTA installation has finished and received a respective service job from the FOTA Master that indicates the partition switch during the next boot process */
	ERROR = 0x70       /* Optional and implementer specific. Reserved state for e.g., implementer specific error handling, which is not (yet) covered by the FOTA Target */
};

class RequestUpdateStatus
{
private:        
	Logger _logger;
    CreateInterface* create_interface;
public:
	RequestUpdateStatus(Logger logger);
	~RequestUpdateStatus();

	void requestUpdateStatus(canid_t frame_id, std::vector<uint8_t> frame_data);
};

#endif /* REQUEST_UPDATE_STATUS_H */
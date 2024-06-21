#include <gtest/gtest.h>
#include "../include/GenerateFrames.h"
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>

/* Test class for GenerateFrames, which provides setup and teardown for CAN socket */
class GenerateFramesTest : public testing::Test {
protected:
	/* Set up a virtual CAN socket before each test */
    void SetUp() override {
        s = createSocket();
    }

	/* Close the CAN socket after each test */
    void TearDown() override {
        close(s);
    }

	/* Create and bind a CAN socket to the virtual CAN interface */
    int createSocket() {
        int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (s < 0) {
            std::cerr << "Error trying to create the socket\n";
            return -1;
        }

        struct sockaddr_can addr;
        struct ifreq ifr;

        strcpy(ifr.ifr_name, "vcan1");
        ioctl(s, SIOCGIFINDEX, &ifr);

        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Error binding\n";
            close(s);
            return -1;
        }

        return s;
    }

    int s;
};

/* Test the construction of a frame */
TEST(GenerateFramesSuite1, GenerateValidSocket) {
    const int sock = 123;

    GenerateFrames gen_frame(sock);

    EXPECT_EQ(gen_frame.getSocket(), sock);
}

/* Test the construction of a frame with an invalid socket */
TEST(GenerateFramesSuite1, GenerateInvalidSocket) {
    const int sock = -1;

    EXPECT_THROW(GenerateFrames frame(sock), std::invalid_argument);
}

/* Test the creation of data frame  */
TEST(GenerateFramesSuite2, CreateDataFrame) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> data = {0x2, 0x51, 0x3};
    int dlc = data.size();
    FrameType frame_type = FrameType::DATA_FRAME;
    
    GenerateFrames gen_frame;
    can_frame frame = gen_frame.createFrame(can_id, data, frame_type);

    EXPECT_EQ(frame.can_id, can_id);
    EXPECT_EQ(frame.can_dlc, dlc);
    for (int i = 0; i < frame.can_dlc; ++i) {
        EXPECT_EQ(frame.data[i], data[i]);
    }
}

/* Test the creation of remote frame */
TEST(GenerateFramesSuite2, CreateRemoteFrame) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> data = {0x2, 0x51, 0x3};
    int dlc = data.size();
    FrameType frame_type = FrameType::REMOTE_FRAME;
    
    GenerateFrames gen_frame;
    can_frame frame = gen_frame.createFrame(can_id, data, frame_type);

    EXPECT_EQ(frame.can_id, 0x40000101);
    EXPECT_EQ(frame.can_dlc, dlc);
    for (int i = 0; i < frame.can_dlc; ++i) {
        EXPECT_EQ(frame.data[i], data[i]);
    }
}

/* Test the creation of error frame */
TEST(GenerateFramesSuite2, CreateErrorFrame) {
    uint32_t can_id = 0x20;
    std::vector<uint8_t> data = {0x2, 0x51, 0x3};
    FrameType frame_type = FrameType::ERROR_FRAME;
    
    GenerateFrames gen_frame;
    can_frame frame = gen_frame.createFrame(can_id, data, frame_type);

    EXPECT_EQ(frame.can_id, CAN_ERR_FLAG);
    EXPECT_EQ(frame.can_dlc, 0);
}

/* Test the creation of overload frame */
TEST(GenerateFramesSuite2, CreateOverloadFrame) {
    uint32_t can_id = 0x20;
    std::vector<uint8_t> data = {0x2, 0x51, 0x3};
    FrameType frame_type = FrameType::OVERLOAD_FRAME;
    
    GenerateFrames gen_frame;
    can_frame frame = gen_frame.createFrame(can_id, data, frame_type);

    EXPECT_EQ(frame.can_id, CAN_ERR_FLAG);
    EXPECT_EQ(frame.can_dlc, 0);
}

/* Test send frame when socket is invalid */
TEST_F(GenerateFramesTest, SendInvalidSocketFrame) {
    uint32_t can_id = 0x123;
    std::vector<uint8_t> data = {0x2, 0x51, 0x3};
    FrameType frame_type = FrameType::DATA_FRAME;
    GenerateFrames gen_frame;

    EXPECT_THROW(gen_frame.sendFrame(can_id, data, frame_type), std::runtime_error);
}

/* Test successful send frame */
TEST_F(GenerateFramesTest, SendSuccessfulsendFrame) {
    uint32_t can_id = 0x123;
    std::vector<uint8_t> data = {0x2, 0x51, 0x3};
    FrameType frame_type = FrameType::DATA_FRAME;
    GenerateFrames gen_frame(s);

    gen_frame.sendFrame(can_id, data, frame_type);

    EXPECT_EQ(errno, 0);
}

/* Test send frame when write fails */
TEST_F(GenerateFramesTest, SendFailWriteOnSocket) {
    uint32_t can_id = 0x123;
    std::vector<uint8_t> data = {0x2, 0x51, 0x3};
    FrameType frame_type = FrameType::DATA_FRAME;
    GenerateFrames gen_frame(21);

    gen_frame.sendFrame(can_id, data, frame_type);

    EXPECT_NE(errno, 0);
}

/* Test getFrame */
TEST_F(GenerateFramesTest, GetFrame) {
    uint32_t can_id = 0x123;
    std::vector<uint8_t> data = {0x2, 0x51, 0x3};
    int dlc = data.size();
    FrameType frame_type = FrameType::DATA_FRAME;
    
    GenerateFrames gen_frame(s);
    can_frame frame = gen_frame.createFrame(can_id, data, frame_type);
    gen_frame.sendFrame(can_id, data, frame_type);

    EXPECT_EQ(gen_frame.getFrame().can_id, frame.can_id);
    EXPECT_EQ(gen_frame.getFrame().can_dlc, frame.can_dlc);
    for (int i = 0; i < gen_frame.getFrame().can_dlc; ++i) {
        EXPECT_EQ(gen_frame.getFrame().data[i], frame.data[i]);
    }
}

/* Test Session control response */
TEST_F(GenerateFramesTest, sessionControlResponse) {
    uint32_t can_id = 0x101;
    uint8_t subfunction = 0x01;
    bool response = 1;

    std::vector<uint8_t> data = {0x2, 0x50, subfunction};
    GenerateFrames gen_frame(s);
    gen_frame.sessionControl(can_id, subfunction, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Session control request */
TEST_F(GenerateFramesTest, sessionControlRequest) {
    uint32_t can_id = 0x101;
    uint8_t subfunction = 0x01;
    bool response = 0;

    std::vector<uint8_t> data = {0x2, 0x10, subfunction};

    GenerateFrames gen_frame(s);
    gen_frame.sessionControl(can_id, subfunction, response);

    EXPECT_EQ(gen_frame.getFrame().can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test ECU reset response */
TEST_F(GenerateFramesTest, ecuResetResponse) {
    uint32_t can_id = 0x101;
    bool response = 1;

    std::vector<int> data = {0x2, 0x51, 0x3};

    GenerateFrames gen_frame(s);
    gen_frame.ecuReset(can_id, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test ECU reset request */
TEST_F(GenerateFramesTest, ecuResetRequest) {
    uint32_t can_id = 0x101;
    int subfunction = 0x01;
    bool response = 0;

    std::vector<int> data = {0x2, 0x11, 0x3};

    GenerateFrames gen_frame(s);
    gen_frame.ecuReset(can_id, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read Data By Identifier request */
TEST_F(GenerateFramesTest, readDataByIdentifierRequest) {
    uint32_t can_id = 0x101;
    uint8_t data_identifier = 0x01;
    std::vector<uint8_t> response;

    std::vector<uint8_t> data = {0x3, 0x22, static_cast<uint8_t>(data_identifier/0x100), static_cast<uint8_t>(data_identifier%0x100)};

    GenerateFrames gen_frame(s);
    gen_frame.readDataByIdentifier(can_id, data_identifier, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read Data By Identifier response */
TEST_F(GenerateFramesTest, readDataByIdentifierResponse) {
    uint32_t can_id = 0x123;
    uint8_t data_identifier = 0x01;
    std::vector<uint8_t> response = {0x2};

    std::vector<uint8_t> data = {static_cast<uint8_t>(response.size() + 3), 0x62, static_cast<uint8_t>(data_identifier/0x100), 
                                 static_cast<uint8_t>(data_identifier%0x100), 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.readDataByIdentifier(can_id, data_identifier, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read Data By Identifier response too long */
TEST_F(GenerateFramesTest, RDBIResponseTooLarge) {
    uint32_t can_id = 0x123;
    uint8_t data_identifier = 0x01;    
    std::vector<uint8_t> response = {0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);

    testing::internal::CaptureStdout();
    gen_frame.readDataByIdentifier(can_id, data_identifier, response);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Response size is too large\n"), std::string::npos);
}

/* Test Create Frame Long first frame */
TEST_F(GenerateFramesTest, createFrameLongFirstFrame) {
    uint32_t can_id = 0x101;
    uint8_t sid = 0x50;
    uint8_t data_identifier = 0x01;  
    std::vector<uint8_t> response = {0x2, 0x2, 0x2};
    bool first_frame = true;

    std::vector<uint8_t> data = {0x10, static_cast<uint8_t>(response.size() + 3), sid, 
                                static_cast<uint8_t>(data_identifier/0x100), static_cast<uint8_t>(data_identifier%0x100), 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.createFrameLong(can_id, sid, data_identifier, response, first_frame);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Create Frame Long not first frame */
TEST_F(GenerateFramesTest, createFrameLongNotFirstFrame) {
    uint32_t can_id = 0x101;
    uint8_t sid = 0x50;
    uint8_t data_identifier = 0x01;  
    std::vector<uint8_t> response = {0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2};
    bool first_frame = false;

    std::vector<uint8_t> data = {0x21, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    testing::internal::CaptureStdout();
    gen_frame.createFrameLong(can_id, sid, data_identifier, response, first_frame);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc - 1; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }

}

/* Test Read Data By Identifier Long */
TEST_F(GenerateFramesTest, readDataByIdentifierLong) {
    uint32_t can_id = 0x101;
    uint8_t data_identifier = 0x01;  
    std::vector<uint8_t> response = {0x2, 0x2, 0x2};
    bool first_frame = true;

    std::vector<uint8_t> data = {0x10, static_cast<uint8_t>(response.size() + 3), 0x62, 
                                static_cast<uint8_t>(data_identifier/0x100), static_cast<uint8_t>(data_identifier%0x100), 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.readDataByIdentifierLong(can_id, data_identifier, response, first_frame);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Flow Control Frame */
TEST_F(GenerateFramesTest, flowControlFrame) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> data = {0x30, 0x00, 0x00, 0x00};

    GenerateFrames gen_frame(s);
    gen_frame.flowControlFrame(can_id);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test authentication Request Seed Empty */
TEST_F(GenerateFramesTest, authenticationRequestSeedEmpty) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> seed;
    std::vector<uint8_t> data = {0x3, 0x29, 0x1};

    GenerateFrames gen_frame(s);
    gen_frame.authenticationRequestSeed(can_id, seed);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test authentication Request Seed Not Empty */
TEST_F(GenerateFramesTest, authenticationRequestSeedNotEmpty) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> seed = {0x2, 0x2, 0x2};
    std::vector<uint8_t> data = {static_cast<uint8_t>(seed.size() + 2), 0x69, 0x1, 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.authenticationRequestSeed(can_id, seed);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test authentication Send Key Empty */
TEST_F(GenerateFramesTest, authenticationSendKeyEmpty) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> key;
    std::vector<uint8_t> data = {0x02, 0x69, 0x02};

    GenerateFrames gen_frame(s);
    gen_frame.authenticationSendKey(can_id, key);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test authentication Send Key Not Empty */
TEST_F(GenerateFramesTest, authenticationSendKeyNotEmpty) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> key = {0x2, 0x2, 0x2};
    std::vector<uint8_t> data = {static_cast<uint8_t>(key.size() + 2), 0x29, 0x2, 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.authenticationSendKey(can_id, key);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Routine Control response */
TEST_F(GenerateFramesTest, routineControlResponse) {
    uint32_t can_id = 0x101;
    uint8_t subfunction = 0x01;
    uint8_t routine_identifier = 0x02;
    bool response = 1;

    std::vector<uint8_t> data = {0x4, 0x71, subfunction, static_cast<uint8_t>(routine_identifier / 0x100), 
                                static_cast<uint8_t>(routine_identifier % 0x100)};

    GenerateFrames gen_frame(s);
    gen_frame.routineControl(can_id, subfunction, routine_identifier, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Routine Control request */
TEST_F(GenerateFramesTest, routineControlRequest) {
    uint32_t can_id = 0x101;
    uint8_t subfunction = 0x01;
    uint8_t routine_identifier = 0x02;
    bool response = 0;

    std::vector<uint8_t> data = {0x4, 0x31, subfunction, static_cast<uint8_t>(routine_identifier / 0x100), 
                                 static_cast<uint8_t>(routine_identifier % 0x100)};

    GenerateFrames gen_frame(s);
    gen_frame.routineControl(can_id, subfunction, routine_identifier, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Tester Present true */
TEST_F(GenerateFramesTest, testerPresentTrue) {
    uint32_t can_id = 0x101;
    bool response = 1;

    std::vector<uint8_t> data = {0x02, 0x7E, 0x00};

    GenerateFrames gen_frame(s);
    gen_frame.testerPresent(can_id, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Tester Present false */
TEST_F(GenerateFramesTest, testerPresentFalse) {
    uint32_t can_id = 0x101;
    bool response = 0;

    std::vector<uint8_t> data = {0x2, 0x3E, 0x00};

    GenerateFrames gen_frame(s);
    gen_frame.testerPresent(can_id, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read Memory By Address response empty */
TEST_F(GenerateFramesTest, readMemoryByAddressResponseEmpty) {
    uint32_t can_id = 0x07;
    uint8_t memory_size = static_cast<uint8_t>(0x2345);
    uint8_t memory_address = 0x01;
    std::vector<uint8_t> response = {};

    std::vector<uint8_t> data = {0x03,0x23,0x10,0x45};

    GenerateFrames gen_frame(s);
    gen_frame.readMemoryByAddress(can_id, memory_address, memory_size, response);


    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read Memory By Address response not emtpy */
TEST_F(GenerateFramesTest, readMemoryByAddressResponseNotEmpty) {
    uint32_t can_id = 0x07;
    uint8_t memory_size = static_cast<uint8_t>(0x2345);
    uint8_t memory_address = 0x01;
    std::vector<uint8_t> response = {0x2, 0x2, 0x2, 0x2};

    std::vector<uint8_t> data = {0x07, 0x63, 0x10, 0x45, 0x2, 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.readMemoryByAddress(can_id, memory_address, memory_size, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc - 1; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read Memory By Address response too large */
TEST_F(GenerateFramesTest, readMemoryByAddressResponseTooLarge) {
    uint32_t can_id = 0x07;
    uint8_t memory_size = static_cast<uint8_t>(0x2345);
    uint8_t memory_address = 0x01;
    std::vector<uint8_t> response = {0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2};
    GenerateFrames gen_frame(s);

    testing::internal::CaptureStdout();
    gen_frame.readMemoryByAddress(can_id, memory_address, memory_size, response);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Response size is too large.\n"), std::string::npos);
}

/* Test Read Memory By Address Long first frame */
TEST_F(GenerateFramesTest, readMemoryByAddressLongFirstFrame) {
    uint32_t can_id = 0x07;
    uint8_t memory_size = static_cast<uint8_t>(0x2345);
    uint8_t memory_address = 0x01;
    std::vector<uint8_t> response = {0x2, 0x2, 0x2};
    bool first_frame = true;
    std::vector<uint8_t> data = {0x10,0x6,0x63,0x10,0x45,0x2,0x2,0x2};

    GenerateFrames gen_frame(s);
    gen_frame.readMemoryByAddressLong(can_id, memory_address, memory_size, response, first_frame);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read Memory By Address Long not first frame */
TEST_F(GenerateFramesTest, readMemoryByAddressLongNotFirstFrame) {
    uint32_t can_id = 0x07;
    uint8_t memory_size = static_cast<uint8_t>(0x2345);
    uint8_t memory_address = 0x01;
    std::vector<uint8_t> response = {0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2}; 
    bool first_frame = false;
    std::vector<uint8_t> data = {0x21,0x2,0x2,0x2,0x2,0x2,0x2,0x2};

    GenerateFrames gen_frame(s);
    gen_frame.readMemoryByAddressLong(can_id, memory_address, memory_size, response, first_frame);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }

}

/* Test Write Data By Identifier. Data Parameter Empty*/
TEST_F(GenerateFramesTest, writeDataByIdentifierEmpty) {
    uint32_t can_id = 0x101;
    uint8_t identifier = 0x20;
    std::vector<uint8_t> data_parameter;

    std::vector<uint8_t> data = {0x03, 0x6E, static_cast<uint8_t>(identifier / 0x100), 
                                static_cast<uint8_t>(identifier % 0x100)};

    GenerateFrames gen_frame(s);
    gen_frame.writeDataByIdentifier(can_id, identifier, data_parameter);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Write Data By Identifier. Data Parameter not empty*/
TEST_F(GenerateFramesTest, writeDataByIdentifierNotEmpty) {
    uint32_t can_id = 0x101;
    uint8_t identifier = 0x20;
    std::vector<uint8_t> data_parameter = {0x2, 0x2, 0x2};

    std::vector<uint8_t> data = {static_cast<uint8_t>(data_parameter.size() + 3), 0x2E, static_cast<uint8_t>(identifier / 0x100), 
                                static_cast<uint8_t>(identifier % 0x100), 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.writeDataByIdentifier(can_id, identifier, data_parameter);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Write Data By Identifier. Data Parameter too large */
TEST_F(GenerateFramesTest, writeDataByIdentifierTooLarge) {
    uint32_t can_id = 0x101;
    uint8_t identifier = 0x20;
    std::vector<uint8_t> data_parameter = {0x2, 0x2, 0x2, 0x2, 0x2, 0x2};

    testing::internal::CaptureStdout();
    GenerateFrames gen_frame(s);
    gen_frame.writeDataByIdentifier(can_id, identifier, data_parameter);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Data parameter size is too large\n"), std::string::npos);
}

/* Test Write Data By Identifier Long */
TEST_F(GenerateFramesTest, writeDataByIdentifierLong) {
    uint32_t can_id = 0x101;
    uint8_t identifier = 0x02;
    std::vector<uint8_t> data_parameter = {0x2, 0x2, 0x2};
    bool first_frame = true;

    std::vector<uint8_t> data = {0x10, static_cast<uint8_t>(data_parameter.size() + 3), 0x2E, static_cast<uint8_t>(identifier/0x100), 
                                static_cast<uint8_t>(identifier%0x100), 0x2, 0x2 ,0x2};

    GenerateFrames gen_frame(s);
    gen_frame.writeDataByIdentifierLong(can_id, identifier, data_parameter, first_frame);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read DTC Information */
TEST_F(GenerateFramesTest, readDTCInformation) {
    uint32_t can_id = 0x101;
    uint8_t subfunction = 0x01;
    uint8_t dtc_status_mask = 0x02;

    std::vector<uint8_t> data = {0x03, 0x19, subfunction, dtc_status_mask};

    GenerateFrames gen_frame(s);
    gen_frame.readDtcInformation(can_id, subfunction, dtc_status_mask);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Read DTC Information Response 01 */
TEST_F(GenerateFramesTest, readDTCInformationResponse01) {
    uint32_t can_id = 0x101;
    uint8_t status_availability_mask = 0x01;
    uint8_t dtc_format_identifier = 0x02;
    uint8_t dtc_count = 0x03;

    std::vector<uint8_t> data = {0x03, 0x59, 0x01, status_availability_mask, dtc_format_identifier, dtc_count};

    GenerateFrames gen_frame(s);
    gen_frame.readDtcInformationResponse01(can_id, status_availability_mask, dtc_format_identifier, dtc_count);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Clear Diagnostic Information Response */
TEST_F(GenerateFramesTest, clearDiagnosticInformationResponse) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> group_of_dtc;
    bool response = true;

    std::vector<uint8_t> data = {0x01, 0x54};

    GenerateFrames gen_frame(s);
    gen_frame.clearDiagnosticInformation(can_id, group_of_dtc, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Clear Diagnostic Information Request */
TEST_F(GenerateFramesTest, clearDiagnosticInformationRequest) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> group_of_dtc = {0x2, 0x2};
    bool response = false;

    std::vector<uint8_t> data = {static_cast<uint8_t>(group_of_dtc.size() + 1), 0x14, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.clearDiagnosticInformation(can_id, group_of_dtc, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Clear Diagnostic Information DTC size too large */
TEST_F(GenerateFramesTest, clearDiagnosticInformationDTCTooLarge) {
    uint32_t can_id = 0x101;
    std::vector<uint8_t> group_of_dtc = {0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2};
    bool response = false;

    testing::internal::CaptureStdout();
    GenerateFrames gen_frame(s);
    gen_frame.clearDiagnosticInformation(can_id, group_of_dtc, response);

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Group of DTC size is too large.\n"), std::string::npos);
}

/* Test Access Timing Parameters Response */
TEST_F(GenerateFramesTest, accessTimingParametersResponse) {
    uint32_t can_id = 0x101;
    uint8_t subfunction = 0x02;
    bool response = true;

    std::vector<uint8_t> data = {0x02, 0xC3, subfunction};

    GenerateFrames gen_frame(s);
    gen_frame.accessTimingParameters(can_id, subfunction, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Access Timing Parameters Request */
TEST_F(GenerateFramesTest, accessTimingParametersRequest) {
    uint32_t can_id = 0x101;
    uint8_t subfunction = 0x02;
    bool response = false;

    std::vector<uint8_t> data = {0x02, 0x83, subfunction};

    GenerateFrames gen_frame(s);
    gen_frame.accessTimingParameters(can_id, subfunction, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Negative Response */
TEST_F(GenerateFramesTest, negativeResponse) {
    uint32_t can_id = 0x123;
    uint8_t sid = 0x01;
    uint8_t nrc = 0x22;

    std::vector<uint8_t> data = {0x03, 0x7F, sid, nrc};

    GenerateFrames gen_frame(s);
    gen_frame.negativeResponse(can_id, sid, nrc);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Request Download */
TEST_F(GenerateFramesTest, requestDownload) {
    uint32_t can_id = 0x07;
    uint8_t data_format_identifier = 0x00;
    uint8_t memory_size = 0x10;
    uint8_t memory_address = static_cast<uint8_t>(0x3445);

    std::vector<uint8_t> data = {0x05, 0x34, 0x00, 0x11, 0x45, 0x10};

    GenerateFrames gen_frame(s);
    gen_frame.requestDownload(can_id, data_format_identifier, memory_address, memory_size);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Request Download Response */
TEST_F(GenerateFramesTest, requestDownloadResponse) {
    uint32_t can_id = 0x07;
    uint8_t max_number_block = static_cast<uint8_t>(0x2234);
    std::vector<uint8_t> data = {0x03, 0x74, 0x10, 0x34};

    GenerateFrames gen_frame(s);
    gen_frame.requestDownloadResponse(can_id, max_number_block);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test transfer Data when transfer request is empty */
TEST_F(GenerateFramesTest, transferDataEmpty) {
    uint32_t can_id = 0x123;
    uint8_t block_sequence_counter = 0x30;
    std::vector<uint8_t> transfer_request;

    std::vector<uint8_t> data = {0x02, 0x76, block_sequence_counter};

    GenerateFrames gen_frame(s);
    gen_frame.transferData(can_id, block_sequence_counter, transfer_request);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test transfer Data when transfer request is not empty */
TEST_F(GenerateFramesTest, transferDataNotEmpty) {
    uint32_t can_id = 0x123;
    uint8_t block_sequence_counter = 0x30;
    std::vector<uint8_t> transfer_request = {0x2, 0x2, 0x2};

    std::vector<uint8_t> data = {static_cast<uint8_t>(transfer_request.size() + 2), 0x36, block_sequence_counter, 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.transferData(can_id, block_sequence_counter, transfer_request);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test transfer Data Long first frame */
TEST_F(GenerateFramesTest, transferDataLongFirstFrame) {
    uint32_t can_id = 0x101;
    uint8_t block_sequence_counter = 0x03; 
    std::vector<uint8_t> transfer_request = {0x2, 0x2, 0x2, 0x2};
    bool first_frame = true;

    std::vector<uint8_t> data = {0x10, static_cast<uint8_t>(transfer_request.size() + 2), 0x36, block_sequence_counter, 0x2, 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.transferDataLong(can_id, block_sequence_counter, transfer_request, first_frame);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test transfer Data Long not first frame */
TEST_F(GenerateFramesTest, transferDataLongNotFirstFrame) {
    uint32_t can_id = 0x101;
    uint8_t block_sequence_counter = 0x03; 
    std::vector<uint8_t> transfer_request = {0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2};  
    bool first_frame = false;

    std::vector<uint8_t> data = {0x21, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2};

    GenerateFrames gen_frame(s);
    gen_frame.transferDataLong(can_id, block_sequence_counter, transfer_request, first_frame);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Request transfer Exit Response */
TEST_F(GenerateFramesTest, requestTransferExitResponse) {
    uint32_t can_id = 0x101;
    bool response = true;

    std::vector<uint8_t> data = {0x01, 0x77};

    GenerateFrames gen_frame(s);
    gen_frame.requestTransferExit(can_id, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

/* Test Request transfer Exit Request */
TEST_F(GenerateFramesTest, requestTransferExitRequest) {
    uint32_t can_id = 0x101;
    bool response = false;

    std::vector<uint8_t> data = {0x01, 0x37};

    GenerateFrames gen_frame(s);
    gen_frame.requestTransferExit(can_id, response);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

TEST_F(GenerateFramesTest, apiResponse) {
    uint32_t can_id = 0x1015;
    uint32_t api_id = 0x15;
    uint8_t sid = 0xD9;
    uint8_t battery_id = 0x11;
    uint8_t doors_id = 0x12;
    uint8_t engine_ide = 0x13;
    std::vector<uint8_t> data = {0x06, sid, MCU_ID, battery_id, doors_id, engine_ide};

    GenerateFrames gen_frame(s);
    gen_frame.apiResponse(api_id);

    EXPECT_EQ(gen_frame.frame.can_id, can_id);
    for (int i = 0; i < gen_frame.frame.can_dlc; ++i) {
        EXPECT_EQ(gen_frame.frame.data[i], data[i]);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#include <gtest/gtest.h>
#include "../include/RequestTransferExit.h"

/* Class fixture for general tests */
class RequestTransferExitTest : public ::testing::Test
{
protected:
    Logger logger;
    RequestTransferExit* transfer_exit;
    RequestTransferExitTest()
    {
    transfer_exit = new RequestTransferExit(logger);
    }
};

TEST_F(RequestTransferExitTest, TransferSuccesNoCallbackSet)
{
    /* simulate a successfull transfer data */
    bool transfer_success =  true;
    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);
    /* should stop as no callback is set */
    EXPECT_FALSE(result);
}

TEST_F(RequestTransferExitTest, TransferFailNoCallbackSet)
{
    /* simulate a failed transfer data*/
    bool transfer_success =  false;
    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);
    /* should stop as no callback is set */
    EXPECT_FALSE(result);
}

TEST_F(RequestTransferExitTest, TransferSuccesCallbackSet)
{
    /* simulate a successfull transfer data*/
    bool transfer_success =  true;
    /** Set a lambda function as the callback for the RequestTransferExit class that takes 
    *   one parameter of type boolean named and the body of lambda simply returns the success parameter
    */
    transfer_exit->setTransferCompleteCallBack([](bool success) { return success; });

    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);

    EXPECT_TRUE(result);
}

TEST_F(RequestTransferExitTest, TransferFailCallbackSet)
{
   /* simulate a failed transfer data*/
    bool transfer_success =  false;
    transfer_exit->setTransferCompleteCallBack([](bool success) { return success; });

    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);

    EXPECT_FALSE(result); 
}

TEST_F(RequestTransferExitTest, CallbackNullptr)
{
    /* simulate a successfull transfer data*/
    bool transfer_success =  true;
    /* Explicitly set the callback nullptr */
    transfer_exit->setTransferCompleteCallBack(nullptr);

    bool result = transfer_exit->requestTransferExit(0x10, transfer_success);
    /* should stop the transfer as callback is nullptr */
    EXPECT_FALSE(result); 
}



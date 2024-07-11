#include <gtest/gtest.h>
#include "../include/RequestTransferExit.h"

/* Class fixture for general tests */
class RequestTransferExitTest : public ::testing::Test
{
protected:
    Logger logger;
    RequestTransferExit* transferExit;
    RequestTransferExitTest()
    {
    transferExit = new RequestTransferExit(logger);
    }
};

TEST_F(RequestTransferExitTest, TransferSuccesNoCallbackSet)
{
    /* simulate a successfull transfer data */
    bool transferSuccess =  true;
    bool result = transferExit->requestTransferExit(0x10, transferSuccess);
    /* should stop as no callback is set */
    EXPECT_FALSE(result);
}

TEST_F(RequestTransferExitTest, TransferFailNoCallbackSet)
{
    /* simulate a failed transfer data*/
    bool transferSuccess =  false;
    bool result = transferExit->requestTransferExit(0x10, transferSuccess);
    /* should stop as no callback is set */
    EXPECT_FALSE(result);
}

TEST_F(RequestTransferExitTest, TransferSuccesCallbackSet)
{
    /* simulate a successfull transfer data*/
    bool transferSuccess =  true;
    /** Set a lambda function as the callback for the RequestTransferExit class that takes 
    *   one parameter of type boolean named and the body of lambda simply returns the success parameter
    */
    transferExit->setTransferCompleteCallBack([](bool success) { return success; });

    bool result = transferExit->requestTransferExit(0x10, transferSuccess);
    /*  */
    EXPECT_TRUE(result);
}

TEST_F(RequestTransferExitTest, TransferFailCallbackSet)
{
   /* simulate a failed transfer data*/
    bool transferSuccess =  false;
    transferExit->setTransferCompleteCallBack([](bool success) { return success; });

    bool result = transferExit->requestTransferExit(0x10, transferSuccess);
    /*  */
    EXPECT_FALSE(result); 
}

TEST_F(RequestTransferExitTest, CallbackNullptr)
{
    /* simulate a successfull transfer data*/
    bool transferSuccess =  true;
    /* Explicitly set the callback nullptr */
    transferExit->setTransferCompleteCallBack(nullptr);

    bool result = transferExit->requestTransferExit(0x10, transferSuccess);
    /* should stop the transfer as callback is nullptr */
    EXPECT_FALSE(result); 
}



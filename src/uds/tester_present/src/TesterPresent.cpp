#include "../include/TesterPresent.h"

TesterPresent::TesterPresent(Logger logger, int socket, int timeout_duration)
    : logger(logger), socket(socket), timeout_duration(timeout_duration)
{
    this->generate = new GenerateFrames(socket, logger);
    startTimerThread();
}

TesterPresent::TesterPresent()
    : logger(Logger("log_test_tester_present", "./log_test_tester_present.log")), socket(-1), timeout_duration(5), running(true)
{

}

TesterPresent::~TesterPresent()
{
    stopTimerThread();
    delete this->generate;
}

void TesterPresent::handleTesterPresent(uint32_t can_id, std::vector<uint8_t> data)
{
    if (data.size() != 3)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Incorrect message length");
        this->generate->negativeResponse(can_id, 0x3E, 0x13);
        return;
    }

    uint8_t sub_function = data[2];

    if (sub_function != 0x00)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Sub-function not supported");
        this->generate->negativeResponse(can_id, 0x3E, 0x12);
        return;
    }

    LOG_INFO(logger.GET_LOGGER(), "Tester Present Service Received");

    /* Send positive response */
    this->generate->testerPresent(can_id, true);

    /* Reset the S3 timer */
    running = false;
    startTimerThread();
}

void TesterPresent::startTimerThread()
{
    running = true;
    timerThread = std::thread(&TesterPresent::timerFunction, this);
}

void TesterPresent::stopTimerThread()
{
    running = false;
    if (timerThread.joinable())
    {
        timerThread.join();
    }
}

void TesterPresent::timerFunction()
{
    auto start = std::chrono::system_clock::now();
    while (running)
    {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - start;

        if (elapsed_seconds.count() >= timeout_duration)
        {
            LOG_WARN(logger.GET_LOGGER(), "S3 timer expired. Returning to default session.");
            /* Handle returning to default session here */

            running = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

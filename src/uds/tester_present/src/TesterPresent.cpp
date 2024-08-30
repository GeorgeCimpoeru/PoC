#include "../include/TesterPresent.h"
#include "../../../ecu_simulation/BatteryModule/include/BatteryModule.h"
#include "../../../ecu_simulation/EngineModule/include/EngineModule.h"
#include "../../../mcu/include/MCUModule.h"

TesterPresent::TesterPresent(Logger& logger, DiagnosticSessionControl* sessionControl, int socket, int timeout_duration):
    logger(logger), sessionControl(sessionControl), socket(socket), timeout_duration(timeout_duration)
{
    this->generate = new GenerateFrames(socket, logger);
    startTimerThread();
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
        uint8_t receiver_id = can_id & 0xFF;
        if (receiver_id == 0x10)
        {
            MCU::mcu->stop_flags[0x3E] = false;
        } else if (receiver_id == 0x11)
        {
            battery->stop_flags[0x3E] = false;
        } else if (receiver_id == 0x12)
        {
            battery->stop_flags[0x3E] = false;
        }
        return;
    }

    uint8_t sub_function = data[2];

    if (sub_function != 0x00)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Sub-function not supported");
        this->generate->negativeResponse(can_id, 0x3E, 0x12);
                uint8_t receiver_id = can_id & 0xFF;
        if (receiver_id == 0x10)
        {
            MCU::mcu->stop_flags[0x3E] = false;
        } else if (receiver_id == 0x11)
        {
            battery->stop_flags[0x3E] = false;
        } else if (receiver_id == 0x12)
        {
            battery->stop_flags[0x3E] = false;
        }
        return;
    }

    /* set the session to programming session */
    sessionControl->sessionControl(can_id, 2);

    LOG_INFO(logger.GET_LOGGER(), "Tester Present Service Received");

    uint8_t receiver_id = can_id & 0xFF;
    switch(receiver_id)
    {
        case 0x10:
            if (MCU::mcu != nullptr && MCU::mcu->stop_flags.find(0x3E) != MCU::mcu->stop_flags.end())
            {
                if (MCU::mcu->stop_flags.find(0x3E) != MCU::mcu->stop_flags.end())
                {
                    /* Send response frame */
                    this->generate->testerPresent(can_id, true);
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x3E);
                    MCU::mcu->stop_flags[0x3E] = false;
                } 
                else
                {
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x3E);
                    NegativeResponse negative_response(socket, logger);
                    negative_response.sendNRC(can_id, 0x3E, 0x78);
                }
            } 
            else
            {
                LOG_ERROR(logger.GET_LOGGER(), "MCU::mcu is null or flag not found.");
            }
            break;
        case 0x11:
            if (battery!= nullptr && battery->stop_flags.find(0x3E) != battery->stop_flags.end())
            {
                if (battery->stop_flags.find(0x3E) != battery->stop_flags.end())
                {
                    /* Send response frame */
                    this->generate->testerPresent(can_id, true);
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x3E);
                    battery->stop_flags[0x3E] = false;
                } 
                else
                {
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x3E);
                    NegativeResponse negative_response(socket, logger);
                    negative_response.sendNRC(can_id, 0x3E, 0x78);
                }
            } 
            else
            {
                LOG_ERROR(logger.GET_LOGGER(), "MCU::mcu is null or flag not found.");
            }
            break;
        case 0x12:
            if (engine!= nullptr && engine->stop_flags.find(0x3E) != engine->stop_flags.end())
            {
                if (engine->stop_flags.find(0x3E) != engine->stop_flags.end())
                {
                    /* Send response frame */
                    this->generate->testerPresent(can_id, true);
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} successfully sent the response frame.", 0x3E);
                    engine->stop_flags[0x3E] = false;
                } 
                else
                {
                    LOG_INFO(logger.GET_LOGGER(), "Service with SID {:x} failed to send the response frame.", 0x3E);
                    NegativeResponse negative_response(socket, logger);
                    negative_response.sendNRC(can_id, 0x3E, 0x78);
                }
            } 
            else
            {
                LOG_ERROR(logger.GET_LOGGER(), "MCU::mcu is null or flag not found.");
            }
            break;
        default:
            LOG_ERROR(logger.GET_LOGGER(), "Module with id {:x} not supported.", receiver_id);
    }

    /* Reset the S3 timer */
    running = false;
    startTimerThread();

    if (sessionControl->getCurrentSession() != DEFAULT_SESSION)
    {
        LOG_INFO(logger.GET_LOGGER(), "Session maintained. Current session: {}", sessionControl->getCurrentSessionToString());
    }
}

void TesterPresent::startTimerThread()
{
    LOG_INFO(logger.GET_LOGGER(), "Attempting to start the timer thread.");

    if (running)
    {
        LOG_WARN(logger.GET_LOGGER(), "Timer thread already running. Aborting start.");
        return;
    }

    try
    {
        running = true;
        timerThread = std::thread(&TesterPresent::timerFunction, this);

        LOG_INFO(logger.GET_LOGGER(), "Timer thread started successfully.");
    }
    catch (const std::exception &ex)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Failed to start timer thread: {}", ex.what());
        running = false;
    }
    catch (...)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Unknown error occurred while starting timer thread.");
        running = false;
    }
}

void TesterPresent::timerFunction()
{
    try
    {
        auto start = std::chrono::steady_clock::now();
        LOG_WARN(logger.GET_LOGGER(), "Starting S3 timer for {} seconds.", timeout_duration);
        while (running)
        {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = now - start;

            if (elapsed_seconds.count() >= timeout_duration)
            {
                LOG_WARN(logger.GET_LOGGER(), "S3 timer expired. Returning to default session.");

                /* Handle returning to default session here */
                sessionControl->sessionControl(0x3E, 1);         

                running = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (const std::exception &ex)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Exception in timer thread: {}", ex.what());
        running = false;
    }
    catch (...)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Unknown exception in timer thread.");
        running = false;
    }
}

void TesterPresent::stopTimerThread()
{
    LOG_INFO(logger.GET_LOGGER(), "Attempting to stop the timer thread.");

    running = false;

    try
    {
        if (timerThread.joinable())
        {
            timerThread.join();
            LOG_INFO(logger.GET_LOGGER(), "Timer thread stopped successfully.");
        }
        else
        {
            LOG_WARN(logger.GET_LOGGER(), "Timer thread was not joinable, skipping join.");
        }
    }
    catch (const std::exception &ex)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Failed to stop timer thread: {}", ex.what());
    }
    catch (...)
    {
        LOG_ERROR(logger.GET_LOGGER(), "Unknown error occurred while stopping timer thread.");
    }
}
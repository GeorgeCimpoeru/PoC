#include "../include/BatteryModule.h"

int main()
{
  BatteryModule batteryModule;
  batteryModule.fetchBatteryData();

  std::thread receiveFrThread([&batteryModule]()
                              { batteryModule.receiveFrames(); 
                                batteryModule.startNotificationCheck(); });
  sleep(8);

  batteryModule.stopFrames();
  receiveFrThread.join();
  return 0;
}

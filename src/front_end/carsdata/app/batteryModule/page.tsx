import React from 'react';
import PercentageCard from '../components/batteryComponents/PercentageCard';
import VoltageCard from '../components/batteryComponents/VoltageCard';
import BatteryStateCard from '../components/batteryComponents/BatteryStateCard';
import TemperatureCard from '../components/batteryComponents/TemperatureCard';
import LifeCycleCard from '../components/batteryComponents/LifeCycleCard';
import FullyChargedCard from '../components/batteryComponents/FullyChargedCard';
import SerialNumberCard from '../components/batteryComponents/SerialNumberCard';
import RangeBatteryCard from '../components/batteryComponents/RangeBatteryCard';
import ChargingTimeCard from '../components/batteryComponents/Charging_time';
import DeviceConsumptionCard from '../components/batteryComponents/DeviceConsumption';
import { write } from 'fs';



const BatteryModule = async () => {
    const res = await fetch(
        'http://127.0.0.1:5000/api/read_info_battery',
        { cache: 'no-store' }
    );
   
    const jsonData = await res.json();

    return (
        <div className="h-screen flex flex-col">
            <nav className="bg-blue-900 p-8 flex justify-between items-center">
                <div className="text-white text-2xl font-bold">Battery Module</div>
            </nav>
            <div className='m-20'>
                <div className=" flex items-center justify-center">
                    <table>
                        <tr>
                            <td>
                                <PercentageCard percentageBattery={jsonData.battery_level}></PercentageCard>
                            </td>
                            <td>
                                <VoltageCard voltageCard={jsonData.voltage}></VoltageCard>
                            </td>
                            <td>
                                <BatteryStateCard BatteryStateOfCharge={jsonData.battery_state_of_charge}></BatteryStateCard>
                            </td>
                            <td>
                                <TemperatureCard temperature={jsonData.temperature}></TemperatureCard>
                            </td> 
                        </tr>
                        <tr>
                            <td>
                                <LifeCycleCard life_cycle={jsonData.life_cycle} ></LifeCycleCard>
                            </td>
                            <td>
                                <FullyChargedCard fully_charged={jsonData.fully_charged}></FullyChargedCard>
                            </td>
                            <td>
                                <SerialNumberCard serialNumber={jsonData.serial_number}></SerialNumberCard>
                            </td>
                            <td>
                                <RangeBatteryCard range_battery={jsonData.range_battery}></RangeBatteryCard>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <ChargingTimeCard chargingTime={jsonData.charging_time}></ChargingTimeCard>
                            </td>
                            {/* <td>
                                <DeviceConsumptionCard deviceConsumption={jsonData.device_consumption}></DeviceConsumptionCard>
                            </td> */}
                        </tr>

                    </table>
                </div>
            </div >
        </div >
    )
}

export default BatteryModule
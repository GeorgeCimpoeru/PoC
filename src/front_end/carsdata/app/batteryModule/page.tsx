import React from 'react'
import PercentageCard from '../components/batteryComponents/PercentageCard'
import VoltageCard from '../components/batteryComponents/VoltageCard'
import BatteryStateCard from '../components/batteryComponents/BatteryStateCard'
import TemperatureCard from '../components/batteryComponents/TemperatureCard'
import LifeCycleCard from '../components/batteryComponents/LifeCycleCard'
import FullyChargedCard from '../components/batteryComponents/FullyChargedCard'
import SerialNumberCard from '../components/batteryComponents/SerialNumberCard'
import RangeBatteryCard from '../components/batteryComponents/RangeBatteryCard'
import ChargingTimeCard from '../components/batteryComponents/Charging_time'
import DeviceConsumptionCard from '../components/batteryComponents/DeviceConsumption'



const BatteryModule = async () => {
    const res = await fetch(
        'https://pocapi.pythonanywhere.com/api?service=battery_module',
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
                                <PercentageCard percentageBattery={jsonData.query_params.battery_level}></PercentageCard>
                            </td>
                            <td>
                                <VoltageCard voltageCard={jsonData.query_params.voltage}></VoltageCard>
                            </td>
                            <td>
                                <BatteryStateCard BatteryStateOfCharge={jsonData.query_params.battery_state_of_charge}></BatteryStateCard>
                            </td>
                            <td>
                                <TemperatureCard temperature={jsonData.query_params.temperature}></TemperatureCard>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <LifeCycleCard life_cycle={jsonData.query_params.life_cycle} ></LifeCycleCard>
                            </td>
                            <td>
                                <FullyChargedCard fully_charged={jsonData.query_params.fully_charged}></FullyChargedCard>
                            </td>
                            <td>
                                <SerialNumberCard serialNumber={jsonData.query_params.serial_number}></SerialNumberCard>
                            </td>
                            <td>
                                <RangeBatteryCard range_battery={jsonData.query_params.range_battery}></RangeBatteryCard>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <ChargingTimeCard chargingTime={jsonData.query_params.charging_time}></ChargingTimeCard>
                            </td>
                            <td>
                                <DeviceConsumptionCard deviceConsumption={jsonData.query_params.device_consumption}></DeviceConsumptionCard>
                            </td>
                        </tr>

                    </table>
                </div>
            </div >
        </div >
    )
}

export default BatteryModule
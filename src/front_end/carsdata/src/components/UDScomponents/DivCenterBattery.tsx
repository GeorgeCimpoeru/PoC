'use client'
import React, { useEffect, useState } from 'react';
import Image from 'next/image';
import './style.css';
import ModalUDS from '../sharedComponents/ModalUDS';

interface batteryData {
    battery_level: any,
    voltage: any,
    battery_state_of_charge: any,
    percentage: any,
    life_cycle: any,
    fully_charged: any,
    serial_number: any,
    range_battery: any,
    charging_time: any,
    device_consumption: any,
    time_stamp: any,
}

const DivCenterBattery = (props: any) => {
    const [data, setData] = useState<batteryData | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    const [batteryLevel, setBatteryLevel] = useState<string>('');
    const [batteryStateOfCharge, setBatteryStateOfCharge] = useState<string>('');
    const [percentage, setPercentage] = useState<string>('');
    const [voltage, setVoltage] = useState<string>('');

    const readInfoBattery = async () => {
        await fetch('http://127.0.0.1:5000/api/read_info_battery')
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.json();
            })
            .then(data => {
                setData(data);
                setBatteryLevel(data.battery_level);
                switch (data.battery_state_of_charge) {
                    case 'Unknown state':
                        setBatteryStateOfCharge('0');
                        break;
                    case 'Charging':
                        setBatteryStateOfCharge('1');
                        break;
                    case 'Discharging':
                        setBatteryStateOfCharge('2');
                        break;
                    case 'Empty':
                        setBatteryStateOfCharge('3');
                        break;
                    case 'Fully charged':
                        setBatteryStateOfCharge('4');
                        break;
                    case 'Pending charge':
                        setBatteryStateOfCharge('5');
                        break;
                    default: // 'Pending discharge':
                        setBatteryStateOfCharge('6');
                        break;
                }
                setPercentage(data.percentage.toString().split(".")[0]);
                setVoltage(data.voltage.toString());
                setLoading(false);
            })
            .catch(error => {
                setError(error);
                setLoading(false);
            });
    };

    useEffect(() => {
        readInfoBattery();
    }, []);

    const handleInputChange = (input: any, id: string) => {
        if (id === "my_modal_1") {
            setBatteryLevel(input);
        } else if (id === "my_modal_8") {
            setPercentage(input);
        } else if (id === "my_modal_10") {
            setVoltage(input);
        }
    }

    const handleInputClick = (id: string) => {
        if (id === "my_modal_1") {
            setBatteryLevel("0");
        } else if (id === "my_modal_8") {
            setPercentage("0");
        } else if (id === "my_modal_10") {
            setVoltage("0");
        }
    }

    const writeInfoBattery = async (newStateOfCharge: string) => {
        const data2 = {
            battery_level: batteryLevel || null,
            battery_state_of_charge: newStateOfCharge || batteryStateOfCharge || null,
            percentage: percentage || null,
            voltage: voltage || null,
        };
        console.log(data2);

        await fetch('http://127.0.0.1:5000/api/write_info_battery', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data2),
        })
            .then(response => response.json())
            .then(data => {
                console.log(data);
            })
            .catch(error => {
                console.error('Error:', error);
            });
        readInfoBattery();
    }

    return (
        <div className="w-[65%] flex h-screen bg-indigo-950 math-paper" >
            <div className="w-[35%] flex flex-col items-center">
                <h3 className="text-white text-3xl">Vehicle Model</h3>
                <div className="w-full h-full flex flex-col items-center justify-center">

                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_1"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.battery_level}%
                        </label>
                        <ModalUDS id="my_modal_1" cardTitle={'Battery level'} handleInputChange={handleInputChange} handleInputClick={handleInputClick} writeInfoBattery={writeInfoBattery} />
                        <p>Battery level</p>
                    </div>


                    <div className="w-[30%] m-7 grid justify-items-center">
                        <div className="dropdown dropdown-end">
                            <div tabIndex={0} role="button" className="btn m-1 text-white inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                                {data?.battery_state_of_charge}
                            </div>
                            <ul tabIndex={0} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                <li><a onClick={() => writeInfoBattery("1")}>Charging</a></li>
                                <li><a onClick={() => writeInfoBattery("2")}>Discharging</a></li>
                                <li><a onClick={() => writeInfoBattery("3")}>Empty</a></li>
                                <li><a onClick={() => writeInfoBattery("4")}>Fully charged</a></li>
                                <li><a onClick={() => writeInfoBattery("5")}>Pending charge</a></li>
                                <li><a onClick={() => writeInfoBattery("6")}>Pending discharge</a></li>
                            </ul>
                        </div>
                        <p className="text-white">State of charge</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-start">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.charging_time}
                        </label>
                        {/* <ModalUDS id="my_modal_3" cardTitle={'Charging time'} /> */}
                        <p>Charging time</p>
                    </div>

                    <div className="w-[30%] m-7 text-white grid justify-items-center">
                        <label htmlFor="my_modal_4"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.device_consumption}
                        </label>
                        {/* <ModalUDS id="my_modal_4" cardTitle={'Device consumption'} /> */}
                        <p>Device consumption</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_5"
                            className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                            {data?.fully_charged}
                        </label>
                        {/* <ModalUDS id="my_modal_5" cardTitle={'Full charged'} /> */}
                        <p>Full charged</p>
                    </div>


                </div>
            </div>
            <div className="w-[30%] h-screen flex">
                <Image src={props.image} alt="car sketch" width={400} height={204} className="invert" />
            </div>

            <div className="w-[35%] flex flex-col items-center justify-center">


                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_6"
                        className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                        {data?.life_cycle}
                    </label>
                    {/* <ModalUDS id="my_modal_6" cardTitle={'Life cycle'} /> */}
                    <p>Life cycle</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-center">
                    <label htmlFor="my_modal_7"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        {data?.life_cycle}
                    </label>
                    {/* <ModalUDS id="my_modal_7" cardTitle={'Life cycle'} /> */}
                    <p>Life cycle</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_8"
                        className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                        {data?.percentage}%
                    </label>
                    <ModalUDS id="my_modal_8" cardTitle={'Battery percentage'} handleInputChange={handleInputChange} handleInputClick={handleInputClick} writeInfoBattery={writeInfoBattery} />
                    <p>Battery percentage</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-center">
                    <label htmlFor="my_modal_9"
                        className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                        {data?.serial_number}
                    </label>
                    {/* <ModalUDS id="my_modal_9" cardTitle={'Serial number'} /> */}
                    <p>Serial number</p>
                </div>

                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_10"
                        className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                        {data?.voltage}V
                    </label>
                    <ModalUDS id="my_modal_10" cardTitle={'Voltage'} handleInputChange={handleInputChange} handleInputClick={handleInputClick} writeInfoBattery={writeInfoBattery} />
                    <p>Voltage</p>
                </div>


            </div>
        </div>
    )
}

export default DivCenterBattery
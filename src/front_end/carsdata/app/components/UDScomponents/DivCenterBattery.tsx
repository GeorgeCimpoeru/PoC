'use client'
import React, { useEffect, useState } from 'react';
import Image from 'next/image';
import './style.css';
import ModalUDS from '../sharedComponents/ModalUDS';

const DivCenterBattery = (props: any) => {
    const [data, setData] = useState(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);
    const [batteryLevel, setBatteryLevel] = useState<string | null>(null);
    const [batteryStateOfCharge, setBatteryStateOfCharge] = useState<string | null>(null);
    const [percentage, setPercentage] = useState<string | null>(null);
    const [voltage, setVoltage] = useState<string | null>(null);

    useEffect(() => {
        const getDataFromApi = async () => {
            await fetch('http://127.0.0.1:5000/api/read_info_battery')
                .then(response => {
                    if (!response.ok) {
                        throw new Error('Network response was not ok');
                    }
                    return response.json();
                })
                .then(data => {
                    setData(data);
                    let number = Number(data.battery_level);
                    setBatteryLevel(data.battery_level);
                    let number2 = Number(data.battery_state_of_charge);
                    setBatteryStateOfCharge("1");
                    let number3 = Number(data.percentage);
                    setPercentage("17");
                    let number4 = Number(data.voltage);
                    setVoltage(data.voltage);
                    setLoading(false);
                })
                .catch(error => {
                    setError(error);
                    setLoading(false);
                });
        };
        getDataFromApi();
    }, []);

    const handleInputChange = (input1: any, id: string) => {
        let input = Number(input1);
        if (id === "my_modal_1") {
            setBatteryLevel(input1);
        } else if (id === "my_modal_2") {
            setBatteryStateOfCharge(input1);
        } else if (id === "my_modal_8") {
            setPercentage(input1);
        } else if (id === "my_modal_10") {
            setVoltage(input1);
        }
    }

    const handleInputClick = (id: string) => {
        if (id === "my_modal_1") {
            setBatteryLevel("0");
        } else if (id === "my_modal_2") {
            setBatteryStateOfCharge("2");
        } else if (id === "my_modal_8") {
            setPercentage("0");
        } else if (id === "my_modal_10") {
            setVoltage("0");
        } 
    }

    const changeAPIdata = async () => {
        console.log(batteryLevel);
        console.log(batteryStateOfCharge);
        console.log(percentage);
        console.log(voltage);

        try {
            const response = await fetch('http://127.0.0.1:5000/api/write_info_battery', {
                method: 'POST',
                mode: 'no-cors',
                cache: 'no-cache',
                credentials: 'omit',
                headers: {
                    'Content-Type': 'application/json'
                },
                redirect: 'follow',
                referrerPolicy: 'no-referrer',
                body: JSON.stringify({
                    battery_level: batteryLevel,
                    battery_state_of_charge: batteryStateOfCharge,
                    percentage: percentage,
                    voltage: voltage
                })
            })
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                console.log(response.json());
                return response.json();
            });
            // const data = await response.json();
            // console.log(data);
        } catch (error) {
            console.error("Error:", error);
        }
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
                        <ModalUDS id="my_modal_1" cardTitle={'Battery level'} handleInputChange={handleInputChange} handleInputClick={handleInputClick} changeAPIdata={changeAPIdata}/>
                        <p>Battery level</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-center">
                        <label htmlFor="my_modal_2"
                            className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                            {data?.battery_state_of_charge}
                        </label>
                        <ModalUDS id="my_modal_2" cardTitle={'State of charge'} handleInputChange={handleInputChange} handleInputClick={handleInputClick} changeAPIdata={changeAPIdata}/>
                        <p>State of charge</p>
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
                    <ModalUDS id="my_modal_8" cardTitle={'Battery percentage'} handleInputChange={handleInputChange} handleInputClick={handleInputClick} changeAPIdata={changeAPIdata}/>
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
                    <ModalUDS id="my_modal_10" cardTitle={'Voltage'} handleInputChange={handleInputChange} handleInputClick={handleInputClick} changeAPIdata={changeAPIdata}/>
                    <p>Voltage</p>
                </div>


            </div>
        </div>
    )
}

export default DivCenterBattery
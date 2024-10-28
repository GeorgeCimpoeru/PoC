'use client'
import React, { useEffect, useState } from 'react';
import Image from 'next/image';
import ModalUDS from './ModalUDS';
import './style.css';
import { displayLoadingCircle , displayErrorPopup , removeLoadingCicle } from '../sharedComponents/LoadingCircle';
import logger from '@/src/utils/Logger';

export interface batteryData {
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

export const readInfoBattery = async (isManualFlow:boolean, setData:any) => {
    displayLoadingCircle();
    console.log("Reading battery info...");
    try {
        await fetch(`http://127.0.0.1:5000/api/read_info_battery?is_manual_flow=${isManualFlow}`, {
            method: 'GET',
        }).then(response => response.json())
            .then(data => {
                console.log(data);
                setData(data);
                if (data?.ERROR === "interrupted") {
                    displayErrorPopup("Connection failed");
                }
            });
    } catch (error) {
        console.log(error);
        removeLoadingCicle();
        displayErrorPopup("Connection failed");
    }
    removeLoadingCicle();
};

export const writeInfoBattery = async (variable: string, newValue: string, setData:any) => {
    console.log("Writing battery info...");
    let item: string = "";
    let data2;
    if (variable === "battery_level") {
        data2 = {
            battery_level: parseInt(newValue),
            is_manual_flow: true
        };
        item = "battery_level";
    } else if (variable === "battery_state_of_charge") {
        data2 = {
            state_of_charge: parseInt(newValue),
            is_manual_flow: true
        };
        item = "state_of_charge";
    } else if (variable === "percentage") {
        data2 = {
            percentage: parseInt(newValue),
            is_manual_flow: true
        };
        item = "percentage";
    } else if (variable === "voltage") {
        data2 = {
            voltage: parseInt(newValue),
            is_manual_flow: true
        };
        item = "voltage";
    }

    console.log(data2);
    displayLoadingCircle();
    await fetch(`http://127.0.0.1:5000/api/write_info_battery`, {
        method: 'POST',
        // mode: 'no-cors',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data2),
    })
        .then(response => response.json())
        .then(data => {
            console.log(data);
            removeLoadingCicle();
        })
        .catch(error => {
            console.error(error);
            displayErrorPopup("Connection failed");
            removeLoadingCicle();
        });
    readInfoBattery(true, setData);
}

const DivCenterBattery = (props: any) => {
    logger.init();
    
    const [data, setData] = useState<batteryData | null>(null);
    let popupElement: HTMLDivElement | null = null;
    let popupStyleElement: HTMLStyleElement | null = null;
    let overlayElement: HTMLDivElement | null = null;

    useEffect(() => {
        readInfoBattery(false, setData);
    }, []);

    return (
        <div className="w-[65%] flex h-screen bg-indigo-950 math-paper" >
            <div className="w-[35%] flex flex-col item-center">
                <h3 className="text-white text-3xl">Vehicle Model</h3>
                <div className="w-full h-full flex flex-col item-center justify-center">

                    <div className="w-[30%] m-7 text-white grid justify-item-end">
                        <label htmlFor="my_modal_2"
                            className="inline-flex item-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.battery_level}%
                        </label>
                        <ModalUDS id="my_modal_2" cardTitle={'Battery level'} writeInfo={writeInfoBattery} param="battery_level" setter={setData}/>
                        <p>Battery level</p>
                    </div>

                    <div className="w-[30%] m-7 grid justify-item-center">
                        <div className="dropdown dropdown-end">
                            <div tabIndex={0} role="button" className="btn m-1 text-white inline-flex item-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                                {data?.battery_state_of_charge}
                            </div>
                            <ul tabIndex={0} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                <li><a onClick={() => writeInfoBattery("battery_state_of_charge", "1", setData)}>Charging</a></li>
                                <li><a onClick={() => writeInfoBattery("battery_state_of_charge", "2", setData)}>Discharging</a></li>
                                <li><a onClick={() => writeInfoBattery("battery_state_of_charge", "3", setData)}>Empty</a></li>
                                <li><a onClick={() => writeInfoBattery("battery_state_of_charge", "4", setData)}>Fully charged</a></li>
                                <li><a onClick={() => writeInfoBattery("battery_state_of_charge", "5", setData)}>Pending charge</a></li>
                                <li><a onClick={() => writeInfoBattery("battery_state_of_charge", "6", setData)}>Pending discharge</a></li>
                            </ul>
                        </div>
                        <p className="text-white">State of charge</p>
                    </div>

                    <div className="w-[30%] m-7 text-white grid justify-item-start">
                        <label htmlFor="my_modal_3"
                            className="inline-flex item-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.charging_time}
                        </label>
                        {/* <ModalUDS id="my_modal_3" cardTitle={'Charging time'} /> */}
                        <p>Charging time</p>
                    </div>

                    <div className="w-[30%] m-7 text-white grid justify-item-center">
                        <label htmlFor="my_modal_4"
                            className="inline-flex item-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.device_consumption}
                        </label>
                        {/* <ModalUDS id="my_modal_4" cardTitle={'Device consumption'} /> */}
                        <p>Device consumption</p>
                    </div>

                    <div className="w-[30%] m-7 text-white grid justify-item-end">
                        <label htmlFor="my_modal_5"
                            className="inline-flex item-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
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

            <div className="w-[35%] flex flex-col item-center justify-center">
                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_6"
                        className="inline-flex item-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                        {data?.life_cycle}
                    </label>
                    {/* <ModalUDS id="my_modal_6" cardTitle={'Life cycle'} /> */}
                    <p>Life cycle</p>
                </div>

                <div className="w-[30%] m-7 text-white grid justify-item-center">
                    <label htmlFor="my_modal_7"
                        className="inline-flex item-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        {data?.life_cycle}
                    </label>
                    {/* <ModalUDS id="my_modal_7" cardTitle={'Life cycle'} /> */}
                    <p>Life cycle</p>
                </div>

                <div className="w-[30%] m-7 text-white grid justify-item-end">
                    <label htmlFor="my_modal_8"
                        className="inline-flex item-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                        {data?.percentage}%
                    </label>
                    <ModalUDS id="my_modal_8" cardTitle={'Battery percentage'} writeInfo={writeInfoBattery} param="percentage" />
                    <p>Battery percentage</p>
                </div>

                <div className="w-[30%] m-7 text-white grid justify-item-center">
                    <label htmlFor="my_modal_9"
                        className="inline-flex item-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                        {data?.serial_number}
                    </label>
                    {/* <ModalUDS id="my_modal_9" cardTitle={'Serial number'} /> */}
                    <p>Serial number</p>
                </div>

                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_10"
                        className="inline-flex item-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                        {data?.voltage}V
                    </label>
                    <ModalUDS id="my_modal_10" cardTitle={'Voltage'} writeInfo={writeInfoBattery} param="voltage" />
                    <p>Voltage</p>
                </div>

            </div>
        </div>
    )
}

export default DivCenterBattery
'use client'
import React, { useEffect, useState } from 'react'
import Image from 'next/image';
import ModalUDS from './ModalUDS';
import './style.css';
import { displayLoadingCircle, displayErrorPopup, removeLoadingCicle } from '../sharedComponents/LoadingCircle';
import logger from '@/src/utils/Logger';

export interface HvacModes {
    ac_status: true,
    legs: false,
    front: true,
    air_recirculation: true,
    defrost: false
}

export interface HVACData {
    ambient_air_temperature: any;
    cabin_temperature: any;
    cabin_temperature_driver_set: any;
    fan_speed: any;
    mass_air_flow: any;
    time_stamp: any;
    hvac_modes: HvacModes;
}


export const readInfoHVAC = async (isManualFlow: boolean, setData: any) => {
    displayLoadingCircle();
    console.log("Reading HVAC info...");
    try {
        await fetch(`http://127.0.0.1:5000/api/read_info_hvac?is_manual_flow=${isManualFlow}`,
            {
                method: "GET",
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

export const writeInfoHvac = async (variable: string, newValue: string, setData: any) => {
    console.log("Writing HVAC info...");
    let item: string = "";
    let data;
    if (variable === "ambient_air_temperature") {
        data = {
            ambient_air_temperature: parseInt(newValue),
            is_manual_flow: true
        };
        item = "ambient_air_temperature";
    } else if (variable === "cabin_temperature_driver_set") {
        data = {
            cabin_temperature_driver_set: parseInt(newValue),
            is_manual_flow: true
        };
        item = "cabin_temperature_driver_set";
    } else if (variable === "fan_speed") {
        data = {
            fan_speed: parseInt(newValue),
            is_manual_flow: true
        };
        item = "fan_speed";
    } else if (variable === "mass_air_flow") {
        data = {
            mass_air_flow: parseInt(newValue),
            is_manual_flow: true
        };
        item = "mass_air_flow";
    } else if (variable === "HVAC_modes") {
        data = {
            HVAC_modes: parseInt(newValue),
            is_manual_flow: true
        };
        item = "HVAC_modes";
    }

    console.log(data);
    displayLoadingCircle();
    await fetch(`http://127.0.0.1:5000/api/write_info_hvac`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
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
    readInfoHVAC(true, setData);
}

const DivCenterHVAC = (props: any) => {
    logger.init();

    const [data, setData] = useState<HVACData | null>(null);

    useEffect(() => {
        readInfoHVAC(false, setData);
    }, []);

    return (
        <div className="w-[65%] flex h-screen bg-indigo-950 math-paper">
            <div className="w-[35%] flex flex-col items-center">
                <h3 className="text-white text-3xl">Vehicle Model</h3>
                {/* <button className="btn" onClick={callApi}>Button{jsonResp.age}</button> */}
                <div className="w-full h-full flex flex-col items-center justify-center">


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_1"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.ambient_air_temperature}°C
                        </label>
                        <ModalUDS id="my_modal_1" cardTitle={'Ambient air temperature'} writeInfo={writeInfoHvac} param="ambient_air_temperature" setter={setData} />
                        <p>Ambient air temperature</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_2"
                            className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                            {data?.cabin_temperature}°C
                        </label>
                        <ModalUDS id="my_modal_2" cardTitle={'Cabin temperature'} writeInfo={writeInfoHvac} param="cabin_temperature" setter={setData} />
                        <p>Cabin temperature</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.cabin_temperature_driver_set}°C
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Cabin temperature driver set'} writeInfo={writeInfoHvac} param="cabin_temperature_driver_set" setter={setData} />
                        <p>Cabin temperature driver set</p>
                    </div>

                    {/* 
                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.hvac_modes.Air_Recirculation}°C
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Air recirculation'} />
                        <p>Air Recirculation</p>
                    </div> */}

                    {/* <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.hvac_modes.Defrost}°C
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Defrost'} />
                        <p>Defrost</p>
                    </div> */}

                </div>
            </div>
            <div className="w-[30%] h-screen flex">
                <Image src={props.image} alt="car sketch" width={400} height={204} className="invert" />
            </div>

            <div className="w-[35%] flex flex-col items-center justify-center">


                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_4"
                        className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                        {data?.fan_speed}rpm
                    </label>
                    <ModalUDS id="my_modal_4" cardTitle={'Fan speed'} writeInfo={writeInfoHvac} param="fan_speed" setter={setData} />
                    <p>Fan speed</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_5"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        {data?.hvac_modes.ac_status}
                    </label>
                    <ModalUDS id="my_modal_5" cardTitle={'HVAC modes'} />
                    <p>HVAC modes</p>
                </div>

                {/* <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_5"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        {data?.hvac_modes.Front}
                    </label>
                    <ModalUDS id="my_modal_5" cardTitle={'Front'} />
                    <p>Front</p>
                </div> */}


                {/* <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_5"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        {data?.hvac_modes.Legs}
                    </label>
                    <ModalUDS id="my_modal_5" cardTitle={'Legs'} />
                    <p>Legs</p>
                </div> */}

                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_6"
                        className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                        {data?.mass_air_flow}
                    </label>
                    <ModalUDS id="my_modal_6" cardTitle={'Mass air flow'} writeInfo={writeInfoHvac} param="mass_air_flow" setter={setData} />
                    <p>Mass air flow</p>
                </div>

            </div>
        </div>
    )
}

export default DivCenterHVAC
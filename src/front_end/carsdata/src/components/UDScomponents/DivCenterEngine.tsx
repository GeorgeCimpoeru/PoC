'use client'
import React, { useEffect, useState } from 'react';
import Image from 'next/image';
import ModalUDS from './ModalUDS';
import './style.css';
import { displayLoadingCircle, displayErrorPopup, removeLoadingCicle } from '../sharedComponents/LoadingCircle';
import logger from '@/src/utils/Logger';

interface engineData {
    coolant_temperature: any,
    engine_load: any,
    engine_rpm: any,
    fuel_level: any,
    fuel_pressure: any,
    intake_air_temperature: any,
    oil_temperature: any,
    throttle_position: any,
    vehicle_speed: any,
}

const DivCenterEngine = (props: any) => {
    logger.init();

    const [data, setData] = useState<engineData | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

    useEffect(() => {
        readInfoEngine();
    }, []);

    const readInfoEngine = async () => {
        displayLoadingCircle();
        console.log("Reading engine info...");
        await fetch(`http://127.0.0.1:5000/api/read_info_engine?is_manual_flow=false`,
            { method: "GET" }
        )
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.json();
            })
            .then(data => {
                setData(data);
                setLoading(false);
            })

            .catch(error => {
                setError(error);
                setLoading(false);
                displayErrorPopup("Connection failed");
                removeLoadingCicle();
            });
        removeLoadingCicle();
    };

    const writeInfoEngine = async (variable: string, newValue: string) => {
        console.log("Writing engine info...");
        let item: string = "";
        let data2;
        if (variable === "coolant_temperature") {
            data2 = {
                coolant_temperature: parseInt(newValue)
            };
            item = "coolant_temperature";
        } else if (variable === "engine_load") {
            data2 = {
                engine_load: parseInt(newValue)
            };
            item = "engine_load";
        } else if (variable === "engine_rpm") {
            data2 = {
                engine_rpm: parseInt(newValue)
            };
            item = "engine_rpm";
        } else if (variable === "fuel_level") {
            data2 = {
                fuel_level: parseInt(newValue)
            };
            item = "fuel_level";
        } else if (variable === "intake_air_temperature") {
            data2 = {
                intake_air_temperature: parseInt(newValue)
            };
            item = "intake_air_temperature";
        } else if (variable === "fuel_pressure") {
            data2 = {
                fuel_pressure: parseInt(newValue)
            };
            item = "fuel_pressure";
        } else if (variable === "oil_temperature") {
            data2 = {
                oil_temperature: parseInt(newValue)
            };
            item = "oil_temperature";
        } else if (variable === "throttle_position") {
            data2 = {
                throttle_position: parseInt(newValue)
            };
            item = "throttle_position";
        } else if (variable === "vehicle_speed") {
            data2 = {
                vehicle_speed: parseInt(newValue)
            };
            item = "vehicle_speed";
        }

        console.log(data2);

        await fetch(`http://127.0.0.1:5000/api/write_info_engine?item=${item}`, {
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

            })
            .catch(error => {
                console.error(error);

            });
        console.log("TEST READ")
        readInfoEngine();
    };

    console.log(data)
    return (
        <div className="w-[65%] flex h-screen bg-indigo-950 math-paper">
            <div className="w-[35%] flex flex-col items-center">
                <h3 className="text-white text-3xl">Vehicle Model</h3>
                <div className="w-full h-full flex flex-col items-center justify-center">


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_1"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.coolant_temperature}°C
                        </label>
                        <ModalUDS id="my_modal_1" cardTitle={'Coolant temperature'} writeInfo={writeInfoEngine} param="coolant_temperature" />
                        <p>Coolant temperature</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_2"
                            className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                            {data?.engine_load}
                        </label>
                        <ModalUDS id="my_modal_2" cardTitle={'Engine load'} writeInfo={writeInfoEngine} param="engine_load" />
                        <p>Engine load</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-start">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.engine_rpm}
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Engine rpm'} writeInfo={writeInfoEngine} param="engine_rpm" />
                        <p>Engine rpm</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                            {data?.fuel_level}%
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Fuel level'} writeInfo={writeInfoEngine} param="fuel_level" />
                        <p>Fuel level</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.fuel_pressure}
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Fuel pressure'} writeInfo={writeInfoEngine} param="fuel_pressure" />
                        <p>Fuel pressure</p>
                    </div>


                </div>
            </div>
            <div className="w-[30%] h-screen flex">
                <Image src={props.image} alt="car sketch" width={400} height={204} className="invert" />
            </div>

            <div className="w-[35%] flex flex-col items-center justify-center">


                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_4"
                        className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                        {data?.intake_air_temperature}°C
                    </label>
                    <ModalUDS id="my_modal_4" cardTitle={'Intake air temperature'} writeInfo={writeInfoEngine} param="intake_air_temperature" />
                    <p>Intake air temperature</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_5"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        {data?.oil_temperature}°C
                    </label>
                    <ModalUDS id="my_modal_5" cardTitle={'Oil temperature'} writeInfo={writeInfoEngine} param="oil_temperature" />
                    <p>Oil temperature</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_6"
                        className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                        {data?.throttle_position}
                    </label>
                    <ModalUDS id="my_modal_6" cardTitle={'Torque'} writeInfo={writeInfoEngine} param="throttle_position" />
                    <p>Throttle Position</p>
                </div>


                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_7"
                        className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                        {data?.vehicle_speed}km
                    </label>
                    <ModalUDS id="my_modal_7" cardTitle={'Vehicle speed'} writeInfo={writeInfoEngine} param="vehicle_speed" />
                    <p>Vehicle speed</p>
                </div>


            </div>
        </div>
    )
}

export default DivCenterEngine
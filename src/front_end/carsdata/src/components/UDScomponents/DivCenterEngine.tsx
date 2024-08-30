'use client'
import React, { useEffect, useState } from 'react';
import Image from 'next/image';
import ModalUDS from './ModalUDS';
import './style.css';

interface engineData {
    current_speed: any,
    engine_state: any,
    fuel_consumption: any,
    fuel_used: any,
    power_output: any,
    serial_number: any,
    state_of_running: any,
    torque: any,
    weight: any,
}

interface engineData {
    current_speed: any,
    engine_state: any,
    fuel_consumption: any,
    fuel_used: any,
    power_output: any,
    serial_number: any,
    state_of_running: any,
    torque: any,
    weight: any,
}

const DivCenterEngine = (props: any) => {
    const [data, setData] = useState<engineData | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

    useEffect(() => {
        const getDataFromApi = async () => {
            await fetch('http://127.0.0.1:5000/api/read_info_engine')  // Replace with your actual API URL
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
                });
        };
        getDataFromApi();
    }, []);

    return (
        <div className="w-[65%] flex h-screen bg-indigo-950 math-paper">
            <div className="w-[35%] flex flex-col items-center">
                <h3 className="text-white text-3xl">Vehicle Model</h3>
                <div className="w-full h-full flex flex-col items-center justify-center">


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_1"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            40km/h
                        </label>
                        {/* <ModalUDS id="my_modal_1" cardTitle={'Current speed'} /> */}
                        <p>Current speed</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_2"
                            className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                            ON
                        </label>
                        {/* <ModalUDS id="my_modal_2" cardTitle={'Engine state'} /> */}
                        <p>Engine state</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-start">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            6.5L/100km
                        </label>
                        {/* <ModalUDS id="my_modal_3" cardTitle={'Fuel consumption'} /> */}
                        <p>Fuel consumption</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                            11L
                        </label>
                        {/* <ModalUDS id="my_modal_3" cardTitle={'Fuel used'} /> */}
                        <p>Fuel used</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            95hp
                        </label>
                        {/* <ModalUDS id="my_modal_3" cardTitle={'Power output'} /> */}
                        <p>Power output</p>
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
                        4Y1-SL6...
                    </label>
                    {/* <ModalUDS id="my_modal_4" cardTitle={'Serial number'} /> */}
                    <p>Serial number</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_5"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        2300r/m
                    </label>
                    {/* <ModalUDS id="my_modal_5" cardTitle={'State of running'} /> */}
                    <p>State of running</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_6"
                        className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                        300N/m
                    </label>
                    {/* <ModalUDS id="my_modal_6" cardTitle={'Torque'} /> */}
                    <p>Torque</p>
                </div>


                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_7"
                        className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                        130kg
                    </label>
                    {/* <ModalUDS id="my_modal_7" cardTitle={'Weight'} /> */}
                    <p>Weight</p>
                </div>


            </div>
        </div>
    )
}

export default DivCenterEngine
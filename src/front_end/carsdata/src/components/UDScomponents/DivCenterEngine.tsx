'use client'
import React, { useEffect, useState } from 'react';
import Image from 'next/image';
import ModalUDS from './ModalUDS';
import './style.css';

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
    const [data, setData] = useState<engineData | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

    useEffect(() => {
        const readInfoEngine = async () => {
            await fetch(`http://127.0.0.1:5000/api/read_info_engine`)
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
        readInfoEngine();
    }, []);

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
                        <ModalUDS id="my_modal_1" cardTitle={'Coolant temperature'} />
                        <p>Coolant temperature</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_2"
                            className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                            {data?.engine_load}
                        </label>
                        <ModalUDS id="my_modal_2" cardTitle={'Engine load'} />
                        <p>Engine load</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-start">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.engine_rpm}
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Engine rpm'} />
                        <p>Engine rpm</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                            {data?.fuel_level}%
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Fuel level'} />
                        <p>Fuel level</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.fuel_pressure}
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Fuel pressure'} />
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
                    <ModalUDS id="my_modal_4" cardTitle={'Intake air temperature'} />
                    <p>Intake air temperature</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_5"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        {data?.oil_temperature}°C
                    </label>
                    <ModalUDS id="my_modal_5" cardTitle={'Oil temperature'} />
                    <p>Oil temperature</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_6"
                        className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                        {data?.throttle_position}
                    </label>
                    <ModalUDS id="my_modal_6" cardTitle={'Torque'} />
                    <p>Throttle Position</p>
                </div>


                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_7"
                        className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                        {data?.vehicle_speed}km
                    </label>
                    <ModalUDS id="my_modal_7" cardTitle={'Vehicle speed'} />
                    <p>Vehicle speed</p>
                </div>


            </div>
        </div>
    )
}

export default DivCenterEngine
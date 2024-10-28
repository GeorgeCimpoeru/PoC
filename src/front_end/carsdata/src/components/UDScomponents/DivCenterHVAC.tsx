'use client'
import React, { useEffect, useState } from 'react'
import Image from 'next/image';
import ModalUDS from './ModalUDS';
import './style.css';
import { displayLoadingCircle, displayErrorPopup, removeLoadingCicle } from '../sharedComponents/LoadingCircle';
import logger from '@/src/utils/Logger';

// interface HvacModes {
//     AC_Status: string;
//     Air_Recirculation: string;
//     Defrost: string;
//     Front: string;
//     Legs: string;
// }

interface HVACData {
    ambient_air_temperature: string;
    cabin_temperature: string;
    cabin_temperature_driver_set: string;
    fan_speed: string;

    // hvac_modes: HvacModes;

    mass_air_flow: string;
    time_stamp: string;
}


const DivCenterHVAC = (props: any) => {
    logger.init();

    const [data, setData] = useState<HVACData | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

    useEffect(() => {
        const readInfoHVAC = async () => {
            displayLoadingCircle();

            await fetch(`http://127.0.0.1:5000/api/read_info_hvac`)
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
        readInfoHVAC();
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
                        <ModalUDS id="my_modal_1" cardTitle={'Ambient air temperature'} />
                        <p>Ambient air temperature</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_2"
                            className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                            {data?.cabin_temperature}°C
                        </label>
                        <ModalUDS id="my_modal_2" cardTitle={'Cabin temperature'} />
                        <p>Cabin temperature</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.cabin_temperature_driver_set}°C
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Cabin temperature driver set'} />
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
                    <ModalUDS id="my_modal_4" cardTitle={'Fan speed'} />
                    <p>Fan speed</p>
                </div>


                {/* <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_5"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        {data?.hvac_modes.AC_Status}
                    </label>
                    <ModalUDS id="my_modal_5" cardTitle={'HVAC modes'} />
                    <p>HVAC modes</p>
                </div> */}

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
                    <ModalUDS id="my_modal_6" cardTitle={'Mass air flow'} />
                    <p>Mass air flow</p>
                </div>

            </div>
        </div>
    )
}

export default DivCenterHVAC
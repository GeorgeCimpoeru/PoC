'use client'
import React, { useEffect, useState } from 'react';
import Image from 'next/image';
import ModalUDS from './ModalUDS';
import './style.css';
import { displayLoadingCircle, displayErrorPopup, removeLoadingCicle } from '../sharedComponents/LoadingCircle';
import logger from '@/src/utils/Logger';

export interface doorsData {
    ajar: any,
    door: any,
    passenger: any,
    passenger_lock: any,
}

const checkInput = (message: any) => {
    let value;
    do {
        value = prompt(message);
        if (value !== '0' && value !== '1') {
            alert('Accepted value: 0/1');
        }
    } while (value !== '0' && value !== '1');
    return value;
};

export const writeInfoDoors = async (setData: any) => {
    displayLoadingCircle();
    const door = checkInput('Enter Door Parameter:');
    const passenger = checkInput('Enter Passenger:');
    const passenger_lock = checkInput('Enter Passenger Lock:');
    const driver = checkInput('Enter Driver:');
    const ajar = checkInput('Enter Ajar:');
    const is_manual_flow = true;

    const data = {
        door: door || null,
        passenger: passenger || null,
        passenger_lock: passenger_lock || null,
        driver: driver || null,
        ajar: ajar || null,
        is_manual_flow: is_manual_flow || null,

    };
    console.log("Writing info doors...");
    console.log(data);
    try {
        await fetch(`http://127.0.0.1:5000/api/write_info_doors`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
        }).then(response => response.json())
            .then(data => {
                setData(data);
                console.log(data);
                // fetchLogs();
            });
    } catch (error) {
        console.log(error);
        removeLoadingCicle();
    }
    removeLoadingCicle();

    readInfoDoors(false, setData)
}

export const readInfoDoors = async (isManualFlow:boolean, setData:any) => {
    displayLoadingCircle();
    console.log("Reading doors info...");
    try {
        await fetch(`http://127.0.0.1:5000/api/read_info_doors?is_manual_flow=${isManualFlow}`, {
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


const DivCenterDoors = (props: any) => {
    logger.init();
    const [data, setData] = useState<doorsData | null>(null);
    

    useEffect(() => {
        
        readInfoDoors(false, setData);
    }, []);


    return (
        <div className="w-[65%] flex h-screen bg-indigo-950 math-paper">
            <div className="w-[35%] flex flex-col items-center">
                <h3 className="text-white text-3xl">Vehicle Model</h3>
                {/* <button className="btn" onClick={callApi}>Button{jsonResp.age}</button> */}
                <div className="w-full h-full flex flex-col items-center justify-center">

                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            {data?.ajar}
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Door status - Ajar'} />
                        <p>Ajar</p>
                    </div>

                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_2"
                            className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                            {data?.door}
                        </label>
                        <ModalUDS id="my_modal_2" cardTitle={'Door'} />
                        <p>Door</p>
                    </div>

                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            {data?.passenger}
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Passenger'} />
                        <p>Passenger</p>
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
                        {data?.passenger_lock}
                    </label>
                    <ModalUDS id="my_modal_4" cardTitle={'Passenger lock'} />
                    <p>Passenger lock</p>
                </div>
            </div>
        </div>
    )
}

export default DivCenterDoors


'use client'
import React, { useState } from 'react';
import Image from 'next/image';
import ModalUDS from './ModalUDS';
import './style.css';

const DivCenterMCU = (props: any) => {
    const [jsonResp, setJsonResp] = useState('');

    const callApi = async () => {
        const res = await fetch(
            'https://api.agify.io/?name=meelad',
            { cache: 'no-store' }
        );
        const jsonData = await res.json();
        setJsonResp(jsonData);
    }

    return (
        <div className="w-[65%] flex h-screen bg-indigo-950 math-paper">
            <div className="w-[35%] flex flex-col items-center">
                <h3 className="text-white text-3xl">Vehicle Model</h3>
                {/* <button className="btn" onClick={callApi}>Button{jsonResp.age}</button> */}
                <div className="w-full h-full flex flex-col items-center justify-center">


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_1"
                            className="inline-flex items-center justify-center p-2 bg-blue-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-blue-700">
                            75%
                        </label>
                        <ModalUDS id="my_modal_1" cardTitle={'Parameter X'} />
                        <p>Parameter X</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-center">
                        <label htmlFor="my_modal_2"
                            className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                            50km/h
                        </label>
                        <ModalUDS id="my_modal_2" cardTitle={'Parameter X'} />
                        <p>Parameter X</p>
                    </div>


                    <div className="w-[30%] m-7 text-white grid justify-items-end">
                        <label htmlFor="my_modal_3"
                            className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                            321km
                        </label>
                        <ModalUDS id="my_modal_3" cardTitle={'Parameter X'} />
                        <p>Parameter X</p>
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
                        4psi
                    </label>
                    <ModalUDS id="my_modal_4" cardTitle={'Parameter X'} />
                    <p>Parameter X</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_5"
                        className="inline-flex items-center justify-center p-2 bg-red-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-red-700">
                        OFF
                    </label>
                    <ModalUDS id="my_modal_5" cardTitle={'Parameter X'} />
                    <p>Parameter X</p>
                </div>


                <div className="w-[30%] m-7 text-white grid justify-items-end">
                    <label htmlFor="my_modal_6"
                        className="inline-flex items-center justify-center p-2 bg-green-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-green-700">
                        ON
                    </label>
                    <ModalUDS id="my_modal_6" cardTitle={'Parameter X'} />
                    <p>Parameter X</p>
                </div>


                <div className="w-[30%] m-7 text-white">
                    <label htmlFor="my_modal_7"
                        className="inline-flex items-center justify-center p-2 bg-purple-500 rounded-full border-4 border-gray-700 transition duration-300 ease-in-out hover:bg-purple-700">
                        4Y1-SL6...
                    </label>
                    <ModalUDS id="my_modal_7" cardTitle={'Parameter X'} />
                    <p>Parameter X</p>
                </div>


            </div>
        </div>
    )
}

export default DivCenterMCU
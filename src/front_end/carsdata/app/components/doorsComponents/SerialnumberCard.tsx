'use client';
import React from 'react';
import SerialNumber from '../animations/serialNumber.js';
import { SetStateAction, useState } from 'react';
import Modal from "./Modal";

const Serialnumber = (props: any) => {
    const [inputValue, setInputValue] = useState(props.serialNumber);

    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Serial number</h2>
                <p>Serial number: {inputValue}</p>
                <div className="card-actions justify-end">
                    <SerialNumber></SerialNumber>
                </div>
            </div>
        </div>
    )
}

export default Serialnumber
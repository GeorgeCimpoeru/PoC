'use client'
import React from 'react'
import SerialNumber from '../animations/serialNumber.js'
import { SetStateAction, useState } from 'react';
import Modal from "./Modal"

const Serialnumber = (props: any) => {
    const [inputValue, setInputValue] = useState(props.serialNumber);

    const handleInputChange = (value: any, id: string) => {
        setInputValue(value);
    };

    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Serial number</h2>
                <p>Serial number: {inputValue}</p>
                <label htmlFor="my_modal_2" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_2" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Serial Number'} />
                <div className="card-actions justify-end">
                    <SerialNumber></SerialNumber>
                </div>
            </div>
        </div>
    )
}

export default Serialnumber
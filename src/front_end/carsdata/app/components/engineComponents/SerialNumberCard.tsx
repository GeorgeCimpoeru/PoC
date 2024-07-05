'use client';
import React, { useState } from "react";
import SerialNumber from '../animations/serialNumber.js';
import Modal from './Modal';

const SerialNumberCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.serial_number);

    const handleInputChange = (value: any) => {
        setInputValue(value);
    }

    const handleInputClick = () => {
        setInputValue('');
    }

    return (
        <div className="card w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Serial number</h2>
                <h1>Serial number: {inputValue}</h1>
                <label htmlFor="my_modal_3" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_3" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Serial Number'} />
                <div className="card-actions justify-end">
                    <SerialNumber></SerialNumber>
                </div>
            </div>
        </div>
    )
}
export default SerialNumberCard
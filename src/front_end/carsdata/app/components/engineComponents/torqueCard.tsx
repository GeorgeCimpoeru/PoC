'use client';
import React, { useState } from "react";
import Torque from '../animations/torque.js';
import Modal from './Modal.jsx';

const TorqueCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.torque);

    const handleInputChange = (value: any) => {
            setInputValue(value);

        
    }

    const handleInputClick = () => {
        setInputValue('');
    };
    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Torque</h2>
                <h1>Torque: {inputValue}</h1>
                <label htmlFor="my_modal_1" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_1" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Torque'} />
                <div className="card-actions justify-end">
                    <Torque></Torque>
                </div>
            </div>
        </div>
    )
}

export default TorqueCard
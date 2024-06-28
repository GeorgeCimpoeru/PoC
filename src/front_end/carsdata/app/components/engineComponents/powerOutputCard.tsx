'use client';
import React, { useState } from "react";
import PowerOuput from '../animations/powerOutput.js';
import Modal from "./Modal.jsx";

const PowerOutputCard = (props:any) => {
    const [inputValue, setInputValue] = useState(props.power_output);

    const handleInputChange = (value: any) => {
            setInputValue(value);

    }

    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div className="card w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Power output</h2>
                <h1>Power output: {inputValue}</h1>
                <label htmlFor="my_modal_4" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_4" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Power Output'} />
                <div className="card-actions justify-end">
                    <PowerOuput></PowerOuput>
                </div>
            </div>
        </div>
    )
}
export default PowerOutputCard
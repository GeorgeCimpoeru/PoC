'use client';
import React, { useState } from 'react';
import MediumSpeed from '../animations/mediumSpeed.js';
import Modal from "./Modal.js";

const MediumSpeedCard = (props:any) => {
    const [inputValue, setInputValue] = useState(props.medium_speed);

    // const handleInputChange = (value: any) => {
    //         setInputValue(value);

    // }

    // const handleInputClick = () => {
    //     setInputValue('');
    // };

    return (
        <div className="card w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Medium Speed</h2>
                <h1>Medium Speed: {inputValue}</h1>
                {/* <label htmlFor="my_modal_4" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_4" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Power Output'} /> */}
                <div className="card-actions justify-end">
                    <MediumSpeed></MediumSpeed>
                </div>
            </div>
        </div>
    )
}
export default MediumSpeedCard
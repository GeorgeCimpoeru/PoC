'use client';
import React, { useState } from "react";
import StateOfRunning from '../animations/stateOfRunning.js';
import Modal from './Modal.jsx';

const StateOfRunningCard = (props:any) => {
    const [inputValue, setInputValue] = useState(props.state_of_running);

    const handleInputChange = (value: any) => {
            setInputValue(value);

    }

    const handleInputClick = () => {
        setInputValue('');
    };
    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">State Of Running</h2>
                <h1>State: {inputValue}</h1>
                <label htmlFor="my_modal_2" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_2" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'State Of Running'} />
                <div className="card-actions justify-end">
                    <StateOfRunning></StateOfRunning>
                </div>
            </div>
        </div>
    )
}
export default StateOfRunningCard
'use client';
import React, { useState } from "react";
import FuelUsed from '../animations/fuelUsed.js';
import Modal from "./Modal.jsx";

const FuelUsedCard = (props:any) => {
    const [inputValue, setInputValue] = useState(props.fuel_used);

    const handleInputChange = (value: any) => {
            setInputValue(value);

    }

    const handleInputClick = () => {
        setInputValue('');
    };
    return (
        <div className="card w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Fuel used</h2>
                <h1>Fuel used: {inputValue}</h1>
                <label htmlFor="my_modal_5" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_5" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Fuel Used'} />
                <div className="card-actions justify-end">
                    <FuelUsed></FuelUsed>
                </div>
            </div>
        </div>
    )
}
export default FuelUsedCard
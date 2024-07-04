'use client';
import React, { useState } from "react";
import FuelConsumption from '../animations/fuelConsumption.js';
import Modal from "./Modal";

const FuelConsumptionCard = (props:any) => {
    const [inputValue, setInputValue] = useState(props.fuel_consumption);

    const handleInputChange = (value: any) => {
            setInputValue(value);

    }

    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Fuel consumption</h2>
                <h1>Fuel consumption: {inputValue}</h1>
                {/* <label htmlFor="my_modal_6" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_6" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Fuel Consumption'} /> */}
                <div className="card-actions justify-end">
                    <FuelConsumption></FuelConsumption>
                </div>
            </div>
        </div>
    )
}
export default FuelConsumptionCard
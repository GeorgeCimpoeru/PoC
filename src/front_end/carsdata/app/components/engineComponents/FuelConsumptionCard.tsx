'use client';
import React, { useState } from "react";
import FuelConsumption from '../animations/fuelConsumption.js';
import Modal from "./Modal";

const FuelConsumptionCard = (props: any) => {
    const [inputValue] = useState(props.fuel_consumption);

    

    return (
        <div className="card w-96 h-64 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Fuel consumption</h2>
                <h1>Fuel consumption: {inputValue}</h1>
                
                <div className="card-actions justify-end">
                    <FuelConsumption></FuelConsumption>
                </div>
            </div>
        </div>
    )
}
export default FuelConsumptionCard
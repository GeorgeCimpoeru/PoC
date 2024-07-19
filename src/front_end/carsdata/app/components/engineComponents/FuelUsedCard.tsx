'use client';
import React, { useState } from "react";
import FuelUsed from '../animations/fuelUsed.js';
import Modal from "./Modal";

const FuelUsedCard = (props: any) => {
    const [inputValue] = useState(props.fuel_used);

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Fuel used</h2>
                <h1>Fuel used: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <FuelUsed></FuelUsed>
                </div>
            </div>
        </div>
    )
}
export default FuelUsedCard
'use client';
import React, { useState } from 'react';
import BatteryPercentage from '../animations/batteryPercentage.js';


const PercentageCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.percentageBattery);

    const handleInputChange = (value: any) => {
        setInputValue(value);
    };

    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Battery percentage</h2>
                <h1>Battery level: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <BatteryPercentage></BatteryPercentage>
                </div>
            </div>
        </div>
    )
}


export default PercentageCard

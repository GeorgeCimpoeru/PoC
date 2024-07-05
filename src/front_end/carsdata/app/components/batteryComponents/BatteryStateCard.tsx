'use client';
import React, { useState } from 'react';
import BatteryStateOfCharge from '../animations/batteryState.js';

const BatteryStateCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.BatteryStateOfCharge);
    const handleInputChange = (value: any) => {
        setInputValue(value);
    };

    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Battery state of charge</h2>
                <h1>Battery state: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <BatteryStateOfCharge></BatteryStateOfCharge>
                </div>
            </div>
        </div>
    )
}

export default BatteryStateCard




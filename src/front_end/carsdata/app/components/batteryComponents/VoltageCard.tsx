'use client';
import React, { useState } from 'react';
import BatteryVoltage from '../animations/batteryVoltage.js';

const VoltageCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.voltageCard);

    const handleInputChange = (value: any) => {
        setInputValue(value);
    };

    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Voltage</h2>
                <h1>Voltage: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <BatteryVoltage></BatteryVoltage>
                </div>
            </div>
        </div>
    )
}

export default VoltageCard


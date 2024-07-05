'use client';
import React, { useState } from 'react';
import ChargingTime from '../animations/chargingTime.js';

const ChargingTimeCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.chargingTime);
    const handleInputChange = (value: any) => {
        setInputValue(value);
    };

    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Charging time</h2>
                <h1>Charging time: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <ChargingTime></ChargingTime>
                </div>
            </div>
        </div>
    )
}

export default ChargingTimeCard
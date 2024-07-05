'use client';
import React, { useState } from 'react';
import Temperature from '../animations/temperature.js';

const TemperatureCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.temperature);
    const handleInputChange = (value: any) => {
        setInputValue(value)

    };
    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div>
            <div className="card h-64 w-96 bg-green-300  text-black">
                <div className="card-body">
                    <h2 className="card-title">Temperature</h2>
                    <h1>Temperature value: {inputValue}</h1>
                    <div className="card-actions justify-end">
                        <Temperature></Temperature>
                    </div>
                </div>
            </div>
        </div>
    )
}


export default TemperatureCard

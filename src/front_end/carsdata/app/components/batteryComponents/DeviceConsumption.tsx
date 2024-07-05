'use client';
import React, { useState } from 'react';
import DeviceConsumption from '../animations/deviceConsumption.js';

const DeviceConsumptionCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.deviceConsumption);
    const handleInputChange = (value: any) => {
        setInputValue(value);
    };

    const handleInputClick = () => {
        setInputValue(0);
    };

    const generateRandomNumbers = () => {
        const numSamples = 2;
        const numbers: number[] = Array.from({ length: numSamples }, () => Math.random() * 10);
        const avg = numbers.reduce((sum, num) => sum + num, 0) / numSamples;

        setInputValue(avg);
    };

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Device consumption</h2>
                <h3>Device consumption: {inputValue.toFixed(2) !== null ? inputValue.toFixed(2) : 'N/A'}</h3>
                <div className="card-actions justify-end">
                    <DeviceConsumption></DeviceConsumption>
                </div>
            </div>
        </div>
    )
}


export default DeviceConsumptionCard


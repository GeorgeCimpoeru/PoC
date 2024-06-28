'use client';
import React from 'react';
import Cigarette_light from '../animations/cigarette_light.js';
import { SetStateAction, useState } from 'react';
import Modal from "./Modal";

const CigaretteVoltageLighter = (props: any) => {
    const [inputValue, setInputValue] = useState(props.cigarette_lighter);
    const handleInputChange = (value: any, id: string) => {
        setInputValue(value);
    };

    const handleInputClick = () => {
        setInputValue('');
    };
    const generateRandomNumbers = () => {
        const numSamples = 2;
        const numbers: number[] = Array.from({ length: numSamples }, () => Math.random() * 10);

        const avg = numbers.reduce((sum, num) => sum + num, 0) / numSamples;
        setInputValue(avg); // Set the average value to cigaretteLighterVoltage
    };
    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Cigarette lighter voltage</h2>
                <h3>Cigarette Lighter Voltage: {inputValue.toFixed(2) !== null ? inputValue.toFixed(2) : 'N/A'}</h3>
                <span style={{ display: 'flex', gap: '8px' }}>
                    <label htmlFor="my_modal_3" className="w-10 btn btn-sm">Edit</label>
                    <button className="w-1/5 btn btn-sm" onClick={generateRandomNumbers}>Average</button>
                </span>
                <Modal id="my_modal_3" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Cigarette voltage'} />
                <div className="card-actions justify-end">
                    <Cigarette_light></Cigarette_light>
                </div>
            </div>
        </div>
    )
}

export default CigaretteVoltageLighter
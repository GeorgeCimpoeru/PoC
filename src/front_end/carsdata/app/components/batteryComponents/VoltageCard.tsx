'use client';
import React, { useState } from 'react';
import BatteryVoltage from '../animations/batteryVoltage.js';
import ModalString from '../sharedComponents/ModalString';

const VoltageCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.voltageCard);
    
    const handleInputChange = (value: any) => {
        setInputValue(value);
    }

    const handleInputClick = () => {
        setInputValue('');
    }
    
    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Voltage</h2>
                <h1>Voltage: {inputValue}</h1>
                <label htmlFor="my_modal_5" className="w-10 btn btn-sm">Edit</label>
                <ModalString id="my_modal_5" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Battery state of charge'} />
                <div className="card-actions justify-end">
                    <BatteryVoltage></BatteryVoltage>
                </div>
            </div>
        </div>
    )
}

export default VoltageCard


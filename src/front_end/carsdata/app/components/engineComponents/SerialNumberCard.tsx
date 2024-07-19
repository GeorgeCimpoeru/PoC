'use client';
import React, { useState } from "react";
import SerialNumber from '../animations/serialNumber.js';

const SerialNumberCard = (props: any) => {
    const [inputValue] = useState(props.serial_number);

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Serial number</h2>
                <h1>Serial number: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <SerialNumber></SerialNumber>
                </div>
            </div>
        </div>
    )
}
export default SerialNumberCard
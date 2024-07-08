'use client';
import React, { useState } from "react";
import Torque from '../animations/torque.js';

const TorqueCard = (props: any) => {
    const [inputValue] = useState(props.torque);

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Torque</h2>
                <h1>Torque: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <Torque></Torque>
                </div>
            </div>
        </div>
    )
}

export default TorqueCard
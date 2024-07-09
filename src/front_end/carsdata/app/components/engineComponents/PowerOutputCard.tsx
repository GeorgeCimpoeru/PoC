'use client';
import React, { useState } from "react";
import PowerOuput from '../animations/powerOutput.js';
import Modal from "./Modal";

const PowerOutputCard = (props: any) => {
    const [inputValue] = useState(props.power_output);

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Power output</h2>
                <h1>Power output: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <PowerOuput></PowerOuput>
                </div>
            </div>
        </div>
    )
}
export default PowerOutputCard
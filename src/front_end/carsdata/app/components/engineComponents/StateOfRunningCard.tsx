'use client';
import React, { useState } from "react";
import StateOfRunning from '../animations/stateOfRunning.js';

const StateOfRunningCard = (props: any) => {
    const [inputValue] = useState(props.state_of_running);

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">State Of Running</h2>
                <h1>State: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <StateOfRunning></StateOfRunning>
                </div>
            </div>
        </div>
    )
}
export default StateOfRunningCard
'use client'
import React, { useState } from "react";
import EngineState from '../animations/engineState.js';

const EngineStateCard = (props: any) => {
    const [status, setstatus] = useState(props.engine_state);

    const change_status = () => {
        if (status == 'start') {
            setstatus('stop');
        } else {
            setstatus('start');
        }
    }

    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Door status</h2>
                <h1>Status: {status}</h1>
                <input type="checkbox" className="toggle" onChange={change_status} defaultChecked />
                <div className="card-actions justify-end">
                    <EngineState></EngineState>
                </div>
            </div>
        </div>
    )
     
}

export default EngineStateCard
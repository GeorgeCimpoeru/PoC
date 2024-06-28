import React from "react";
import EngineState from '../animations/engineState.js';

const EngineStateCard = (props:any) => {
    return (
        <div className="card w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Engine state</h2>
                <h1>Engine state: {props.engine_state}</h1>
                <div className="card-actions justify-end">
                    <EngineState></EngineState>
                </div>
            </div>
        </div>
    )
}
export default EngineStateCard
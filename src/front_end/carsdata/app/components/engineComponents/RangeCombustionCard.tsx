'use client';
import React, { useState } from 'react';
import CombustionRange from '../animations/combustion.js';

const PowerOutputCard = (props:any) => {
    const [inputValue] = useState(props.combustion_range);

    return (
        <div className="card w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Range Combustion</h2>
                <h1>Range Combustion: {inputValue}</h1>
                <div className="card-actions justify-end">
                    <CombustionRange></CombustionRange>
                </div>
            </div>
        </div>
    )
}
export default PowerOutputCard
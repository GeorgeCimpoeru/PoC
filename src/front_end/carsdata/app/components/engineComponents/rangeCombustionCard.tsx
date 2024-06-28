'use client';
import React, { useState } from 'react';
import CombustionRange from '../animations/combustion.js';
import Modal from "./Modal.jsx";

const PowerOutputCard = (props:any) => {
    const [inputValue, setInputValue] = useState(props.combustion_range);

    // const handleInputChange = (value: any) => {
    //         setInputValue(value);

    // }

    // const handleInputClick = () => {
    //     setInputValue('');
    // };

    return (
        <div className="card w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Range Combustion</h2>
                <h1>Range Combustion: {inputValue}</h1>
                {/* <label htmlFor="my_modal_4" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_4" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Power Output'} /> */}
                <div className="card-actions justify-end">
                    <CombustionRange></CombustionRange>
                </div>
            </div>
        </div>
    )
}
export default PowerOutputCard
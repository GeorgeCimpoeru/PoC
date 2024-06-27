'use client'
import React, { useState } from "react"
import CurrentSpeed from '../animations/currentSpeed.js'
import Modal from "./Modal"

const CurrentSpeedCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.current_speed);

    const handleInputChange = (value: any, id: string) => {
            setInputValue(value);

    }

    const handleInputClick = () => {
        setInputValue('');
    };

    return (
        <div className="card w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Current speed</h2>
                <h1>Speed: {inputValue}</h1>
                <label htmlFor="my_modal_7" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_7" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Current Speed'} />
                <div className="card-actions justify-end">
                    <CurrentSpeed></CurrentSpeed>
                </div>
            </div>
        </div>
    )

}

export default CurrentSpeedCard
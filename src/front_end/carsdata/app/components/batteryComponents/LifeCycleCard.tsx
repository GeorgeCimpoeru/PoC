'use client';
import React, { useState } from 'react';
import LifeCycle from '../animations/lifeCycle.js';
import Modal from '../sharedComponents/ModalString';

const LifeCycleCard = (props: any) => {
    const [inputValue, setInputValue] = useState(props.life_cycle);
    
    const handleInputChange = (value: any) => {
        setInputValue(value);
    }

    const handleInputClick = () => {
        setInputValue('');
    }

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Life cycle</h2>
                <h1>Life cycle: {inputValue}</h1>
                <label htmlFor="my_modal_6" className="w-10 btn btn-sm">Edit</label>
                <Modal id="my_modal_6" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Life Cycle'} />
                <div className="card-actions justify-end">
                    <LifeCycle></LifeCycle>
                </div>
            </div>
        </div>
    )
}

export default LifeCycleCard


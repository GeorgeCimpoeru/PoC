'use client'
import React from 'react'
import Car from '../animations/car.js'
import { SetStateAction, useState } from 'react';
import Modal from "./Modal"

const DoorPosition = (props: any) => {
  const [inputValue, setInputValue] = useState(props.doorparam);

  const handleInputChange = (value: any, id: string) => {
      setInputValue(value);
  };

  const handleInputClick = () => {
    setInputValue('');
  };

  return (
    <div className="card h-64 w-96 bg-green-300 text-black">
      <div className="card-body">
        <h2 className="card-title">Door</h2>
        <p>Door: {inputValue}</p>
        <label htmlFor="my_modal_1" className="w-10 btn btn-sm">Edit</label>
        <Modal id="my_modal_1" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Door'} />
        <div className="card-actions justify-end">
          <Car></Car>
        </div>
      </div>
    </div>
  )
}

export default DoorPosition
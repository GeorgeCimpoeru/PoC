'use client'
import React, { useState } from 'react';
import Temperature from '../animations/temperature.js';
import Modal from '../sharedComponents/ModalString';

const TemperatureCard = (props: any) => {
      const [inputValue, setInputValue] = useState(props.temperature);
      const handleInputChange = (value: any) => {
            setInputValue(value)

      }
      const handleInputClick = () => {
            setInputValue('');
      };
      return (
            <div>
                  <div className="card h-64 w-96 bg-green-300  text-black">
                        <div className="card-body">
                              <h2 className="card-title">Temperature</h2>
                              <h1>Temperature value: {inputValue}</h1>
                              <label htmlFor="my_modal_2" className="w-10 btn btn-sm">Edit</label>
                              <Modal id="my_modal_2" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Temperature'} />
                              <div className="card-actions justify-end">
                                    <Temperature></Temperature>
                              </div>
                        </div>
                  </div>
            </div>
      )
}


export default TemperatureCard

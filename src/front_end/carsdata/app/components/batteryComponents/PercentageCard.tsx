'use client';
import React, { useState } from 'react';
import BatteryPercentage from '../animations/batteryPercentage.js';
import ModalString from '../sharedComponents/ModalString';

const PercentageCard = (props: any) => {
      const [inputValue, setInputValue] = useState(props.percentageBattery);
      const handleInputChange = (value: any) => {
            setInputValue(value)

      }

      const handleInputClick = () => {
            setInputValue('');
      };
      return (
            <div className="card h-64 w-96 bg-green-300 text-black">
                  <div className="card-body">
                        <h2 className="card-title">Battery percentage</h2>
                        <h1>Battery level: {inputValue}</h1>
                        <label htmlFor="my_modal_1" className="w-10 btn btn-sm">Edit</label>
                        <ModalString id="my_modal_1" handleInputChange={handleInputChange} handleInputClick={handleInputClick}
                              cardTitle={'Battery percentage'} />
                        <div className="card-actions justify-end">
                              <BatteryPercentage></BatteryPercentage>
                        </div>
                  </div>
            </div>
      )
}


export default PercentageCard

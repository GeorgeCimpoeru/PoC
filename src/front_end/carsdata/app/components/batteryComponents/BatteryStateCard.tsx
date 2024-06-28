'use client'
import React, { useState } from 'react';
import BatteryStateOfCharge from '../animations/batteryState.js';
import Modal from '../sharedComponents/ModalString';

const BatteryStateCard = (props: any) => {
      const [inputValue, setInputValue] = useState(props.BatteryStateOfCharge);
      const handleInputChange = (value: any) => {
            setInputValue(value)

      };

      const handleInputClick = () => {
            setInputValue('');
      };
      return (
            <div className="card h-64 w-96 bg-green-300  text-black">
                  <div className="card-body">
                        <h2 className="card-title">Battery state of charge</h2>
                        <h1>Battery state: {inputValue}</h1>
                        <label htmlFor="my_modal_4" className="w-10 btn btn-sm">Edit</label>
                        <Modal id="my_modal_4" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Battery state of charge'} />
                        <div className="card-actions justify-end">
                              <BatteryStateOfCharge></BatteryStateOfCharge>
                        </div>
                  </div>
            </div>
      )
}

export default BatteryStateCard




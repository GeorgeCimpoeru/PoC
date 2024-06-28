'use client'
import React, { useState } from 'react'
import ChargingTime from '../animations/charging_time'
import Modal from '../sharedComponents/ModalString'

const ChargingTimeCard = (props: any) => {
      const [inputValue, setInputValue] = useState(props.chargingTime);
      const handleInputChange = (value: any, id: string) => {
            setInputValue(value)
      };

      const handleInputClick = () => {
            setInputValue('');
      };
      return (
            <div className="card h-64 w-96 bg-green-300  text-black">
                  <div className="card-body">
                        <h2 className="card-title">Charging time</h2>
                        <h1>Charging time: {inputValue}</h1>
                        <label htmlFor="my_modal_7" className="w-10 btn btn-sm">Edit</label>
                        <Modal id="my_modal_7" handleInputChange={handleInputChange} handleInputClick={handleInputClick} cardTitle={'Charging time'} />
                        <div className="card-actions justify-end">
                              <ChargingTime></ChargingTime>
                        </div>
                  </div>
            </div>
      )
}

export default ChargingTimeCard
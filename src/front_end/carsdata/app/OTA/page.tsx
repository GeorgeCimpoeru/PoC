'use client'
import React, { useState } from 'react';
import Modal from '../components/doorsComponents/Modal';

const OTApage = () => {
      const [inputValue, setInputValue] = useState();


      return (
            <div className="navbar bg-gray-300 flex justify-between items-center px-4 py-2">
                  <h1 className="text-lg font-bold">OTA page</h1>
                  <div className="ml-auto">
                        <div className="form-control">
                              <input type="text" placeholder="Upload file" className="input input-bordered w-24 md:w-auto" />
                        </div>
                        <label htmlFor="my_modal_4" className="w-10 btn btn-sm">{inputValue}Search</label>
                        <Modal id="my_modal_test" handleInputChange={setInputValue} handleInputClick={setInputValue} cardTitle={'search'} />
                  </div>
            </div>
      )
}


export default OTApage

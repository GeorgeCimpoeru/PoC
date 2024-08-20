import React, { useState } from 'react';
import { ToastContainer, toast } from 'react-toastify';
import 'react-toastify/dist/ReactToastify.css';

const VINsearch = () => {
    const [VIN, setVIN] = useState(null);

    const handleInputChange = (event: any) => {
        setVIN(event.target.value);
    };


    const showToast = () => {
        const number = Math.floor(Math.random() * 5) + 1;
        if (number == 1) {
            toast("Diagnosis no active");
        } else if (number == 2) {
            toast("Connection error");
        } else if (number == 3) {
            toast("VIN not found");
        } else if (number == 4) {
            toast("Connection succeed");
        } else if (number == 5) {
            toast("Turn off the car before diagnosis");
        }
    };

    return (
        <div className="flex center">
            <input type="text" placeholder="Type VIN" className="m-2 input input-bordered w-full max-w-xs" onChange={handleInputChange} />
            <button className="m-2 btn btn-active btn-primary" onClick={showToast}>Search</button>
            <ToastContainer />
        </div>
    )
}

export default VINsearch
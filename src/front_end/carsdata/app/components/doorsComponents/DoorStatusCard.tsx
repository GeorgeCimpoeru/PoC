'use client';
import React, { useState } from 'react';
import Car from '../animations/car.js';

const DoorStatus = (props: any) => {
    const [status, setstatus] = useState(props.doorstatus);

    const change_status = () => {
        if (status == 'open') {
            setstatus('close');
        } else {
            setstatus('open');
        }
    }

    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Door status</h2>
                <h1>Status: {status}</h1>
                <input type="checkbox" className="toggle" onChange={change_status} defaultChecked />
                <div className="card-actions justify-end">
                    <Car></Car>
                </div>
            </div>
        </div>
    )
}

export default DoorStatus
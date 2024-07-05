'use client';
import React, { useState } from 'react';
import RangeBattery from '../animations/rangeBattery.js';


const RangeBatteryCard = (props: any) => {
    const [status, setstatus] = useState('off');

    const change_status = () => {
        if (status == 'on') {
            setstatus('off');
        } else {
            setstatus('on');
        }
    };

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Range battery</h2>
                <h1>Range: {status}</h1>
                <div className="card-actions justify-end">
                    <RangeBattery></RangeBattery>
                </div>
            </div>
        </div>
    )
}

export default RangeBatteryCard

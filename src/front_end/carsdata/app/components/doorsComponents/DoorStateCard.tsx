'use client';
import React, { useState } from 'react';
import LockUnlock from '../animations/lock.js';

const DoorState = (props: any) => {
    const [state, setstate] = useState(props.door_state);

    const change_status = () => {
        if (state == 'Lock') {
            setstate('Unlock');
        } else {
            setstate('Lock');
        }
    }

    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Lock/Unlock state</h2>
                <h1>Door state: {state}</h1>
                <input type="checkbox" className="toggle" onChange={change_status} defaultChecked />
                <div className="card-actions justify-end">
                    <LockUnlock></LockUnlock>
                </div>
            </div>
        </div>
    )
}

export default DoorState
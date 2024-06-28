'use client';
import React, { useState } from 'react';
import Indoor_light from '../animations/indoor_light.js';

const Lights = (props: any) => {
    const [light_state, setstate] = useState(props.lights);
    const change_status = () => {

          if (light_state == 'On') {
                setstate('Off');
          } else {
                setstate('On');
          }
    }
    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Lights</h2>
                <h1>Lights state: {light_state}</h1>
                <input type="checkbox" className="toggle" onChange={change_status} defaultChecked />
                <div className="card-actions justify-end">
                    <Indoor_light></Indoor_light>
                </div>
            </div>
        </div>
    )
}

export default Lights
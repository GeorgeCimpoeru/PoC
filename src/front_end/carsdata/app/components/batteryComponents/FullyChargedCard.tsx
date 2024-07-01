'use client';
import React, { useState } from 'react';
import BatteryFully from '../animations/batteryFully.js';

const FullyChargedCard = (props: any) => {
      const [status, setstatus] = useState('off');
      const change_status = () => {

            if (status == 'on') {
                  setstatus('off')
            } else {
                  setstatus('on');
            }
      }

      return (
            <div className="card h-64 w-96 bg-green-300  text-black">
                  <div className="card-body">
                        <h2 className="card-title">Fully charged</h2>
                        <h1>Fully charged: {status}</h1>
                        <input type="checkbox" className="toggle" onChange={change_status} defaultChecked />
                        <div className="card-actions justify-end">
                              <BatteryFully></BatteryFully>
                        </div>
                  </div>
            </div>
      )
}

export default FullyChargedCard

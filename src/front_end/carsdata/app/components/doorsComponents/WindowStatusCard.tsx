
'use client'
import React, { useState } from 'react'
import Window from '../animations/window.js'

const WindowStatusCard = (props: any) => {
    const [status, setstatus] = useState(props.windows_status);
    const change_status = () => {

          if (status == 'Closed') {
                setstatus('Open')
          } else {
                setstatus('Closed');
          }
    }
    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Windows status</h2>
                <h1>Windows status: {status}</h1>
                <input type="checkbox" className="toggle" onChange={change_status} defaultChecked />
                <div className="card-actions justify-end">
                <Window></Window>
                </div>
            </div>
        </div>
    )
}

export default WindowStatusCard
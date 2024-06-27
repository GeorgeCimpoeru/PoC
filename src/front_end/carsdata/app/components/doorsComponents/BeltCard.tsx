'use client'
import React, { useState } from 'react'
import Belt from '../animations/belt.js'

const BeltCard = (props: any) => {
    const [belt_state, setstate] = useState(props.belt);
    const change_belt_status = () => {

          if (belt_state == 'On') {
                setstate('Off')
          } else {
                setstate('On');
          }
    }
    return (
        <div className="card h-64 w-96 bg-green-300  text-black">
            <div className="card-body">
                <h2 className="card-title">Belt</h2>
                <h1>Belt: {belt_state}</h1>
                <input type="checkbox" className="toggle" onChange={change_belt_status} defaultChecked />
                <div className="card-actions justify-end">
                <Belt></Belt>
                </div>
            </div>
        </div>
    )
}

export default BeltCard
'use client';
import React from 'react';
import Cigarette_light from '../animations/cigarette_light.js';
import { SetStateAction, useState } from 'react';
import Modal from "./Modal";

const CigaretteVoltageLighter = (props: any) => {
    const [inputValue] = useState(props.cigarette_lighter);

    

    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Cigarette lighter voltage</h2>
                <h3>Cigarette Lighter Voltage: {inputValue}</h3>
                <div className="card-actions justify-end">
                    <Cigarette_light></Cigarette_light>
                </div>
            </div>
        </div>
    )
}

export default CigaretteVoltageLighter
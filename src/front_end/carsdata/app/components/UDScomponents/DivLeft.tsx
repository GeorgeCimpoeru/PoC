'use client'
import Image from 'next/image';
import React, { useState } from 'react';

const DivLeft = (props: any) => {
    let [activeIcon, setActiveIcon] = useState(props.selectedModule);

    const handleIconClick = (iconIndex: number) => {
        activeIcon = iconIndex;
    };

    const handleSendBack = async (iconIndex: string) => {
        props.changeSelectedModule(iconIndex)
        // Modify the value if needed
        const modifiedValue = iconIndex;
        setActiveIcon(iconIndex);

        // Send the modified value back to the server
        const response = await fetch('/UDS', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(modifiedValue),
        });

        // if (response.ok) {
        //     console.log('Value sent back to the server successfully');
        // } else {
        //     console.error('Failed to send value back to the server');
        // }
    };

    return (
        <div className="w-[10%] bg-blue-950 p-6 shadow-xl flex flex-col justify-center space-y-4">
            <div onClick={() => handleSendBack("1")} className={activeIcon === "1" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>MCU</div>
            <div onClick={() => handleSendBack("2")} className={activeIcon === "2" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>Battery</div>
            <div onClick={() => handleSendBack("3")} className={activeIcon === "3" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>Engine</div>
            <div onClick={() => handleSendBack("4")} className={activeIcon === "4" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>SOTA/SIT</div>
        </div>
    );
}

export default DivLeft;

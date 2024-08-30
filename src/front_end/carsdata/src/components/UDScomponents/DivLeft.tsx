'use client'
import React, { useState } from 'react';

const DivLeft = (props: any) => {
    let [activeIcon, setActiveIcon] = useState(props.selectedModule);

    const handleSendBack = (iconIndex: string) => {
        props.changeSelectedModule(iconIndex);
        setActiveIcon(iconIndex);
    };

    return (
        <div className="w-[10%] h-screen bg-blue-950 p-6 shadow-xl flex flex-col justify-center space-y-4">
            <div onClick={() => handleSendBack("1")} className={activeIcon === "1" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>MCU</div>
            <div onClick={() => handleSendBack("2")} className={activeIcon === "2" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>Battery</div>
            <div onClick={() => handleSendBack("3")} className={activeIcon === "3" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>Engine</div>
            <div onClick={() => handleSendBack("4")} className={activeIcon === "4" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>Doors</div>
            <div onClick={() => handleSendBack("5")} className={activeIcon === "5" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>HVAC</div>
            <div onClick={() => handleSendBack("6")} className={activeIcon === "6" ? "bg-white/50 p-1 rounded-lg" : "text-cyan-50"}>Send requests</div>
        </div>
    );
}

export default DivLeft;

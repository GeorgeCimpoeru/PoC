'use client'
import Image from 'next/image';
import React, { useState } from 'react';

const LeftSideBar2 = ({ selectedModule }: any) => {
    let [activeIcon, setActiveIcon] = useState(selectedModule);

    // const handleIconClick = (iconIndex: number) => {
    //     activeIcon = iconIndex;
    // };

    const handleSendBack = async (iconIndex: string) => {
        // Modify the value if needed
        const modifiedValue = iconIndex;
        setActiveIcon(iconIndex);

        // Send the modified value back to the server
        const response = await fetch('/uds', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(modifiedValue),
        });

        if (response.ok) {
            console.log('Value sent back to the server successfully');
        } else {
            console.error('Failed to send value back to the server');
        }
    };

    return (
        <div className="w-[6%] bg-blue-700 rounded-lg m-5 p-6 shadow-xl flex flex-col items-center space-y-4">
            <div
                onClick={() => handleSendBack("1")}
                className={activeIcon === "1" ? 'bg-white/50 p-1 rounded-lg' : ''}>
                <Image
                    src="/bat.png"
                    alt="Cloud icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
            </div>
            <div
                onClick={() => handleSendBack("2")}
                className={activeIcon === "2" ? 'bg-white/50 p-1 rounded-lg' : ''}>
                <Image
                    src="/3.png"
                    alt="Cloud icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
            </div>
            <div
                onClick={() => handleSendBack("3")}
                className={activeIcon === "3" ? 'bg-white/50 p-1 rounded-lg' : ''}>
                <Image
                    src="/car-door.svg"
                    alt="Cloud icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
            </div>
        </div>
    );
}

export default LeftSideBar2;

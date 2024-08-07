import React, { useState } from 'react';
import Image from "next/image";
import TableOta from './TableOta';

const LeftSideBar = (props: any) => {
    const [activeIcon, setActiveIcon] = useState<number | null>(null);
    const [hoveredIcon, setHoveredIcon] = useState<number | null>(null);

    const handleIconClick = (iconIndex: number) => {
        setActiveIcon(iconIndex);
        props.clickedIcon(iconIndex);
    };

    const handleMouseEnter = (iconIndex: number) => {
        setHoveredIcon(iconIndex);
    };

    const handleMouseLeave = () => {
        setHoveredIcon(null);
    };

    return (
        <div className="w-[6%] bg-blue-700 rounded-lg m-5 p-6 shadow-xl flex flex-col items-center">
            <div
                onClick={() => handleIconClick(1)}
                onMouseEnter={() => handleMouseEnter(1)}
                onMouseLeave={handleMouseLeave}
                className={activeIcon === 1 ? 'bg-white/50 p-1 rounded-lg' : ''}
            >
                <Image
                    src="/ECU.png"
                    alt="ECU icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
                {hoveredIcon === 1 && (
                    <div className="absolute bg-gray-700 text-white p-2 rounded mt-1">
                        ECU 1
                    </div>
                )}
            </div>
            <div
                onClick={() => handleIconClick(2)}
                onMouseEnter={() => handleMouseEnter(2)}
                onMouseLeave={handleMouseLeave}
                className={activeIcon === 2 ? 'bg-white/50 p-1 rounded-lg' : ''}
            >
                <Image
                    src="/mcu.png"
                    alt="MCU icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
                {hoveredIcon === 2 && (
                    <div className="absolute bg-gray-700 text-white p-2 rounded mt-1">
                        MCU 1
                    </div>
                )}
            </div>
        </div>
    );
}

export default LeftSideBar;

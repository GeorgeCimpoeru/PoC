'use client'
import React, { useState } from 'react';
import Icons from './Icons';

function LeftSideBar(props: any) {
    const [activeIcon, setActiveIcon] = useState<number | null>(1);
    const [hoveredIcon, setHoveredIcon] = useState<number | null>(null);

    const handleIconClick = (iconIndex: number) => {
        setActiveIcon(iconIndex);
        props.clickedDevice(iconIndex);
    };

    const handleMouseEnter = (iconIndex: number) => {
        setHoveredIcon(iconIndex);
    };

    const handleMouseLeave = () => {
        setHoveredIcon(null);
    };

    const iconsData = [
        { index: 1, imageSrc: '/mcu.svg', tooltipText: 'MCU' },
        { index: 2, imageSrc: '/ECUfinal.svg', tooltipText: 'Battery' },
        { index: 3, imageSrc: '/ECUfinal.svg', tooltipText: 'Engine' },
        { index: 4, imageSrc: '/ECUfinal.svg', tooltipText: 'Doors' },
        { index: 5, imageSrc: '/ECUfinal.svg', tooltipText: 'HVAC' },
    ];

    return (
        <div className="w-[6%] bg-blue-700 rounded-lg m-5 p-6 shadow-xl flex flex-col items-center">
            <div className="mt-20">
                {iconsData.map(icon => (
                    <Icons
                        key={icon.index}
                        index={icon.index}
                        activeIcon={activeIcon}
                        hoveredIcon={hoveredIcon}
                        handleIconClick={handleIconClick}
                        handleMouseEnter={handleMouseEnter}
                        handleMouseLeave={handleMouseLeave}
                        imageSrc={icon.imageSrc}
                        tooltipText={icon.tooltipText}
                    />
                ))}
            </div>
        </div>
    );
}

export default LeftSideBar;

import React from 'react';
import Image from 'next/image';

interface IconsProps {
    index: number;
    activeIcon: number | null;
    hoveredIcon: number | null;
    handleIconClick: (iconIndex: number) => void;
    handleMouseEnter: (iconIndex: number) => void;
    handleMouseLeave: () => void;
    imageSrc: string;
    tooltipText: string;
}

function Icons({
    index,
    activeIcon,
    hoveredIcon,
    handleIconClick,
    handleMouseEnter,
    handleMouseLeave,
    imageSrc,
    tooltipText,
}: IconsProps) {
    return (
        <div
            onClick={() => handleIconClick(index)}
            onMouseEnter={() => handleMouseEnter(index)}
            onMouseLeave={handleMouseLeave}
            className={activeIcon === index ? 'bg-white/50 p-1 rounded-lg' : ''}
        >
            <Image
                src={imageSrc}
                alt={`${tooltipText} icon`}
                className="dark:invert m-1 hover:object-scale-down"
                width={100}
                height={24}
                priority
            />
            {hoveredIcon === index && (
                <div className="absolute bg-gray-700 text-white p-2 rounded mt-1">
                    {tooltipText}
                </div>
            )}
        </div>
    );
}

export default Icons;

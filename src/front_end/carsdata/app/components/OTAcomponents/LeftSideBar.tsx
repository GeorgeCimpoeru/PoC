import React, { useState } from 'react';
import Image from "next/image";

const LeftSideBar = (props: any) => {
    const [activeIcon, setActiveIcon] = useState<number | null>(null);

    const handleIconClick = (iconIndex: number) => {
        setActiveIcon(iconIndex);
        props.clickedIcon(iconIndex);
    };

    return (
        <div className="w-[6%] bg-blue-700 rounded-lg m-5 p-6 shadow-xl flex flex-col items-center">
            <div
                onClick={() => handleIconClick(1)}
                className={activeIcon === 1 ? 'bg-white/50 p-1 rounded-lg' : ''}
            >
                <Image
                    src="/cloud.png"
                    alt="Cloud icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
            </div>
            <div
                onClick={() => handleIconClick(2)}
                className={activeIcon === 2 ? 'bg-white/50 p-1 rounded-lg' : ''}
            >
                <Image
                    src="/history.png"
                    alt="History icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
            </div>
        </div>
    );
}

export default LeftSideBar;



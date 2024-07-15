import React from 'react';
import Image from "next/image";



const LeftSideBar = () => {
    return (
        <div className="w-[6%] bg-blue-700 rounded-lg m-5 p-6 shadow-xl flex flex-col items-center">
            <div>
                <Image
                    src="/cloud.png"
                    alt="Cloud icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
            </div>
            <div>
                <Image
                    src="/lens2.png"
                    alt="Lens icon"
                    className="dark:invert m-1 hover:object-scale-down"
                    width={100}
                    height={24}
                    priority
                />
            </div>
        </div>

    )
}

export default LeftSideBar


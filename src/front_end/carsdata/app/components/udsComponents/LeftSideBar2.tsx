import Image from 'next/image';
import React from 'react';

interface LeftSideBar2Props {
    onButtonClick: (component: string) => void;
    activeButton: string;
}

const LeftSideBar2: React.FC<LeftSideBar2Props> = ({ onButtonClick, activeButton }) => {
    return (
        <div className="w-[6%] bg-blue-700 rounded-lg m-5 p-6 shadow-xl flex flex-col items-center space-y-4">
            <button 
                onClick={() => onButtonClick('BatteryInfo')}
                className={`w-full p-2 ${activeButton === 'BatteryInfo' ? 'bg-gray-400' : ''}`}
            >
                <Image src="/bat.png" alt="Button 1" width={70} height={10} />
            </button>
            <button 
                onClick={() => onButtonClick('EngineInfo')}
                className={`w-full p-2 ${activeButton === 'EngineInfo' ? 'bg-gray-400' : ''}`}
            >
                <Image src="/3.png" alt="Button 2" width={70} height={10} />
            </button>
            <button 
                onClick={() => onButtonClick('Component3')}
                className={`w-full p-2 ${activeButton === 'Component3' ? 'bg-gray-400' : ''}`}
            >
                <Image src="/car-door.svg" alt="Button 3" width={70} height={10} />
            </button>
        </div>
    );
}

export default LeftSideBar2;

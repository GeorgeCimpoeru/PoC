'use client';
import { useState } from "react";
import LeftSideBar2 from '../components/udsComponents/LeftSideBar2';
import NavbarUds from '../components/udsComponents/NavbarUds';
import BatteryInfo from '../components/udsComponents/BatteryInfo';
import EngineInfo from "../components/udsComponents/EngineInfo";
import Component1 from '../components/animations/batteryFully';
import Component2 from '../components/animations/batteryPercentage';
import Component3 from '../components/animations/rangeBattery';

const UDSpage = () => {
    const [selectedComponent, setSelectedComponent] = useState('BatteryInfo');
    const [activeButton, setActiveButton] = useState('BatteryInfo');

    const handleButtonClick = (component: string) => {
        setSelectedComponent(component);
        setActiveButton(component);
    }

    const renderComponent = () => {
        switch (selectedComponent) {
            case 'EngineInfo':
                return <EngineInfo />;
            case 'BatteryInfo':
                return <BatteryInfo />;
            case 'Component3':
                return <Component3 />;
            default:
                return <BatteryInfo />;
        }
    }

    return (
        <div className="flex w-full h-screen flex-col lg:flex-row">
            <LeftSideBar2 onButtonClick={handleButtonClick} activeButton={activeButton}></LeftSideBar2>
            <div className="w-[94%] h-screen flex flex-col">
                <NavbarUds></NavbarUds>
                <div>
                    {renderComponent()}
                </div>
            </div>
        </div>
    );
}

export default UDSpage;

'use client';
import { useState, useEffect } from "react";
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
    const [data, setData] = useState(null);
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState(null);

    const fetchWithTimeout = (url, options, timeout = 7000) => {
        return Promise.race([
            fetch(url, options),
            new Promise((_, reject) =>
                setTimeout(() => reject(new Error('Timeout')), timeout)
            )
        ]);
    };

    const handleButtonClick = async (component: string) => {
        setSelectedComponent(component);
        setActiveButton(component);
        if (component === 'BatteryInfo') {
            setLoading(true);
            setError(null); // Reset error state before making a new request
            console.log("Fetching battery info...");
            try {
                const response = await fetchWithTimeout('http://127.0.0.1:5000/api/read_info_battery', { cache: 'no-store' });
                console.log("Response received:", response);
                if (!response.ok) {
                    throw new Error(`Network response was not ok: ${response.statusText}`);
                }
                const result = await response.json();
                console.log("Data received:", result);
                setData(result);
            } catch (error) {
                console.error("Fetch error:", error);
                setError(error);
            } finally {
                setLoading(false);
            }
        }
    };

    const renderComponent = () => {
        switch (selectedComponent) {
            case 'EngineInfo':
                return <EngineInfo />;
            case 'BatteryInfo':
                return <BatteryInfo data={data} loading={loading} error={error} />;
            case 'Component3':
                return <Component3 />;
            default:
                return <BatteryInfo data={data} loading={loading} error={error} />;
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
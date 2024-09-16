'use client'
import { useState } from 'react';
import DivLeft from '@/src/components/UDScomponents/DivLeft';
import DivCenterMCU from '@/src/components/UDScomponents/DivCenterMCU';
import DivRight from '@/src/components/UDScomponents/DivRight';
import DivCenterBattery from '@/src/components/UDScomponents/DivCenterBattery';
import DivCenterEngine from '@/src/components/UDScomponents/DivCenterEngine';
import SendRequests from '@/src/components/UDScomponents/SendRequests';
import DivCenterDoors from '@/src/components/UDScomponents/DivCenterDoors';
import DivCenterHVAC from '@/src/components/UDScomponents/DivCenterHVAC';

const UDSpage = () => {
    const [selectedModule, setSelectedModule] = useState("1");

    const changeSelectedModule = (newSelect: string) => {
        if (newSelect == "1") {
            setSelectedModule("1");
        } else if (newSelect == "2") {
            setSelectedModule("2");
        } else if (newSelect == "3") {
            setSelectedModule("3");
        } else if (newSelect == "4") {
            setSelectedModule("4");
        } else if (newSelect == "5") {
            setSelectedModule("5");
        } else if (newSelect == "6") {
            setSelectedModule("6");
        }
    }

    return (
        <div className="flex w-full h-full">
            <DivLeft selectedModule={selectedModule} changeSelectedModule={changeSelectedModule}></DivLeft>
            {selectedModule === "1" ? <><DivCenterMCU image={"/carSketch2.svg"}/><DivRight></DivRight></> : 
            selectedModule === "2" ? <><DivCenterBattery image={"/battery.svg"}/><DivRight></DivRight></> :
            selectedModule === "3" ? <><DivCenterEngine image={"/engine.svg"}/><DivRight></DivRight></> : 
            selectedModule === "4" ? <><DivCenterDoors image={"/door.svg"}/><DivRight></DivRight></> :
            selectedModule === "5" ? <><DivCenterHVAC image={"/hvac-icon.svg"}/><DivRight></DivRight></> : <SendRequests></SendRequests>}
        </div>
    );
}

export default UDSpage;

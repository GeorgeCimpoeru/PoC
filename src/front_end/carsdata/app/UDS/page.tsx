'use client'
import LeftLeft from '../components/UDScomponents/DivLeft';
import DivCenterMCU from '../components/UDScomponents/DivCenterMCU';
import DivRight from '../components/UDScomponents/DivRight';
import DivCenterBattery from '../components/UDScomponents/DivCenterBattery';
import DivCenterEngine from '../components/UDScomponents/DivCenterEngine';
import SendRequests from '../components/UDScomponents/SendRequests';
import { useState } from 'react';

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
        }
    }

    return (
        <div className="flex w-full h-full">
            <LeftLeft selectedModule={selectedModule} changeSelectedModule={changeSelectedModule}></LeftLeft>
            {selectedModule === "1" ? <><DivCenterMCU image={"/carSketch2.svg"}/><DivRight></DivRight></> : 
            selectedModule === "2" ? <><DivCenterBattery image={"/car-battery.svg"}/><DivRight></DivRight></> :
            selectedModule === "3" ? <><DivCenterEngine image={"/motor.svg"}/><DivRight></DivRight></> : 
            selectedModule === "4" ? <><DivCenterMCU image={"/cloud-change.svg"}/><DivRight></DivRight></> : <SendRequests></SendRequests>}
        </div>
    );
}

export default UDSpage;

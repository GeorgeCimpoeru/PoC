'use client'
import LeftLeft from '../components/UDScomponents/DivLeft';
import DivCenter from '../components/UDScomponents/DivCenter';
import DivRight from '../components/UDScomponents/DivRight';
import { useState } from 'react';

const UDSpage = () => {
    const [selectedModule, setSelectedModule] = useState("1");
    const [currentImage, setCurrentImage] = useState("/carSketch2.svg");
    let carImage: string = "/carSketch2.svg";
    let engineImage: string = "/motor.svg";
    let batteryImage: string = "/car-battery.svg";
    let updateImage: string = "/cloud-change.svg";

    const changeSelectedModule = (newSelect: string) => {
        if (newSelect == "1") {
            setSelectedModule("1");
            setCurrentImage(carImage);
        } else if (newSelect == "2") {
            setSelectedModule("2");
            setCurrentImage(engineImage);
        } else if (newSelect == "3") {
            setSelectedModule("3");
            setCurrentImage(batteryImage);
        } else if (newSelect == "4") {
            setSelectedModule("4");
            setCurrentImage(updateImage);
        }
    }

    return (
        <div className="flex w-full h-full">
            <LeftLeft selectedModule={selectedModule} changeSelectedModule={changeSelectedModule}></LeftLeft>
            <DivCenter image={currentImage}></DivCenter>
            <DivRight></DivRight>
        </div>
    );
}

export default UDSpage;

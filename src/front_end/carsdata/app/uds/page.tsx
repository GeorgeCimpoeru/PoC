import LeftSideBar2 from '../components/udsComponents/LeftSideBar2';
import NavbarUds from '../components/udsComponents/NavbarUds';
import BatteryInfo from '../components/udsComponents/BatteryInfo';
import EngineInfo from "../components/udsComponents/EngineInfo";
import Component1 from '../components/animations/batteryFully';
import Component2 from '../components/animations/batteryPercentage';
import Component3 from '../components/animations/rangeBattery';

const UDSpage = () => {
    let whichIcon: string = "1";

    // if (whichIcon == 1) {
    //     return (
    //         <div className="flex w-full h-screen">
    //             <LeftSideBar2 selectedModule={whichIcon}></LeftSideBar2>
    //             <div className="w-[94%] h-screen flex flex-col">
    //                 <NavbarUds></NavbarUds>
    //                 <BatteryInfo></BatteryInfo>
    //             </div>
    //         </div>
    //     );
    // } else if (whichIcon == 2) {
    //     return (
    //         <div className="flex w-full h-screen">
    //             <LeftSideBar2 selectedModule={whichIcon}></LeftSideBar2>
    //             <div className="w-[94%] h-screen flex flex-col">
    //                 <NavbarUds></NavbarUds>
    //                 <EngineInfo></EngineInfo>
    //             </div>
    //         </div>
    //     );
    // } else if (whichIcon == 3) {
    //     return (
    //         <div className="flex w-full h-screen">
    //             <LeftSideBar2 selectedModule={whichIcon}></LeftSideBar2>
    //             <div className="w-[94%] h-screen flex flex-col">
    //                 <NavbarUds></NavbarUds>

    //             </div>
    //         </div>
    //     );
    // }
    console.log
    return (
        <div className="flex w-full h-screen">
            <LeftSideBar2 selectedModule={whichIcon}></LeftSideBar2>
            <div className="w-[94%] h-screen flex flex-col">
                <NavbarUds></NavbarUds>
                {whichIcon === "1" && (
                    <BatteryInfo></BatteryInfo>
                )}
                {whichIcon === "2" && (
                    <EngineInfo></EngineInfo>
                )}
                {whichIcon === "3" && (
                    <EngineInfo></EngineInfo>
                )}
            </div>
        </div>
    );
}

export default UDSpage;

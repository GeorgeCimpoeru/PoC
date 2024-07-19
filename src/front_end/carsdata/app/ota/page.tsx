'use client';
import { useState } from "react";
import LeftSideBar from "../components/OTAcomponents/LeftSideBar";
import NavbarOTA from "../components/OTAcomponents/NavbarOTA";
import TableOta from "../components/OTAcomponents/TableOta";
import NavbarHistory from "../components/OTAcomponents/NavbarHistory";


const OTApage = () => {
    let history1: Array<string> = ['Sofware update 1', 'Sofware update 2', 'Sofware update 3'];
    let history2: Array<string> = ['Sofware update 1', 'Sofware update 2', 'Sofware update 3', 'Sofware update 4'];
    let history3: Array<string> = ['Sofware update 1', 'Sofware update 2', 'Sofware update 3', 'Sofware update 4', 'Sofware update 5'];
    let history4: Array<string> = ['Sofware update 1', 'Sofware update 2', 'Sofware update 3', 'Sofware update 4', 'Sofware update 5', 'Sofware update 6'];
    let history5: Array<string> = ['Sofware update 1', 'Sofware update 2', 'Sofware update 3', 'Sofware update 4', 'Sofware update 5', 'Sofware update 6', 'Sofware update 7'];
    let [history6, setHistory6] = useState(['']);

    const [icon, setIcon] = useState(true);

    const clickedButton = (button: number) => {
        if (button == 1) {
            setHistory6(history1);
        } else if (button == 2) {
            setHistory6(history2);
        } else if (button == 3) {
            setHistory6(history3);
        } else if (button == 4) {
            setHistory6(history4);
        } else if (button == 5) {
            setHistory6(history5);
        }
    }

    const clickedIcon = (icon: number) => {
        if (icon == 1) {
            setIcon(true);
        } else if (icon == 2) {
            setIcon(false);
        }
    }

    if (icon) {
        return (
            <div className="flex w-full h-screen flex-col lg:flex-row">
                <LeftSideBar clickedIcon={clickedIcon}></LeftSideBar>
                <div className="w-[94%] h-screen flex flex-col">
                    <NavbarOTA></NavbarOTA>
                </div>
            </div>
        );
    } else {
        return (
            <div className="flex w-full h-screen flex-col lg:flex-row">
                <LeftSideBar clickedIcon={clickedIcon}></LeftSideBar>
                <div className="w-[94%] h-screen flex flex-col">
                    <NavbarHistory checkedButton={clickedButton}></NavbarHistory>
                    <TableOta listOfUpdates={history6} ></TableOta>
                </div>
            </div>
        );
    }
}

export default OTApage;

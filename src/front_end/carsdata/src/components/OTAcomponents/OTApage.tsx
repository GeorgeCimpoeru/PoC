'use client';
import { useState } from "react";
import LeftSideBar from "@/src/components/OTAcomponents/LeftSideBar";
import NavbarOta from "@/src/components/OTAcomponents/NavbarOta";
import TableVersionControl from "@/src/components/OTAcomponents/TableVersionControl";
import TableHistory from "@/src/components/OTAcomponents/TableHistory";
import {displayLoadingCircle, displayErrorPopup, removeLoadingCicle} from '../sharedComponents/LoadingCircle';


let MCU_versions : string = '';
let Battery_versions : string = '';
let Engine_versions : string = '';
let Doors_versions : string = '';
let HVAC_versions : string = '';

const getAvailable = async () => {
    displayLoadingCircle();
    console.log("Fetching available versions from database...");

    await fetch(`/api/getAvailableVersions`, {
        method: 'GET',
    }).then(response => response.json())
        .then(data => {
            MCU_versions = data.versions[0].versions;
            Battery_versions = data.versions[1].versions;
            Engine_versions = data.versions[2].versions;
            Doors_versions = data.versions[3].versions;
            HVAC_versions = data.versions[4].versions;
        })
        .catch(error => {
            console.log(error);
            displayErrorPopup();
            removeLoadingCicle();
        });
};
getAvailable();


const OTApage = () => {
    let history1: { id: number, artifact: string, status: string, startTime: string, size: string, uploadedBy: string }[] = [
        { "id": 0, "artifact": "Software update 1", "status": "Failed", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 1, "artifact": "Software update 2", "status": "Succeded", "startTime": "20 July 2024, 12:30", "size": "235kB", "uploadedBy": "user1" },
        { "id": 2, "artifact": "Software update 3", "status": "Incomplete", "startTime": "21 July 2024, 11:30", "size": "203kB", "uploadedBy": "user1" },
    ];
    let history2: { id: number, artifact: string, status: string, startTime: string, size: string, uploadedBy: string }[] = [
        { "id": 0, "artifact": "Software update 1", "status": "Failed", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 1, "artifact": "Software update 2", "status": "Succeded", "startTime": "21 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 2, "artifact": "Software update 3", "status": "Incomplete", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 3, "artifact": "Software update 4", "status": "Succeded", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
    ];
    let history3: { id: number, artifact: string, status: string, startTime: string, size: string, uploadedBy: string }[] = [
        { "id": 0, "artifact": "Software update 1", "status": "Failed", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 1, "artifact": "Software update 2", "status": "Incomplete", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 2, "artifact": "Software update 3", "status": "Succeded", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 3, "artifact": "Software update 4", "status": "Failed", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 4, "artifact": "Software update 5", "status": "Incomplete", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
    ];
    let history4: { id: number, artifact: string, status: string, startTime: string, size: string, uploadedBy: string }[] = [
        { "id": 0, "artifact": "Software update 1", "status": "Succeded", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 1, "artifact": "Software update 2", "status": "Incomplete", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 2, "artifact": "Software update 3", "status": "Failed", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 3, "artifact": "Software update 4", "status": "Succeded", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 4, "artifact": "Software update 5", "status": "Incomplete", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 5, "artifact": "Software update 6", "status": "Failed", "startTime": "18 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
    ];
    let history5: { id: number, artifact: string, status: string, startTime: string, size: string, uploadedBy: string }[] = [
        { "id": 0, "artifact": "Software update 1", "status": "Succeded", "startTime": "18 July 2024, 12:31", "size": "23kB", "uploadedBy": "user1" },
        { "id": 1, "artifact": "Software update 2", "status": "Incomplete", "startTime": "23 July 2024, 09:34", "size": "23kB", "uploadedBy": "user1" },
        { "id": 2, "artifact": "Software update 3", "status": "Failed", "startTime": "25 July 2024, 09:10", "size": "23kB", "uploadedBy": "user1" },
        { "id": 3, "artifact": "Software update 4", "status": "Succeded", "startTime": "30 July 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 4, "artifact": "Software update 5", "status": "Failed", "startTime": "03 August 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 5, "artifact": "Software update 6", "status": "Incomplete", "startTime": "06 August 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
        { "id": 6, "artifact": "Software update 7", "status": "Succeded", "startTime": "07 August 2024, 09:30", "size": "23kB", "uploadedBy": "user1" },
    ];
    let [history6, setHistory6] = useState<{ id: number, artifact: string, status: string, startTime: string, size: string, uploadedBy: string }[]>(history1);

    const [selectedDevice, setSelectedDevice] = useState(1);
    const [selectedTable, setSelectedTable] = useState(1);

    const clickedDevice = (icon: number) => {
        setSelectedDevice(icon);
        if (icon == 1) {
            setHistory6(history1);
        } else if (icon == 2) {
            setHistory6(history2);
        } else if (icon == 3) {
            setHistory6(history3);
        } else if (icon == 4) {
            setHistory6(history4);
        } else if (icon == 5) {
            setHistory6(history5);
        }
        renderTableComponent();
    }

    const clickedTable = (nrTable: number) => {
        setSelectedTable(nrTable);
    }

    const renderTableComponent = () => {
        if (selectedTable === 1) {
            switch (selectedDevice) {
                case 1:
                    return <TableVersionControl device={"MCU"} versions={MCU_versions} />;
                case 2:
                    return <TableVersionControl device={"Battery"} versions={Battery_versions} />;
                case 3:
                    return <TableVersionControl device={"Engine"} versions={Engine_versions}/>;
                case 4:
                    return <TableVersionControl device={"Doors"} versions={Doors_versions}/>;
                default:
                    return <TableVersionControl device={"HVAC"} versions={HVAC_versions}/>;
            }
        } else {
            return <TableHistory listOfUpdates={history6} />;
        }
    };

    return (
        <div className="flex w-full h-screen" style={{ position: 'relative', zIndex: 1 }}>
            <div style={{
                position: 'fixed',
                top: 0,
                left: 0,
                width: '100vw',
                height: '100vh',
                backgroundImage: 'url(/car1.jpg)',
                backgroundSize: 'cover',
                backgroundPosition: 'center',
                opacity: 0.4,
                zIndex: -1,
            }}>
            </div>
            <LeftSideBar clickedDevice={clickedDevice} />
            <div className="w-[94%] h-screen flex flex-col">
                <NavbarOta clickedTable={clickedTable} />
                {renderTableComponent()}
            </div>
        </div>
    );
}

export default OTApage

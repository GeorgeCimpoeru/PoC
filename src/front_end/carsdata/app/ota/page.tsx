'use client';
import { useState } from "react";
import LeftSideBar from "../components/OTAcomponents/LeftSideBar";
import NavbarOta from "../components/OTAcomponents/NavbarOta";
import TableOta from "../components/OTAcomponents/TableOta";
import NavbarHistory from "../components/OTAcomponents/NavbarHistory";
import OtaUpdatePage from "../components/OTAcomponents/OtaUpdatePage";


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
    let [history6, setHistory6] = useState<{ id: number, artifact: string, status: string, startTime: string, size: string, uploadedBy: string }[]>([]);

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
            <div className="flex w-full h-screen">
                <LeftSideBar clickedIcon={clickedIcon} />
                <div className="w-[94%] h-screen flex flex-col">
                    <NavbarOta />
                    <OtaUpdatePage></OtaUpdatePage>
                </div>
            </div>

        );
    } else {
        return (
            <div className="flex w-full h-screen">
                <LeftSideBar clickedIcon={clickedIcon}></LeftSideBar>
                <div className="w-[94%] h-screen flex flex-col">
                    <NavbarHistory checkedButton={clickedButton}></NavbarHistory>
                    <TableOta listOfUpdates={history6} ></TableOta>
                </div>
            </div>
        );
    }
}

export default OTApage

import React from 'react';
import DoorPosition from "../components/doorsComponents/DoorPositionCard";
import DoorStatus from "../components/doorsComponents/DoorStatusCard";
import DoorState from "../components/doorsComponents/DoorStateCard";
import CigaretteVoltageLighter from "../components/doorsComponents/CigaretteVoltageLighterCard";
import Lights from "../components/doorsComponents/LightsCard";
import Serialnumber from "../components/doorsComponents/SerialnumberCard";
import BeltCard from "../components/doorsComponents/BeltCard";
import WindowStatusCard from "../components/doorsComponents/WindowStatusCard";

const DoorsModule = async () => {
    const res = await fetch(
        'https://pocapi.pythonanywhere.com/api?service=read_doors',
        { cache: 'no-store' }
    );
    const jsonData = await res.json();

    return (
        <div className="h-screen flex flex-col">
            <nav className="bg-blue-900 p-8 flex justify-between items-center">
                <div className="text-white text-2xl font-bold">Doors Module</div>
            </nav>
                <div className=" flex items-center justify-center">
                    <table>
                        <tr>
                            <td>
                                <DoorPosition doorparam={jsonData.query_params.door}></DoorPosition>
                            </td>
                            <td>
                                <DoorStatus doorstatus={jsonData.query_params.status}></DoorStatus>
                            </td>
                            <td>
                                <Serialnumber serialNumber={jsonData.query_params.serial_number}></Serialnumber>
                            </td>
                            <td>
                                <CigaretteVoltageLighter cigarette_lighter={jsonData.query_params.cigarette_lighter_voltage}></CigaretteVoltageLighter>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <DoorState door_state={jsonData.query_params.door_state}></DoorState>
                            </td>
                            <td>
                                <Lights lights={jsonData.query_params.light_state}></Lights>
                            </td>
                            <td>
                                <BeltCard belt={jsonData.query_params.belt}></BeltCard>
                            </td>
                            <td>
                                <WindowStatusCard windows_status={jsonData.query_params.windows_closed}></WindowStatusCard>
                            </td>
                        </tr>
                    </table>
                </div>
        </div >
    )
}

export default DoorsModule
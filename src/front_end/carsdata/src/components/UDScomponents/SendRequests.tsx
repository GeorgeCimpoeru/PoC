import Link from 'next/link'
import React, { useEffect, useState } from 'react'

interface batteryData {
    battery_level: any,
    voltage: any,
    battery_state_of_charge: any,
    percentage: any,
    life_cycle: any,
    fully_charged: any,
    serial_number: any,
    range_battery: any,
    charging_time: any,
    device_consumption: any,
    time_stamp: any,
}

const SendRequests = () => {
    const [logs, setLogs] = useState<string[]>([]);
    const [data23, setData23] = useState<{ ecu_ids: [], mcu_id: any, status: string, time_stamp: string } | string[] | string | null>();
    const [batteryData, setBatteryData] = useState<batteryData | null>();
    const [canId, setCanId] = useState("");
    const [canData, setCanData] = useState("");
    const [disableFrameAndDtcBtns, setDisableFrameAndDtcBtns] = useState<boolean>(false);
    const [disableRequestIdsBtn, setDisableRequestIdsBtn] = useState<boolean>(false);
    const [disableUpdateToVersionBtn, setDisableUpdateToVersionBtn] = useState<boolean>(false);
    const [disableInfoBatteryBtns, setDisableInfoBatteryBtns] = useState<boolean>(false);
    const [disableInfoEngineBtns, setDisableInfoEngineBtns] = useState<boolean>(false);
    const [disableInfoDoorsBtns, setDisableInfoDoorsBtns] = useState<boolean>(false);
    const [disableConvertBtn, setDisableConvertBtn] = useState<boolean>(true);

    const fetchLogs = async () => {
        console.log("Fetching logs...");
        await fetch('http://127.0.0.1:5000/api/logs', {
            method: 'GET',
        }).then(response => response.json())
            .then(data => {
                setLogs(data.logs);
                console.log(data);
            })
            .catch(error => {
                console.error('Error fetching logs:', error);
            });
    }

    const hexToAscii = () => {
        let asciiString = '';
        console.log(data23.response.can_data);
        const hexArray: string[] = data23.response.can_data;

        hexArray.forEach(hexStr => {
            const decimal = parseInt(hexStr.slice(2), 16);
            asciiString += String.fromCharCode(decimal);
        });

        setData23(asciiString);
    }

    const sendFrame = async () => {
        if (!canId || !canData) {
            alert('CAN ID and CAN Data cannot be empty.');
            return;
        }
        console.log("Sending frames...");
        await fetch('http://127.0.0.1:5000/api/send_frame', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                can_id: canId,
                can_data: canData
            }),
        }).then(response => response.json())
            .then(data => {
                setData23(data);
                console.log(data);
                fetchLogs();
                setDisableConvertBtn(false);
            });
    }


    const testerPresent = async () => {
        const displayPopup = () => {
            const popup = document.createElement('div');
            popup.innerText = "Tester present is still active";
            popup.style.position = 'fixed';
            popup.style.top = '50%';
            popup.style.left = '50%';
            popup.style.transform = 'translate(-50%, -50%)';
            popup.style.padding = '20px';
            popup.style.backgroundColor = 'rgba(0, 0, 0, 0.8)';
            popup.style.color = 'white';
            popup.style.borderRadius = '10px';
            popup.style.zIndex = '1000';
            popup.style.textAlign = 'center';

            document.body.appendChild(popup);

            setTimeout(() => {
                document.body.removeChild(popup);
            }, 4000);
        };

        displayPopup();
        // setInterval(() => {
        //     displayPopup();
        // }, 10000);
    };


    const requestIds = async (initialRequest: boolean) => {
        console.log("Requesting ids...");
        try {
            await fetch('http://127.0.0.1:5000/api/request_ids', {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    if (!initialRequest) {
                        setData23(data);
                        console.log(data);
                        fetchLogs();
                    } else {
                        if (data.mcu_id == null) {
                            setDisableFrameAndDtcBtns(true);
                            setDisableRequestIdsBtn(true);
                            setDisableUpdateToVersionBtn(true);
                            setDisableInfoBatteryBtns(true);
                            setDisableInfoEngineBtns(true);
                            setDisableInfoDoorsBtns(true);
                        }
                        if (data.ecu_ids[0] == '00') {
                            setDisableInfoBatteryBtns(true);
                        } else {
                            readInfoBattery(true);
                        }
                        if (data.ecu_ids[1] == '00') {
                            setDisableInfoEngineBtns(true);
                        }
                        if (data.ecu_ids[2] == '00') {
                            setDisableInfoDoorsBtns(true);
                        }
                        if (data.ecu_ids[3] == '00') {
                            // TO DO ECU 4
                        }
                        console.log(data);
                    }
                });
        } catch (error) {
            console.log(error);
        }
    }

    const updateToVersion = async () => {
        const ecuId = prompt('Enter ECU ID:');
        const version = prompt('Enter Version:');
        console.log("Updateing version...");
        try {
            await fetch('http://127.0.0.1:5000/api/update_to_version', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ ecu_id: ecuId, version: version }),
            }).then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                });
        } catch (error) {
            console.log(error);
        }
    }

    const readInfoBattery = async (initialRequest: boolean) => {
        console.log("Reading info battery...");
        try {
            await fetch('http://127.0.0.1:5000/api/read_info_battery', {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    if (!initialRequest) {
                        setData23(data);
                        console.log(data);
                        fetchLogs();
                    } else {
                        console.log(data);
                        setBatteryData(data);
                    }
                });
        } catch (error) {
            console.log(error);
        }
    }

    const readInfoEngine = async () => { }

    const readInfoDoors = async () => {
        console.log("Reading info doors...");
        try {
            await fetch('http://127.0.0.1:5000/api/read_info_doors', {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                });
        } catch (error) {
            console.log(error);
        }
    }

    const getNewSoftVersions = async () => {
        console.log("Geting new soft versions...");
        try {
            await fetch('http://127.0.0.1:5000/api/drive_update_data', {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    const versionsArray: string[] = [];
                    for (let i = 0; i < data.children[3].children.length; ++i) {
                        versionsArray.push(data.children[3].children[i].name)
                    }
                    for (let i = 0; i < data.children[4].children.length; ++i) {
                        versionsArray.push(data.children[4].children[i].name)
                    }
                    setData23(versionsArray);
                    console.log(data);
                    fetchLogs();
                });
        } catch (error) {
            console.log(error);
        }
    }

    const writeInfoDoors = async () => {
        const door = prompt('Enter Door Parameter:');
        const serial_number = prompt('Enter Serial Number:');
        const lighter_voltage = prompt('Enter Cigarette Lighter Voltage:');
        const light_state = prompt('Enter Light State:');
        const belt = prompt('Enter Belt Card State:');
        const windows_closed = prompt('Enter Window Status:');

        const data = {
            door: door || null,
            serial_number: serial_number || null,
            lighter_voltage: lighter_voltage || null,
            light_state: light_state || null,
            belt: belt || null,
            windows_closed: windows_closed || null,
        };

        console.log("Writing info doors...");
        try {
            await fetch('http://127.0.0.1:5000/api/write_info_doors', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data),
            }).then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                });
        } catch (error) {
            console.log(error);
        }
    }

    const writeInfoBattery = async () => {
        const battery_level = prompt('Enter Battery Energy Level: ' + batteryData?.battery_level);
        const stateOfCharge = prompt('Enter Battery State of Charge: ' + batteryData?.battery_state_of_charge);
        const percentage = prompt('Enter Battery Percentage: ' + batteryData?.percentage);
        const voltage = prompt('Enter Battery Voltage: ' + batteryData?.voltage);

        // const temperature = prompt('Enter Battery Temperature:');
        // const lifeCycle = prompt('Enter Battery Life Cycle:');
        // const fullyCharged = prompt('Enter Battery Fully Charged Status:');
        // const range = prompt('Enter Battery Range:');
        // const chargingTime = prompt('Enter Battery Charging Time:');
        // const deviceConsumption = prompt('Enter Device Consumption:');

        const data2 = {
            battery_level: battery_level || null,
            battery_state_of_charge: stateOfCharge || null,
            percentage: percentage || null,
            voltage: voltage || null,
            // temperature: temperature || null,
            // life_cycle: lifeCycle || null,
            // fully_charged: fullyCharged || null,
            // range_battery: range || null,
            // charging_time: chargingTime || null,
            // device_consumption: deviceConsumption || null
        };

        console.log("Writing info battery...");
        try {
            await fetch('http://127.0.0.1:5000/api/write_info_battery', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(data2),
            })
                .then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                })
                .catch(error => {
                    console.error('Error:', error);
                });
        } catch (error) {
            console.log(error);
        }
    }

    useEffect(() => {
        requestIds(true);
    }, []);

    return (
        <div className="w-[90%] h-screen flex flex-col items-center">
            <div className="w-[50%] h-screen flex flex-col">
                <h1 className="text-3xl mt-4">CAN Interface Control</h1>
                <div>
                    <button className="btn btn-info w-fit mt-5 text-white">
                        <Link href="http://127.0.0.1:5000/apidocs/" target="_blank" rel="noopener noreferrer">Go to Docs</Link>
                    </button>
                    <button className="btn btn-warning w-fit mt-5 ml-5 text-white" onClick={testerPresent} disabled={disableFrameAndDtcBtns}>Tester present</button>

                </div>
               
                <p className="text-xl mt-3">CAN ID:</p>
                <input type="text" placeholder="e.g., 0xFa, 0x1234" className="input input-bordered w-full" onChange={e => setCanId(e.target.value)} />
                <p className="text-xl mt-3">CAN Data:</p>
                <input type="text" placeholder="e.g., 0x12, 0x34" className="input input-bordered w-full" onChange={e => setCanData(e.target.value)} />
                <div>
                    <button className="btn btn-success w-fit mt-5 text-white" onClick={sendFrame} disabled={disableFrameAndDtcBtns}>Send Frame</button>
                    <b> or </b>
                    <button className="btn btn-success w-fit mt-5 text-white" onClick={sendFrame} disabled={disableFrameAndDtcBtns}>Read DTC</button>
                    <button className="btn btn-success w-fit ml-5 mt-5 text-white" onClick={hexToAscii} disabled={disableConvertBtn}>Convert response to ASCII</button>
                    <br></br>
                    <button className="btn btn-warning w-fit mt-5 text-white" onClick={getNewSoftVersions} disabled={disableFrameAndDtcBtns}>Check new soft versions</button>
                </div>
                <div className="w-full h-px mt-4 bg-gray-300"></div>
                <div className="mt-4">
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={() => requestIds(false)} disabled={disableRequestIdsBtn}>Request IDs</button>
                    <button className="btn btn-success w-fit m-1 text-white" onClick={updateToVersion} disabled={disableUpdateToVersionBtn}>Update to version</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={() => readInfoBattery(false)} disabled={disableInfoBatteryBtns}>Read Info Battery</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoEngine} disabled={disableInfoEngineBtns}>Read Info Engine</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoDoors} disabled={disableInfoDoorsBtns}>Read Info Doors</button>
                </div>
                <div className="mt-4">
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoDoors} disabled={disableInfoDoorsBtns}>Write Doors Info</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoBattery} disabled={disableInfoBatteryBtns}>Write Battery Info</button>
                </div>
                <h1 className="text-3xl mt-4">Response</h1>
                <textarea id="response-output" className="m-2 h-36 textarea textarea-bordered" placeholder="" value={JSON.stringify(data23, null, 0)}></textarea>

                <div className="m-2 border-2 border-black overflow-x-auto max-h-52">
                    <h1 className="text-3xl mt-4">Logs:</h1>
                    <table className="table table-zebra">
                        <thead>
                            <tr>
                                <th>Index</th>
                                <th>Log Message</th>
                            </tr>
                        </thead>
                        <tbody id="log-body">
                            {logs.map((log: any, index: any) => (
                                <tr key={index}>
                                    <td align="center">{index}</td>
                                    <td align="center">{log}</td>
                                </tr>
                            ))}
                        </tbody>
                    </table>
                </div>
            </div>
        </div>
    )
}

export default SendRequests

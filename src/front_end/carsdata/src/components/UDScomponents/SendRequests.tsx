import Link from 'next/link'
import React, { useEffect, useRef, useState } from 'react'
import { clearInterval } from 'timers';

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
    const [session, setSession] = useState<string>("default");
    const [testerPres, setTesterPres] = useState<string>("disabled");
    let popupElement: any = null;
    let popupStyleElement: any = null;
    // const intervalID: any = useRef<number | null>(null);

    const displayLoadingCircle = () => {
        if (popupElement || popupStyleElement) {
            return;
        }
        popupElement = document.createElement('div');
        popupElement.style.position = 'fixed';
        popupElement.style.top = '50%';
        popupElement.style.left = '50%';
        popupElement.style.transform = 'translate(-50%, -50%)';
        popupElement.style.padding = '20px';
        popupElement.style.backgroundColor = 'rgba(0, 0, 0, 0.8)';
        popupElement.style.borderRadius = '10px';
        popupElement.style.zIndex = '1000';
        popupElement.style.textAlign = 'center';

        const loadingCircle = document.createElement('div');
        loadingCircle.style.width = '40px';
        loadingCircle.style.height = '40px';
        loadingCircle.style.border = '5px solid white';
        loadingCircle.style.borderTop = '5px solid transparent';
        loadingCircle.style.borderRadius = '50%';
        loadingCircle.style.animation = 'spin 1s linear infinite';

        popupElement.appendChild(loadingCircle);

        document.body.appendChild(popupElement);

        popupStyleElement = document.createElement('style');
        popupStyleElement.type = 'text/css';
        popupStyleElement.innerText = `@keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }}`;
        document.head.appendChild(popupStyleElement);
    };

    const removeLoadingCicle = () => {
        if (popupElement) {
            popupElement.remove();
            popupElement = null;
        }
        if (popupStyleElement) {
            popupStyleElement.remove();
            popupStyleElement = null;
        }
    };

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
        // console.log(data23.response.can_data);
        // const hexArray: string[] = data23?.response.can_data;

        // hexArray.forEach(hexStr => {
        //     const decimal = parseInt(hexStr.slice(2), 16);
        //     asciiString += String.fromCharCode(decimal);
        // });

        setData23(asciiString);
    }

    const sendFrame = async () => {
        displayLoadingCircle();
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
        removeLoadingCicle();
    }

    const readDTC = async () => {
        displayLoadingCircle();
        console.log("Reading DTC...");
        try {
            await fetch('http://127.0.0.1:5000/api/read_dtc_info', {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                });
        } catch (error) {
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const intervalID: any = useRef<NodeJS.Timeout | null>(null);

    const checkTesterPresent = async () => {
        console.log("Checking state of tester present...");
        try {
            await fetch('http://127.0.0.1:5000/api/tester_present', {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                });
        } catch (error) {
            console.log(error);
        }
    }

    const testerPresent = () => {
        console.log("Current testerPres state:", testerPres);

        if (testerPres === "disabled") {
            console.log("Starting interval (Entering 'if' branch)");

            setTesterPres("enabled"); // Change state to "enabled"

            // Start the interval if it's not already running
            if (!intervalID.current) {
                intervalID.current = window.setInterval(() => {
                    checkTesterPresent();
                    console.log("Interval running, checking tester presence...");
                }, 2000);
                console.log("Interval started with ID:", intervalID.current);
            } else {
                console.log("An interval is already running with ID:", intervalID.current);
            }

        } else {
            console.log("Stopping interval (Entering 'else' branch)");

            setTesterPres("disabled"); // Change state to "disabled"

            // Clear the interval if it exists
            console.log(intervalID.current);
            if (intervalID.current) {
                console.log(intervalID.current);
                clearInterval(intervalID.current);
                console.log("Interval stopped with ID:", intervalID.current);
                intervalID.current = null; // Reset intervalID after stopping
            } else {
                console.log("No interval to stop (intervalID is null).");
            }
        }
    };

    const requestIds = async (initialRequest: boolean) => {
        displayLoadingCircle();
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
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const updateToVersion = async () => {
        displayLoadingCircle();
        const ecuId = prompt('Enter ECU ID:');
        const version = prompt('Enter Version:');
        const updateData = {
            ecu_id: ecuId,
            version: version,
        }
        console.log(updateData)
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
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const readInfoBattery = async (initialRequest: boolean) => {
        displayLoadingCircle();
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
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const readInfoEngine = async () => { }

    const readInfoDoors = async () => {
        displayLoadingCircle();
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
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const getNewSoftVersions = async () => {
        displayLoadingCircle();
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
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const writeInfoDoors = async () => {
        displayLoadingCircle();
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
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const writeInfoBattery = async () => {
        displayLoadingCircle();
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
                    removeLoadingCicle();
                });
        } catch (error) {
            console.log(error);
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const changeSession = async () => {
        let sessiontype: any;
        if (session === "default") {
            sessiontype = {
                sub_funct: 2,
            }
        } else {
            sessiontype = {
                sub_funct: 1,
            }
        }
        displayLoadingCircle();
        console.log("Changing session...");
        try {
            await fetch('http://127.0.0.1:5000/api/change_session', {
                method: 'POST',
                mode: 'cors',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(sessiontype),
            })
                .then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                    if (data.status === "success" && session === "default") {
                        setSession("programming");
                    } else if (data.status === "success") {
                        setSession("default");
                    }
                })
                .catch(error => {
                    console.error('Error:', error);
                    removeLoadingCicle();
                });
        } catch (error) {
            console.log(error);
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const authenticate = async () => {
        console.log("Authenticating...");
        displayLoadingCircle();
        try {
            await fetch('http://127.0.0.1:5000/api/authenticate', {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                }).catch(error => {
                    console.error('Error:', error);
                    removeLoadingCicle();
                });
        } catch (error) {
            console.log(error);
            removeLoadingCicle();
        }
        removeLoadingCicle();

    }

    const readAccessTiming = async () => {
        console.log("Reading access timing...");
        let readAccessTimingType: any;
        if (testerPres === "default") {
            readAccessTimingType = {
                sub_funct: 3,
            }
        } else {
            readAccessTimingType = {
                sub_funct: 1,
            }
        }
        displayLoadingCircle();
        try {
            await fetch('http://127.0.0.1:5000/api/read_access_timing', {
                method: 'POST',
                mode: 'cors',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(readAccessTimingType),
            })
                .then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                    if (data.status === "success" && testerPres === "default") {
                        setTesterPres("programming");
                    } else if (data.status === "success") {
                        setTesterPres("default");
                    }
                })
                .catch(error => {
                    console.error(error);
                    removeLoadingCicle();
                });
        } catch (error) {
            console.log(error);
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const getIdentifiers = async () => {
        console.log("Reading all data identifiers...");
        displayLoadingCircle();
        try {
            await fetch('http://127.0.0.1:5000/api/get_identifiers', {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                }).catch(error => {
                    console.error('Error:', error);
                    removeLoadingCicle();
                });
        } catch (error) {
            console.log(error);
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    useEffect(() => {
        requestIds(true);
    }, []);

    return (
        <div className="bg-gray-100 w-[90%] h-screen flex flex-col items-center">
            <div className="w-[50%] h-screen flex flex-col">
                <h1 className="text-3xl mt-4">CAN Interface Control</h1>
                <div className="inline-flex">
                    <button className="btn btn-info w-fit mt-5 text-white">
                        <Link href="http://127.0.0.1:5000/apidocs/" target="_blank" rel="noopener noreferrer">Go to Docs</Link>
                    </button>
                    <div className="mt-5 ml-5">
                        <p>Tester present: {testerPres}</p>
                        <input type="checkbox" className="toggle toggle-info" checked={testerPres === "disabled" ? false : true} onClick={testerPresent} />
                        {/* <>checked?????????????</> */}
                    </div>
                    <div className="mt-5 ml-5">
                        <p>Session: {session}</p>
                        <input type="checkbox" className="toggle toggle-info" checked={session === "default" ? false : true} onClick={changeSession} />
                        {/* <>checked?????????????</> */}
                    </div>
                </div>
                <div>
                    <p className="text-xl mt-3">CAN ID:</p>
                    <input type="text" placeholder="e.g., 0xFa, 0x1234" className="input input-bordered w-full" onChange={e => setCanId(e.target.value)} />
                    <p className="text-xl mt-3">CAN Data:</p>
                    <input type="text" placeholder="e.g., 0x12, 0x34" className="input input-bordered w-full" onChange={e => setCanData(e.target.value)} />
                    <button className="btn btn-success w-fit mt-5 text-white" onClick={sendFrame} disabled={disableFrameAndDtcBtns}>Send Frame</button>
                </div>
                <div className="w-full h-px mt-4 bg-gray-300"></div>
                <div>
                    <button className="btn btn-success w-fit mt-5 text-white" onClick={readDTC} disabled={disableFrameAndDtcBtns}>Read DTC</button>
                    {/* <button className="btn btn-success w-fit ml-5 mt-5 text-white" onClick={hexToAscii} disabled={disableConvertBtn}>Convert response to ASCII</button> */}
                    <br></br>
                    <button className="btn btn-warning w-fit ml-1 mt-5 text-white" onClick={getNewSoftVersions} disabled={disableFrameAndDtcBtns}>Check new soft versions</button>
                    <button className="btn btn-warning w-fit ml-1 mt-5 text-white" onClick={authenticate} disabled={disableFrameAndDtcBtns}>Authenticate</button>
                    <button className="btn btn-warning w-fit ml-1 mt-5 text-white" onClick={readAccessTiming} disabled={disableFrameAndDtcBtns}>Read access timing</button>
                    <button className="btn btn-warning w-fit ml-1 mt-5 text-white" onClick={getIdentifiers} disabled={disableFrameAndDtcBtns}>Read identifiers</button>
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
        </div >
    )
}

export default SendRequests

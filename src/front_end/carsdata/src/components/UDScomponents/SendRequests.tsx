import Link from 'next/link';
import Image from 'next/image';
import React, { useEffect, useState } from 'react';

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

let intervalID: number | NodeJS.Timeout | null = null;

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
    const [accessTiming, setAccessTiming] = useState<string>("current");
    const [selectedECUid, setSelectedECUid] = useState<string>("");
    const [selectedECU, setSelectedECU] = useState<string>("Select ECU");
    const [isDropdownOpen, setIsDropdownOpen] = useState(false);
    let popupElement: HTMLDivElement | null = null;
    let popupStyleElement: HTMLStyleElement | null = null;
    let overlayElement: HTMLDivElement | null = null;

    const displayLoadingCircle = () => {
        if (popupElement || popupStyleElement || overlayElement) {
            return;
        }

        overlayElement = document.createElement('div');
        overlayElement.style.position = 'fixed';
        overlayElement.style.top = '0';
        overlayElement.style.left = '0';
        overlayElement.style.width = '100vw';
        overlayElement.style.height = '100vh';
        overlayElement.style.backgroundColor = 'rgba(0, 0, 0, 0.5)';
        overlayElement.style.zIndex = '999';
        overlayElement.style.pointerEvents = 'all';
        overlayElement.style.cursor = 'not-allowed';

        document.body.appendChild(overlayElement);

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
        popupStyleElement.innerText = `
            @keyframes spin {
                0% { transform: rotate(0deg); }
                100% { transform: rotate(360deg); }
            }`;
        document.head.appendChild(popupStyleElement);
    };

    const displayMessagePopup = (text: string) => {
        const popup = document.createElement('div');
        popup.innerText = text;
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
        }, 2000);
    };

    const removeLoadingCicle = () => {
        if (popupElement && popupStyleElement && overlayElement) {
            document.body.removeChild(popupElement);
            document.head.removeChild(popupStyleElement);
            document.body.removeChild(overlayElement);

            popupElement = null;
            popupStyleElement = null;
            overlayElement = null;
        }
    };

    const fetchLogs = async () => {
        console.log("Fetching logs...");
        await fetch(`http://127.0.0.1:5000/api/logs`, {
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
        if (!canId || !canData) {
            alert('CAN ID and CAN Data cannot be empty.');
            return;
        }
        displayLoadingCircle();
        console.log("Sending frames...");
        console.log({
            can_id: canId,
            can_data: canData
        });
        await fetch(`http://127.0.0.1:5000/api/send_frame`, {
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
            await fetch(`http://127.0.0.1:5000/api/read_dtc_info`, {
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
        displayLoadingCircle();
        console.log("Requesting ids...");
        try {
            await fetch(`http://127.0.0.1:5000/api/request_ids`, {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    console.log(data);
                    if (!initialRequest) {
                        setData23(data);
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
                            readInfoBattery(true, "");
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
        console.log({ ecu_id: ecuId, version: version });
        console.log("Updateing version...");
        try {
            await fetch(`http://127.0.0.1:5000/api/update_to_version`, {
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

    const readInfoBattery = async (initialRequest: boolean, identifier: string) => {
        displayLoadingCircle();
        console.log("Reading info battery...");
        console.log(identifier);
        try {
            await fetch(`http://127.0.0.1:5000/api/read_info_battery?identifier=${identifier}`, {
                method: 'GET',
                // mode: 'no-cors',
            }).then(response => response.json())
                .then(data => {
                    console.log(data);
                    if (!initialRequest) {
                        setData23(data);
                        fetchLogs();
                    } else {
                        setBatteryData(data);
                    }
                    removeLoadingCicle();
                });
        } catch (error) {
            console.log(error);
            removeLoadingCicle();
        }
        // removeLoadingCicle();
    }

    const readInfoEngine = async () => { }
    const writeInfoEngine = async () => { }

    const readInfoDoors = async () => {
        displayLoadingCircle();
        console.log("Reading info doors...");
        try {
            await fetch(`http://127.0.0.1:5000/api/read_info_doors`, {
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
            await fetch(`http://127.0.0.1:5000/api/drive_update_data`, {
                method: 'GET',
            }).then(response => response.json())
                .then(data => {
                    console.log(data);
                    const versionsArray: string[] = [];
                    for (let i = 0; i < data.children[4].children.length; ++i) {
                        versionsArray.push(data.children[4].children[i].name)
                    }
                    for (let i = 0; i < data.children[5].children.length; ++i) {
                        versionsArray.push(data.children[5].children[i].name)
                    }
                    setData23(versionsArray);
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
        console.log(data);
        try {
            await fetch(`http://127.0.0.1:5000/api/write_info_doors`, {
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

    const writeInfoBattery = async (identifier: string) => {
        let data2 = {}
        if (identifier === "battery_level") {
            let batteryLevel = prompt('Enter Battery Level: ');
            if (batteryLevel === null) {
                return;
            }
            data2 = {
                battery_level: parseInt(batteryLevel)
            };
        } else if (identifier === "state_of_charge") {
            let stateOfCharge = prompt('Enter Battery State of Charge: ');
            if (stateOfCharge === null) {
                return;
            }
            data2 = {
                state_of_charge: parseInt(stateOfCharge)
            };
        } else if (identifier === "percentage") {
            let percentage = prompt('Enter Battery Percentage: ');
            if (percentage === null) {
                return;
            }
            data2 = {
                percentage: parseInt(percentage)
            };
        } else if (identifier === "voltage") {
            let voltage = prompt('Enter Battery Voltage: ');
            if (voltage === null) {
                return;
            }
            data2 = {
                voltage: parseInt(voltage)
            };
        }

        console.log("Writing info battery...");
        console.log(data2);
        displayLoadingCircle();
        try {
            await fetch(`http://127.0.0.1:5000/api/write_info_battery?identifier=${identifier}`, {
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
        console.log(sessiontype);
        try {
            await fetch(`http://127.0.0.1:5000/api/change_session`, {
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
            await fetch(`http://127.0.0.1:5000/api/authenticate`, {
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

    const readAccessTiming = async () => {
        console.log("Reading access timing...");
        let readAccessTimingType: any;
        if (accessTiming === "current") {
            readAccessTimingType = {
                sub_funct: 3,
            }
        } else {
            readAccessTimingType = {
                sub_funct: 1,
            }
        }
        console.log(readAccessTimingType);
        displayLoadingCircle();
        try {
            await fetch(`http://127.0.0.1:5000/api/read_access_timing`, {
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
                    if (data.status === "success" && accessTiming === "current") {
                        setAccessTiming("default");
                        displayMessagePopup("Current access timing")
                    } else if (data.status === "success") {
                        setAccessTiming("current");
                        displayMessagePopup("Default access timing")
                    }
                })
        } catch (error) {
            console.log(error);
            displayMessagePopup("Connection failed");
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const getIdentifiers = async () => {
        console.log("Reading all data identifiers...");
        displayLoadingCircle();
        try {
            await fetch(`http://127.0.0.1:5000/api/get_identifiers`, {
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

    const checkTesterPresent = async () => {
        console.log("Checking state of tester present...");
        try {
            await fetch(`http://127.0.0.1:5000/api/tester_present`, {
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
        if (testerPres === "disabled") {
            setTesterPres("enabled");
            if (!intervalID) {
                intervalID = window.setInterval(() => {
                    checkTesterPresent();
                }, 2000);
            } else {
                console.log("An interval is already running with ID:", intervalID);
            }

        } else {
            console.log("Stopping interval (Entering 'else' branch)");
            setTesterPres("disabled");
            if (intervalID) {
                clearInterval(intervalID);
                console.log("Interval stopped with ID:", intervalID);
                intervalID = null;
            } else {
                console.log("No interval to stop (intervalID is null).");
            }
        }
    };

    const ecuReset = async (resetType: string) => {
        if (selectedECUid === "") {
            displayMessagePopup("Select ECU");
            return;
        }
        console.log("Reseting ECU...");
        console.log({
            ecu_id: selectedECUid,
            type_reset: resetType
        });
        displayLoadingCircle();
        try {
            await fetch(`http://127.0.0.1:5000/api/reset_ecu`, {
                method: 'POST',
                // mode: 'no-cors',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    ecu_id: selectedECUid,
                    type_reset: resetType
                }),
            })
                .then(response => response.json())
                .then(data => {
                    setData23(data);
                    console.log(data);
                    fetchLogs();
                    displayMessagePopup(selectedECUid + " reseted");
                })
        } catch (error) {
            console.log(error);
            displayMessagePopup("Connection failed");
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    useEffect(() => {
        requestIds(true);
    }, []);

    return (
        <div className="bg-gray-100 w-[90%] h-screen flex flex-col items-center">
            <div className="w-[60%] h-screen flex flex-col">
                <h1 className="text-3xl mt-2">CAN Interface Control</h1>
                <div className="inline-flex">
                    <button className="btn btn-info w-fit mt-2 text-white">
                        <Link href="http://127.0.0.1:5000/apidocs/" target="_blank" rel="noopener noreferrer">Go to Docs</Link>
                    </button>
                    <div className="mt-2 ml-5">
                        <p>Tester present: {testerPres}</p>
                        <input type="checkbox" className="toggle toggle-info" checked={testerPres === "disabled" ? false : true} onClick={testerPresent} />
                        {/* <>checked?????????????</> */}
                    </div>
                    <div className="mt-2 ml-5">
                        <p>Session: {session}</p>
                        <input type="checkbox" className="toggle toggle-info" checked={session === "default" ? false : true} onClick={changeSession} />
                        {/* <>checked?????????????</> */}
                    </div>
                    <div className="mt-2 ml-5">
                        <p>Read {accessTiming} access timing</p>
                        <input type="checkbox" className="toggle toggle-info" checked={accessTiming === "current" ? false : true} onClick={readAccessTiming} />
                        {/* <>checked?????????????</> */}
                    </div>
                    <div className="mt-2 ml-5 border-2 border-black">
                        <p>ECU reset</p>
                        <div className="dropdown">
                            <button tabIndex={0} className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white relative" onClick={() => setIsDropdownOpen(!isDropdownOpen)}>
                                {selectedECU}
                                <Image
                                    src="/dropdownarrow.png"
                                    alt="Dropdown arrow icon"
                                    className="dark:invert m-1 hover:object-scale-down"
                                    width={10}
                                    height={10}
                                    priority
                                />
                            </button>
                            {isDropdownOpen && (
                                <ul tabIndex={0} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                    <li><a onClick={() => { setSelectedECUid("10"); setSelectedECU("MCU"); setIsDropdownOpen(false) }}>MCU</a></li>
                                    <li><a onClick={() => { setSelectedECUid("11"); setSelectedECU("Battery"); setIsDropdownOpen(false) }}>Battery</a></li>
                                    {/* <li><a onClick={() => { setSelectedECUid("12"); setSelectedECU("Engine"); setIsDropdownOpen(false) }}>Engine</a></li>
                                    <li><a onClick={() => { setSelectedECUid("13"); setSelectedECU("Doors"); setIsDropdownOpen(false) }}>Doors</a></li>
                                    <li><a onClick={() => { setSelectedECUid("14"); setSelectedECU("HVAC"); setIsDropdownOpen(false) }}>HVAC</a></li> */}
                                </ul>
                            )}
                        </div>

                        <div className="dropdown">
                            <button tabIndex={1} className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white relative" onClick={() => setIsDropdownOpen(!isDropdownOpen)}>
                                Reset type
                                <Image
                                    src="/dropdownarrow.png"
                                    alt="Dropdown arrow icon"
                                    className="dark:invert m-1 hover:object-scale-down"
                                    width={10}
                                    height={10}
                                    priority
                                />
                            </button>
                            {isDropdownOpen && (
                                <ul tabIndex={1} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                    <li><a onClick={() => { ecuReset("soft"); setIsDropdownOpen(false) }}>Soft</a></li>
                                    <li><a onClick={() => { ecuReset("hard"); setIsDropdownOpen(false) }}>Hard</a></li>
                                </ul>
                            )}
                        </div>
                    </div>
                </div>
                <div>
                    <p className="text-xl mt-2">CAN ID:</p>
                    <input type="text" placeholder="e.g., 0xFa, 0x1234" className="input input-bordered w-full" onChange={e => setCanId(e.target.value)} />
                    <p className="text-xl mt-2">CAN Data:</p>
                    <input type="text" placeholder="e.g., 0x12, 0x34" className="input input-bordered w-full" onChange={e => setCanData(e.target.value)} />
                    <button className="btn btn-success w-fit mt-2 text-white" onClick={sendFrame} disabled={disableFrameAndDtcBtns}>Send Frame</button>
                </div>
                <div className="w-full h-px mt-2 bg-gray-300"></div>
                <div>
                    <button className="btn btn-success w-fit mt-2 text-white" onClick={readDTC} disabled={disableFrameAndDtcBtns}>Read DTC</button>
                    {/* <button className="btn btn-success w-fit ml-5 mt-2 text-white" onClick={hexToAscii} disabled={disableConvertBtn}>Convert response to ASCII</button> */}
                    <button className="btn btn-warning w-fit ml-1 mt-2 text-white" onClick={getNewSoftVersions} disabled={disableFrameAndDtcBtns}>Check new soft versions</button>
                    <button className="btn btn-warning w-fit ml-1 mt-2 text-white" onClick={authenticate} disabled={disableFrameAndDtcBtns}>Authenticate</button>
                    <button className="btn btn-warning w-fit ml-1 mt-2 text-white" onClick={getIdentifiers} disabled={disableFrameAndDtcBtns}>Read identifiers</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={() => requestIds(false)} disabled={disableRequestIdsBtn}>Request IDs</button>
                </div>
                <div className="w-full h-px mt-2 bg-gray-300"></div>
                <div className="mt-2">
                    <div className="dropdown">
                        <button tabIndex={2} className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white relative" onClick={() => setIsDropdownOpen(!isDropdownOpen)} disabled={disableInfoBatteryBtns}>
                            Read battery param
                            <Image
                                src="/dropdownarrow.png"
                                alt="Dropdown arrow icon"
                                className="dark:invert m-1 hover:object-scale-down"
                                width={10}
                                height={10}
                                priority
                            />
                        </button>
                        {isDropdownOpen && (
                            <ul tabIndex={2} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(false, "") }}>All params</a></li>
                                <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(false, "battery_level") }}>Battery level</a></li>
                                <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(false, "state_of_charge") }}>State of charge</a></li>
                                <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(false, "percentage") }}>Percentage</a></li>
                                <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(false, "voltage") }}>Voltage</a></li>
                            </ul>
                        )}
                    </div>
                    <div className="dropdown">
                        <button tabIndex={3} className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white relative" onClick={() => setIsDropdownOpen(!isDropdownOpen)} disabled={disableInfoBatteryBtns}>
                            Write battery param
                            <Image
                                src="/dropdownarrow.png"
                                alt="Dropdown arrow icon"
                                className="dark:invert m-1 hover:object-scale-down"
                                width={10}
                                height={10}
                                priority
                            />
                        </button>
                        {isDropdownOpen && (
                            <ul tabIndex={3} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                <li><a onClick={() => { setIsDropdownOpen(false); writeInfoBattery("battery_level") }}>Battery level</a></li>
                                <li><a onClick={() => { setIsDropdownOpen(false); writeInfoBattery("state_of_charge") }}>State of charge</a></li>
                                <li><a onClick={() => { setIsDropdownOpen(false); writeInfoBattery("percentage") }}>Percentage</a></li>
                                <li><a onClick={() => { setIsDropdownOpen(false); writeInfoBattery("voltage") }}>Voltage</a></li>
                            </ul>
                        )}
                    </div>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoEngine} disabled={disableInfoEngineBtns}>Read Info Engine</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoEngine} disabled={disableInfoEngineBtns}>Write Info Engine</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoDoors} disabled={disableInfoDoorsBtns}>Read Info Doors</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoDoors} disabled={disableInfoDoorsBtns}>Write Doors Info</button>
                </div>
                <h1 className="text-2xl mt-2">Response</h1>
                <textarea id="response-output" className="m-2 h-36 textarea textarea-bordered" placeholder="" value={JSON.stringify(data23, null, 0)}></textarea>

                <div className="m-2 border-2 border-black overflow-x-auto max-h-52">
                    <h1 className="text-2xl mt-2">Logs:</h1>
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

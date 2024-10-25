import Link from 'next/link';
import Image from 'next/image';
import React, { useEffect, useState } from 'react';
import { batteryData, readInfoBattery } from './DivCenterBattery';
import { writeInfoBattery } from './DivCenterBattery';
import ModalUDS from './ModalUDS';
import { displayLoadingCircle, displayErrorPopup, removeLoadingCicle } from '../sharedComponents/LoadingCircle';
import logger from '@/src/utils/Logger';


let intervalID: number | NodeJS.Timeout | null = null;

const SendRequests = () => {
    logger.init();
    
    const [logs, setLogs] = useState<string[]>([]);
    const [data23, setData23] = useState<{ ecu_ids: [], mcu_id: any, status: string, time_stamp: string } | string[] | string | null |
        { name: string; version: string; }[]>();
    const [varBatteryData, setVarBatteryData] = useState<batteryData | null>();
    const [canId, setCanId] = useState("");
    const [canData, setCanData] = useState("");
    const [disableFrameAndDtcBtns, setDisableFrameAndDtcBtns] = useState<boolean>(false);
    const [disableRequestIdsBtn, setDisableRequestIdsBtn] = useState<boolean>(false);
    const [disableUpdateToVersionBtn, setDisableUpdateToVersionBtn] = useState<boolean>(false);
    const [disableInfoBatteryBtns, setDisableInfoBatteryBtns] = useState<boolean>(false);
    const [disableInfoEngineBtns, setDisableInfoEngineBtns] = useState<boolean>(false);
    const [disableInfoDoorsBtns, setDisableInfoDoorsBtns] = useState<boolean>(false);
    const [disableInfoHvacBtns, setDisableInfoHvacsBtns] = useState<boolean>(false);
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

    const fetchLogs = async () => {
        displayLoadingCircle();
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
                displayErrorPopup("Connection failed");
                removeLoadingCicle();
            });
        removeLoadingCicle();
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
            displayErrorPopup("can't read DTC ");
        }
        removeLoadingCicle();
    }

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
                            readInfoBattery(true, setData23);
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
            displayErrorPopup("Connection failed");
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
            displayErrorPopup("Connection failed");
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const writeInfoEngine = async () => {
        displayLoadingCircle();
        const engine_rpm = prompt('Enter Engine rpm:');
        const coolant_temperature = prompt('Enter coolant temperature:');
        const throttle_position = prompt('Enter throttle position:');
        const vehicle_speed = prompt('Enter vehicle speed:');
        const engine_load = prompt('Enter engine load:');
        const fuel_level = prompt('Enter fuel level:');
        const oil_temperature = prompt('Enter oil temperature:');
        const fuel_pressure = prompt('Enter fuel pressure:');
        const intake_air_temperature = prompt('Enter intake air temperature:');
        const is_manual_flow = true;

        const data = {
            engine_rpm: engine_rpm || null,
            coolant_temperature: coolant_temperature || null,
            throttle_position: throttle_position || null,
            vehicle_speed: vehicle_speed || null,
            engine_load: engine_load || null,
            fuel_level: fuel_level || null,
            oil_temperature: oil_temperature || null,
            fuel_pressure: fuel_pressure || null,
            intake_air_temperature: intake_air_temperature || null,
            is_manual_flow: is_manual_flow || null


        };
        console.log("Writing info engine...");
        console.log(data);
        try {
            await fetch(`http://127.0.0.1:5000/api/write_info_engine?is_manual_flow=true`, {
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
            displayErrorPopup("Connection failed");
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

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
    };

    const getNewSoftVersions = async (): Promise<{ message: string; versions: { name: string; version: string }[] }> => {
        displayLoadingCircle();
        const responseContainer = document.getElementById('response-data');

        if (responseContainer) {
            responseContainer.innerHTML = "<p>Getting new soft versions...</p>";
        }

        let versionsArray: { name: string; version: string }[] = [];

        try {
            const response = await fetch(`http://127.0.0.1:5000/api/drive_update_data`, {
                method: 'GET',
            });

            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }

            const data = await response.json();
            console.log("Response data:", data);

            if (data && data.children && data.children.length > 0) {

                for (const folder of data.children) {
                    console.log("Processing folder:", folder.name);

                    for (const child of folder.children) {
                        const fullName: string = child.name;
                        const version: string = getVersion(fullName);
                        console.log(`Found file: ${fullName}, version: ${version}`);
                        const nameWithoutId: string = fullName.split('.zip')[0];
                        versionsArray.push({ name: nameWithoutId, version });
                    }
                };

                console.log("Versions array:", versionsArray);


                const searchTerms = ["HVAC", "battery", "engine", "doors"];
                const filteredVersions = getElementByName(versionsArray, searchTerms);
                console.log("Filtered ECU Versions:", filteredVersions);

                setData23(versionsArray);

                return {
                    message: "Versions retrieved successfully",
                    versions: filteredVersions
                };
            } else {
                return { message: "No versions available", versions: [] };
            }
        } catch (error) {
            console.error("Error:", error);
            const errorMessage = (error instanceof Error) ? error.message : "Unknown error occurred";

            if (responseContainer) {
                responseContainer.innerHTML += `<h3>Error:</h3><p>${errorMessage}</p>`;
            }

            return {
                message: "Failed to retrieve versions",
                versions: []
            };
        } finally {
            removeLoadingCicle();
        }
    };

    const getVersion = (fullName: string): string => {
        const versionMatch = fullName.match(/_(\d+\.\d+)\.zip/);
        console.log(`Extracting version from: ${fullName}`);
        return versionMatch ? versionMatch[1] : "unknown";
    };  // to be removed 

    const getElementByName = (versionsArray: { name: string; version: string }[], searchTerms: string[]) => {
        const filteredVersions = versionsArray.filter(version =>
            searchTerms.some(term => version.name.toLowerCase().includes(term.toLowerCase()))
        );
        return filteredVersions;
    };

    const checkInput = (message: any) => {
        let value;
        do {
            value = prompt(message);
            if (value !== '0' && value !== '1') {
                alert('Accepted value: 0/1');
            }
        } while (value !== '0' && value !== '1');
        return value;
    };


    const writeInfoHvac = async () => {
        displayLoadingCircle();
        const door = checkInput('Enter Door Parameter:');
        const passenger = checkInput('Enter Passenger:');
        const passenger_lock = checkInput('Enter Passenger Lock:');
        const driver = checkInput('Enter Driver:');
        const ajar = checkInput('Enter Ajar:');
        const is_manual_flow = true;
        // const windows_closed = prompt('Enter Window Status:');

        const data = {
            door: door || null,
            passenger: passenger || null,
            passenger_lock: passenger_lock || null,
            driver: driver || null,
            ajar: ajar || null,
            is_manual_flow: is_manual_flow || null,
            // windows_closed: windows_closed || null,
        };
        console.log("Writing info hvac...");
        console.log(data);
        try {
            await fetch(`http://127.0.0.1:5000/api/write_info_hvac`, {
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

    const writeInfoHvac = async () => {
        displayLoadingCircle();
        const mass_air_flow = prompt('Enter Mass Air Flow:');
        const ambient_air_temperature = prompt('Enter Ambient Air Temperature:');
        const cabin_temperature = prompt('Enter Cabin Temperature:');
        const cabin_temperature_driver_set = prompt('Enter Cabin Temperature Driver Set:');
        const fan_speed = prompt('Enter Fan Speed:');
        const hvac_modes = checkInput('Enter Hvac Modes:');
        const is_manual_flow = true;

        const data = {
            mass_air_flow: mass_air_flow || null,
            ambient_air_temperature: ambient_air_temperature || null,
            cabin_temperature: cabin_temperature || null,
            cabin_temperature_driver_set: cabin_temperature_driver_set || null,
            fan_speed: fan_speed || null,
            hvac_modes: hvac_modes || null,
            is_manual_flow: is_manual_flow || null
        };
        console.log("Writing info hvac...");
        console.log(data);
        try {
            await fetch(`http://127.0.0.1:5000/api/write_info_hvac`, {
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
                        displayErrorPopup("Current access timing")
                    } else if (data.status === "success") {
                        setAccessTiming("current");
                        displayErrorPopup("Default access timing")
                    }
                })
        } catch (error) {
            console.log(error);
            displayErrorPopup("Failed to read access timing");
            removeLoadingCicle();
        }
        removeLoadingCicle();
    }

    const writeTiming = async () => {

        const timingData = {
            p2_max: parseInt(prompt("Enter p2_max value:") || "0", 10),
            p2_star_max: parseInt(prompt("Enter p2_star_max value:") || "0", 10)
        };

        console.log("Writing timing data...", timingData);
        displayLoadingCircle();

        try {
            const response = await fetch(`http://127.0.0.1:5000/api/write_timing`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(timingData),
            });

            const data = await response.json();
            console.log("Response data:", data);

            if (data.status === "success") {
                setData23(data)
                console.log(data)

                const writtenValues = data.written_values;
                const message = `Timing parameters written successfully.\n` +
                    `New P2 Max Time: ${writtenValues["New P2 Max Time"]}\n` +
                    `New P2 Star Max: ${writtenValues["New P2 Star Max"]}`;

                    displayErrorPopup(message);
            } else {
                displayErrorPopup(`Error: ${data.message}`);
            }

            fetchLogs();

        } catch (error) {
            console.error("Error:", error);
            displayErrorPopup("Failed to write timing values");
            removeLoadingCicle();
        }

        removeLoadingCicle();
    };

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
            displayErrorPopup("Select ECU");
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
                    displayErrorPopup(selectedECUid + " reseted");
                })
        } catch (error) {
            console.log(error);
            displayErrorPopup("Connection failed");
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
                    <div className="flex flex-col">
                        <button className="btn btn-info w-fit mt-2 text-white">
                            <Link href="http://127.0.0.1:5000/apidocs/" target="_blank" rel="noopener noreferrer">Go to Docs</Link>
                        </button>
                        <button className="btn btn-info w-fit mt-2 text-white" onClick={() => logger.downloadLogs()}>
                            Download Logs
                        </button>
                    </div>
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
                    <button className="btn btn-warning w-fit ml-1 mt-2 text-white" onClick={getNewSoftVersions}>Check new soft versions</button>
                    <button className="btn btn-warning w-fit ml-1 mt-2 text-white" onClick={authenticate} disabled={disableFrameAndDtcBtns}>Authenticate</button>
                    <button className="btn btn-warning w-fit ml-1 mt-2 text-white" onClick={getIdentifiers} disabled={disableFrameAndDtcBtns}>Read identifiers</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={() => requestIds(false)} disabled={disableRequestIdsBtn}>Request IDs</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeTiming}>Write Timing</button>
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
                        {/* {isDropdownOpen && ( */}
                        <ul tabIndex={2} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                            <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(true, setData23) }}>All params</a></li>
                            <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(true, setData23) }}>Battery level</a></li>
                            <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(true, setData23) }}>State of charge</a></li>
                            <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(true, setData23) }}>Percentage</a></li>
                            <li><a onClick={() => { setIsDropdownOpen(false); readInfoBattery(true, setData23) }}>Voltage</a></li>
                        </ul>
                        {/* )} */}
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
                            <div>
                                <ul tabIndex={3} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                    <li>
                                        <label htmlFor="my_modal_1">
                                            <a onClick={() => { setIsDropdownOpen(false) }} >Battery level</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_1">
                                            <a onClick={() => { setIsDropdownOpen(false) }} >State of charge</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_1">
                                            <a onClick={() => { setIsDropdownOpen(false) }} >Percentage</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_1">
                                            <a onClick={() => { setIsDropdownOpen(false) }} >Voltage</a>
                                        </label>
                                    </li>
                                </ul>
                                <ModalUDS id="my_modal_1" cardTitle={'Battery level'} writeInfo={writeInfoBattery} param="battery_level" setter={setData23} />
                            </div>
                        )}

                    </div>
                    {/* <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoEngine} disabled={disableInfoEngineBtns}>Read Info Engine</button> */}
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoEngine} disabled={disableInfoEngineBtns}>Write Info Engine</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoDoors} disabled={disableInfoDoorsBtns}>Read Info Doors</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoDoors} disabled={disableInfoDoorsBtns}>Write Doors Info</button>
                    {/* <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoHvac} disabled={disableInfoHvacBtns}>Read Info Hvac</button> */}
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoHvac} disabled={disableInfoHvacBtns}>Write Info Hvac</button>
                </div>

                <h1 className="text-2xl mt-2">Response</h1>
                {data23 && (
                    <ul className="m-2 p-2 list-disc">
                        {Object.entries(data23).map(([key, value]) => (
                            <li key={key}>
                                <strong>{key}:</strong> {JSON.stringify(value)}
                            </li>
                        ))}
                    </ul>
                )}

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
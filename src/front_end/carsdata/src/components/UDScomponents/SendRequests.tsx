import Link from 'next/link';
import Image from 'next/image';
import React, { useEffect, useState } from 'react';
import ModalUDS from './ModalUDS';
import { displayLoadingCircle, displayErrorPopup, removeLoadingCicle } from '../sharedComponents/LoadingCircle';
import { batteryData, readInfoBattery, writeInfoBattery } from './DivCenterBattery';
import { engineData, readInfoEngine, writeInfoEngine } from './DivCenterEngine';
import { doorsData, readInfoDoors, writeInfoDoors } from './DivCenterDoors';
import { HVACData, readInfoHVAC, writeInfoHvac } from './DivCenterHVAC';
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
    const [isDropDownOpenBattery, setIsDropdownOpenBattery] = useState(false);
    const [isDropDownOpenEngine, setIsDropdownOpenEngine] = useState(false);
    const [isDropdownOpenHVAC, setIsDropdownOpenHVAC] = useState(false);
    const [engineCardTitle, setEngineCardTitle] = useState("");
    const [hvacCardTitle, setHvacCardTitle] = useState("");
    const [paramEngine, setParamEngine] = useState("");
    const [paramHvac, setParamHvac] = useState("");


    const dropdownEngineItemClick = (cardTitle: string, param: string) => {
        setIsDropdownOpenEngine(false);
        setEngineCardTitle(cardTitle);
        setParamEngine(param);
    }

    const dropdownHVACItemClick = (cardTitle: string, param: string) => {
        setIsDropdownOpenHVAC(false);
        setHvacCardTitle(cardTitle);
        setParamHvac(param);
    }

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

    // Send a CAN frame to the server
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

    // Read Diagnostic Trouble Codes (DTC)
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
                        /* making a single block for each function */
                        if (data.mcu_id == null) {
                            [setDisableFrameAndDtcBtns, setDisableRequestIdsBtn, setDisableUpdateToVersionBtn,
                             setDisableInfoBatteryBtns, setDisableInfoEngineBtns, setDisableInfoDoorsBtns]
                            .forEach(setFunction => setFunction(true));
                        }
                        /* map with all ecu to activate and deactivate functions */
                        const ecuMappings = [
                            { index: 0, disableFunction: setDisableInfoBatteryBtns, enableFunction: () => readInfoBattery(true, setData23) },
                            { index: 1, disableFunction: setDisableInfoEngineBtns },
                            { index: 2, disableFunction: setDisableInfoDoorsBtns },
                            { index: 3, disableFunction: setDisableInfoHvacsBtns }
                        ];
                        /* map iterration to apply enable/disable functions */
                        ecuMappings.forEach(({ index, disableFunction, enableFunction }) => {
                            data.ecu_ids[index] === '00' ? disableFunction(true) : enableFunction?.();
                        });
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


    const getNewSoftVersions = async (): Promise<{ message: string; versions: { name: string; version: string }[] }> => {
        displayLoadingCircle();
        const responseContainer = document.getElementById('response-data');

        responseContainer && (responseContainer.innerHTML = "<p>Getting new soft versions...</p>");

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
    }; // will be moved only in DivCenterHvac

    const changeSession = async () => {
        let sessiontype: any;
        
        // Define session type based on the current session state
        sessiontype = {
            sub_funct: session === "default" ? 2 : 1
        };

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
                    data.status === "success" ? setSession(session === "default" ? "programming" : "default") : setSession;

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
        
        readAccessTimingType = {
            sub_funct: accessTiming === "current" ? 3 : 1
        };
        
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

                    if (data.status === "success") {
                        setAccessTiming(accessTiming === "current" ? "default" : "current");
                        displayErrorPopup(accessTiming === "current" ? "Current access timing" : "Default access timing");
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

    // Reset the ECU based on the selected ECU ID and reset type
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
                        <button tabIndex={3} className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white relative" onClick={() => setIsDropdownOpenBattery(!isDropDownOpenBattery)} disabled={disableInfoBatteryBtns}>
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
                        {isDropDownOpenBattery && (
                            <div>
                                <ul tabIndex={3} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                    <li>
                                        <label htmlFor="my_modal_7">
                                            <a onClick={() => { setIsDropdownOpenBattery(false) }} >Battery level</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_7">
                                            <a onClick={() => { setIsDropdownOpenBattery(false) }} >State of charge</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_7">
                                            <a onClick={() => { setIsDropdownOpenBattery(false) }} >Percentage</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_7">
                                            <a onClick={() => { setIsDropdownOpenBattery(false) }} >Voltage</a>
                                        </label>
                                    </li>
                                </ul>
                                <ModalUDS id="my_modal_7" cardTitle={'Battery level'} writeInfo={writeInfoBattery} param="battery_level" setter={setData23} />
                            </div>
                        )}

                    </div>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={() => { setIsDropdownOpen(false); readInfoEngine(true, setData23) }}>Read Info Engine</button>

                    <div className="dropdown">
                        <button tabIndex={4} className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white relative" onClick={() => setIsDropdownOpenEngine(!isDropDownOpenEngine)} disabled={disableInfoEngineBtns}>
                            Write engine param
                            <Image
                                src="/dropdownarrow.png"
                                alt="Dropdown arrow icon"
                                className="dark:invert m-1 hover:object-scale-down"
                                width={10}
                                height={10}
                                priority
                            />
                        </button>
                        {isDropDownOpenEngine && (
                            <div>
                                <ul tabIndex={4} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("Coolant temperature", "coolant_temperature") }} >Coolant temperature</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("Load", "engine_load") }} >Engine load</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("RPM", "engine_rpm") }} >RPM</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("Fuel", "fuel_level") }} >Fuel</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("Fuel pressure", "Fuel pressure") }} >Fuel pressure</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("Intake air temperature", "intake_air_temperature") }} >Intake air temperature</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("Oil temperature", "oil_temperature") }} >Oil temperature</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("Throttle position", "throttle_position") }} >Throttle position</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_8">
                                            <a onClick={() => { dropdownEngineItemClick("Speed", "vehicle_speed") }} >Speed</a>
                                        </label>
                                    </li>
                                </ul>
                                <ModalUDS id="my_modal_8" cardTitle={engineCardTitle} writeInfo={writeInfoBattery} param={paramEngine} setter={setData23} />
                            </div>
                        )}

                    </div>
                    <button
                        className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white"
                        onClick={() => readInfoDoors(true, setData23)}
                        disabled={disableInfoDoorsBtns}
                    >
                        Read Info Doors
                    </button>

                    <button
                        className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white"
                        onClick={() => writeInfoDoors(setData23)}
                        disabled={disableInfoDoorsBtns}
                    >
                        Write Info Doors
                    </button>


                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={() => { setIsDropdownOpen(false); readInfoHVAC(true, setData23) }}>Read Info Hvac</button>

                    <div className="dropdown">
                        <button tabIndex={5} className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white relative" onClick={() => setIsDropdownOpenHVAC(!isDropdownOpenHVAC)} disabled={disableInfoHvacBtns}>
                            Write HVAC param
                            <Image
                                src="/dropdownarrow.png"
                                alt="Dropdown arrow icon"
                                className="dark:invert m-1 hover:object-scale-down"
                                width={10}
                                height={10}
                                priority
                            />
                        </button>
                        {isDropdownOpenHVAC && (
                            <div>
                                <ul tabIndex={5} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                                    <li>
                                        <label htmlFor="my_modal_9">
                                            <a onClick={() => { dropdownHVACItemClick("Ambient air temperature", "ambient_air_temperature"); }}>Ambient air temperature</a>
                                        </label>
                                    </li>
                                    <li>
                                        <label htmlFor="my_modal_9">
                                            <a onClick={() => { dropdownHVACItemClick("Cabin temperature driver set", "cabin_temperature_driver_set"); }}>Cabin temperature driver set</a>
                                        </label>
                                        <li>
                                            <label htmlFor="my_modal_9">
                                                <a onClick={() => { dropdownHVACItemClick("Fan speed", "fan_speed"); }}>Fan speed</a>
                                            </label>
                                            <label htmlFor="my_modal_9">
                                                <a onClick={() => { dropdownHVACItemClick("Mass air flow", "mass_air_flow"); }}>Mass air flow</a>
                                            </label>
                                        </li>
                                    </li>
                                </ul>
                                <ModalUDS id="my_modal_8" cardTitle={hvacCardTitle} writeInfo={writeInfoHvac} param={paramHvac} setter={setData23} />


                            </div>
                        )}
                        {data23 && (
                            <div>
                                <h1 className="text-2xl mt-2">Response</h1>
                                <ul className="m-2 p-2 list-disc">
                                    {Object.entries(data23).map(([key, value]) => (
                                        <li key={key}>
                                            <strong>{key}:</strong> {JSON.stringify(value)}
                                        </li>
                                    ))}
                                </ul>
                            </div>
                        )}

                    </div>

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
                </div >
            </div>
        </div >

    )
}
export default SendRequests
import Link from 'next/link'
import React, { useState } from 'react'

const SendRequests = () => {
    const [logs, setLogs] = useState<string[]>([]);
    const [data, setData] = useState();
    const [canId, setCanId] = useState("");
    const [canData, setCanData] = useState("");

    const fetchLogs = () => {
        fetch('http://127.0.0.1:5000/api/logs', {
            method: 'GET',
        }).then(response => response.json())
            .then(data => {
                setLogs(data.logs);
            })
            .catch(error => {
                console.error('Error fetching logs:', error);
            });
    }

    const sendFrame = () => {
        if (!canId || !canData) {
            alert('CAN ID and CAN Data cannot be empty.');
            return;
        }
        fetch('http://127.0.0.1:5000/api/send_frame', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ can_id: canId, can_data: canData }),
        }).then(response => response.json())
            .then(data => {
                setData(data);
                fetchLogs();
            });
    }

    const requestIds = () => {
        fetch('http://127.0.0.1:5000/api/request_ids', {
            method: 'GET',
        }).then(response => response.json())
            .then(data => {
                setData(data);
                fetchLogs();
            });
    }

    const updateToVersion = () => {
        const ecuId = prompt('Enter ECU ID:');
        const version = prompt('Enter Version:');
        fetch('http://127.0.0.1:5000/api/update_to_version', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({ ecu_id: ecuId, version: version }),
        }).then(response => response.json())
            .then(data => {
                setData(data);
                fetchLogs();
            });
    }

    const readInfoBattery = () => {
        fetch('http://127.0.0.1:5000/api/read_info_battery', {
            method: 'GET',
        }).then(response => response.json())
            .then(data => {
                setData(data);
                fetchLogs();
            });
    }

    const readInfoEngine = () => {

    }

    const readInfoDoors = () => {
        fetch('http://127.0.0.1:5000/api/read_info_doors', {
            method: 'GET',
        }).then(response => response.json())
            .then(data => {
                setData(data);
                fetchLogs();
            });
    }

    const writeInfoDoors = () => {
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

        fetch('http://127.0.0.1:5000/api/write_info_doors', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
        }).then(response => response.json())
            .then(data => {
                setData(data);
                fetchLogs();
            });
    }

    const writeInfoBattery = () => {
        const battery_level = prompt('Enter Battery Energy Level:');
        const voltage = prompt('Enter Battery Voltage:');
        const stateOfCharge = prompt('Enter Battery State of Charge:');
        const percentage = prompt('Enter Battery Percentage:');
        // const temperature = prompt('Enter Battery Temperature:');
        // const lifeCycle = prompt('Enter Battery Life Cycle:');
        // const fullyCharged = prompt('Enter Battery Fully Charged Status:');
        // const range = prompt('Enter Battery Range:');
        // const chargingTime = prompt('Enter Battery Charging Time:');
        // const deviceConsumption = prompt('Enter Device Consumption:');

        const data = {
            battery_level: battery_level || null,
            voltage: voltage || null,
            battery_state_of_charge: stateOfCharge || null,
            percentage: percentage || null,
            // temperature: temperature || null,
            // life_cycle: lifeCycle || null,
            // fully_charged: fullyCharged || null,
            // range_battery: range || null,
            // charging_time: chargingTime || null,
            // device_consumption: deviceConsumption || null
        };


        fetch('http://127.0.0.1:5000/api/write_info_battery', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
        })
            .then(response => response.json())
            .then(data => {
                setData(data);
                fetchLogs();
            })
            .catch(error => {
                console.error('Error:', error);
            });
    }

    return (
        <div className="w-[90%] h-screen flex flex-col items-center">
            <div className="w-[50%] h-screen flex flex-col">
                <h1 className="text-3xl mt-4">CAN Interface Control</h1>

                <button className="btn btn-info w-fit mt-5 text-white">
                    <Link href="http://127.0.0.1:5000/apidocs/" target="_blank" rel="noopener noreferrer">Go to Docs</Link>
                </button>

                <p className="text-xl mt-3">CAN ID:</p>
                <input type="text" placeholder="e.g., 0xFa, 0x1234" className="input input-bordered w-full" onChange={e => setCanId(e.target.value)} />
                <p className="text-xl mt-3">CAN Data:</p>
                <input type="text" placeholder="e.g., 0x12, 0x34" className="input input-bordered w-full" onChange={e => setCanData(e.target.value)} />
                <button className="btn btn-success w-fit mt-5 text-white" onClick={sendFrame}>Send Frame</button>
                <div className="mt-4">
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={requestIds}>Request IDs</button>
                    <button className="btn btn-success w-fit m-1 text-white" onClick={updateToVersion}>Update to version</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoBattery}>Read Info Battery</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoEngine}>Read Info Engine</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={readInfoDoors}>Read Info Doors</button>
                </div>
                <div className="mt-4">
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoDoors}>Write Doors Info</button>
                    <button className="btn bg-blue-500 w-fit m-1 hover:bg-blue-600 text-white" onClick={writeInfoBattery}>Write Battery Info</button>
                </div>
                <h1 className="text-3xl mt-4">Response</h1>
                <textarea id="response-output" className="m-2 h-fit textarea textarea-bordered" placeholder="" value={JSON.stringify(data, null, 0)}></textarea>

                <div className="m-2 border-2 border-black overflow-x-auto">
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

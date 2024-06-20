import React from 'react'
import EngineState from '../components/animations/engineState.js'
import FuelConsumption from '../components/animations/fuelConsumption.js'
import CurrentSpeed from '../components/animations/currentSpeed.js'
import FuelUsed from '../components/animations/fuelUsed.js'
import PowerOutput from '../components/animations/powerOutput.js'
import SerialNumber from '../components/animations/serialNumber.js'
import StateOfRunning from '../components/animations/stateOfRunning.js'
import Torque from '../components/animations/torque.js'

const EnginesModule = async () => {
    const res = await fetch(
        'https://pocapi.pythonanywhere.com/api?service=engine_module',
        { cache: 'no-store'}
    );
    const jsonData = await res.json();

    return (
        <div className="h-screen flex flex-col">
            <nav className="bg-blue-900 p-8 flex justify-between items-center">
                <div className="text-white text-2xl font-bold">Battery Module</div>
            </nav>
            <div>
                <div className='m-8 flex items-center justify-center'>
                    <div className="card w-96 bg-base-100 shadow-xl">
                        <div className="card-body">
                            <div className='flex justify-center'>
                                <div className="dropdown dropdown-hover">
                                    <div tabIndex={0} role="button" className="btn bg-green-300 margin">Choose method</div>
                                    <ul tabIndex={0} className="dropdown-content z-[1] menu p-2 shadow bg-base-100 rounded-box w-52 bg-center">
                                        <li><a>Get</a></li>
                                        <li><a>Post</a></li>
                                        <li><a>Put</a></li>
                                    </ul>
                                </div>
                                <div className="dropdown dropdown-hover">
                                    <div tabIndex={0} role="button" className="btn bg-green-300">Choose parameter</div>
                                    <ul tabIndex={0} className="dropdown-content z-[1] menu p-2 shadow bg-base-100 rounded-box w-52">
                                        <li><a>Current speed</a></li>
                                        <li><a>Engine state</a></li>
                                        <li><a>Fuel consumption</a></li>
                                        <li><a>Fuel used</a></li>
                                        <li><a>Power output</a></li>
                                        <li><a>Serial number</a></li>
                                        <li><a>State of number</a></li>
                                        <li><a>Torque</a></li>
                                        
                                    </ul>
                                </div>
                                <div className="dropdown dropdown-hover">
                                    <button className="btn bg-gray-500"> Send
                                    </button>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <div className=" flex items-center justify-center">
                    <table>
                        <tr>
                            <td>
                                <div className="card w-96 bg-green-300 text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Current speed</h2>
                                        <h1>Current speed: {jsonData.query_params.current_speed}</h1>
                                        <div className="card-actions justify-end">
                                            <CurrentSpeed></CurrentSpeed>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Engine state</h2>
                                        <h1>Engine state: {jsonData.query_params.engine_state}</h1>
                                        <div className="card-actions justify-end">
                                            <EngineState></EngineState>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Fuel consumption</h2>
                                        <h1>Fuel consumption: {jsonData.query_params.fuel_conusmption}</h1>
                                        <div className="card-actions justify-end">
                                            <FuelConsumption></FuelConsumption>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Fuel used</h2>
                                        <h1>Fuel used: {jsonData.query_params.fuel_used}</h1>
                                        <div className="card-actions justify-end">
                                            <FuelUsed></FuelUsed>
                                        </div>
                                    </div>
                                </div>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Power output</h2>
                                        <h1>Power output: {jsonData.query_params.power_output}</h1>
                                        <div className="card-actions justify-end">
                                            <PowerOutput></PowerOutput>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Serial number</h2>
                                        <h1>Serial number: {jsonData.query_params.serial_number}</h1>
                                        <div className="card-actions justify-end">
                                            <SerialNumber></SerialNumber>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">State of running</h2>
                                        <h1>State of running: {jsonData.query_params.state_of_running}</h1>

                                        <div className="card-actions justify-end">
                                            <StateOfRunning></StateOfRunning>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Torque</h2>
                                        <h1>Torque: {jsonData.query_params.torque}</h1>
                                        <div className="card-actions justify-end">
                                            <Torque></Torque>
                                        </div>
                                    </div>
                                </div>
                            </td>
                        </tr>
                        
                    </table>
                </div>
            </div >
        </div >
    )
        
}

export default EnginesModule
import React from 'react'
import BatteryPercentage from '../components/animations/batteryPercentage.js'
import BatteryVoltage from '../components/animations/batteryVoltage.js'
import BatteryStateOfCharge from '../components/animations/batteryState.js'
import LifeCycle from '../components/animations/lifeCycle.js'
import BatteryFully from '../components/animations/batteryFully.js'
import SerialNumber from '../components/animations/serialNumber.js'
import Other from '../components/animations/other.js'
import Temperature from '../components/animations/temperature.js'

const BatteryModule = async () => {
    const res = await fetch(
        'https://pocapi.pythonanywhere.com/api?service=battery_module',
        { cache: 'no-store' }
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
                                        <li><a>Battery level</a></li>
                                        <li><a>Voltage</a></li>
                                        <li><a>Battery state of charge</a></li>
                                        <li><a>Temperature</a></li>
                                        <li><a>Life cycle</a></li>
                                        <li><a>Fully charged</a></li>
                                        <li><a>Serial number</a></li>
                                        <li><a>Other</a></li>
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
                                        <h2 className="card-title">Battery</h2>
                                        <h1>Battery level: {jsonData.query_params.battery_level}</h1>
                                        {/* <p>If a dog chews shoes whose shoes does he choose?</p> */}
                                        <div className="card-actions justify-end">
                                            <BatteryPercentage></BatteryPercentage>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Voltage</h2>
                                        <h1>Voltage: {jsonData.query_params.voltage}</h1>
                                        {/* <p>If a dog chews shoes whose shoes does he choose?</p> */}
                                        <div className="card-actions justify-end">
                                            <BatteryVoltage></BatteryVoltage>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Battery state of charge</h2>
                                        <h1>Battery state of charge: {jsonData.query_params.battery_state_of_charge}</h1>
                                        {/* <p>If a dog chews shoes whose shoes does he choose?</p> */}
                                        <div className="card-actions justify-end">
                                            <BatteryStateOfCharge></BatteryStateOfCharge>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Temperature</h2>
                                        <h1>Temperature: {jsonData.query_params.temperature}</h1>
                                        {/* <p>If a dog chews shoes whose shoes does he choose?</p> */}
                                        <div className="card-actions justify-end">
                                            <Temperature></Temperature>
                                            {/* <img src="/temperature.jpg" alt="" className="h-10 w-10 shadow-2xl" /> */}
                                        </div>
                                    </div>
                                </div>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Life cycle</h2>
                                        <h1>Life cycle: {jsonData.query_params.life_cycle}</h1>
                                        {/* <p>If a dog chews shoes whose shoes does he choose?</p> */}
                                        <div className="card-actions justify-end">
                                            <LifeCycle></LifeCycle>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Fully charged</h2>
                                        <h1>Fully charged: {jsonData.query_params.fully_charged}</h1>
                                        {/* <p>If a dog chews shoes whose shoes does he choose?</p> */}
                                        <div className="card-actions justify-end">
                                            <BatteryFully></BatteryFully>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Serial number</h2>
                                        <h1>Serial number: {jsonData.query_params.serial_number}</h1>
                                        {/* <p>If a dog chews shoes whose shoes does he choose?</p> */}
                                        <div className="card-actions justify-end">
                                            <SerialNumber></SerialNumber>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Other</h2>
                                        <h1>Other: {jsonData.query_params.other}</h1>
                                        {/* <p>If a dog chews shoes whose shoes does he choose?</p> */}
                                        <div className="card-actions justify-end">
                                            <Other></Other>
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

export default BatteryModule
import Link from "next/link";
import React from 'react'
import SerialNumber from '../components/animations/serialNumber.js'
import Car from '../components/animations/car.js'
import LockUnlock from '../components/animations/lock.js'
import Cigarette_light from '../components/animations/cigarette_light.js'
import Indoor_light from '../components/animations/indoor_light.js'

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
                                        <li><a>door position</a></li>
                                        <li><a>door status</a></li>
                                        <li><a>serial_number</a></li>
                                        <li><a>cigarette_lighter_voltage</a></li>
                                    </ul>
                                </div>
                                <div className="dropdown dropdown-hover">
                                    <button className="btn bg-green-300"> Send
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
                                        <h2 className="card-title">Door</h2>
                                        <h1>Door position: {jsonData.query_params.door}</h1>
                                        <div className="card-actions justify-end">
                                        <Car></Car>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300 text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Door status</h2>
                                        <h1>Status: {jsonData.query_params.status}</h1>
                                        <div className="card-actions justify-end">
                                        <Car></Car>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300 text-black">
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
                                <div className="card w-96 bg-green-300 text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Cigarette lighter voltage</h2>
                                        <h1>Cigarette voltage: {jsonData.query_params.cigarette_lighter_voltage}</h1>
                                        <div className="card-actions justify-end">
                                            <Cigarette_light></Cigarette_light>
                                        </div>
                                    </div>
                                </div>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Lock/Unlock state</h2>
                                        <h1>Door state: {jsonData.query_params.door_state}</h1>
                                        <div className="card-actions justify-end">
                                        <LockUnlock></LockUnlock>
                                        </div>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div className="card w-96 bg-green-300  text-black">
                                    <div className="card-body">
                                        <h2 className="card-title">Lights</h2>
                                        <h1>Lights state: {jsonData.query_params.light_state}</h1>
                                        <div className="card-actions justify-end">
                                        <Indoor_light></Indoor_light>
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

export default DoorsModule
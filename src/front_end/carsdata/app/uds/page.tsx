import Link from 'next/link';
import React from 'react';

const UDSpage = () => {
    return (
        <>
            <nav className="bg-blue-900 p-8 flex justify-between items-center">
                <div className="text-white text-2xl font-bold">Unified Diagnostic Services (UDS)</div>
            </nav>
            <div className="m-9 flex items-center justify-center">
                <table>
                    <tr>
                        <td>
                            <div className="card w-96 bg-green-300 text-black">
                                <div className="card-body">
                                    <h2 className="card-title">Battery</h2>
                                    <Link href="/batteryModule"><img src="/battery.png" alt="" className="object-contain hover:object-scale-down h-28 w-56" /></Link>
                                    <Link href="/engineModule"><button className="btn btn-primary">Check Battery</button></Link>
                                </div>
                            </div>
                        </td>
                        <td>
                            <div className="card w-96 bg-green-300 text-black">
                                <div className="card-body">
                                    <h2 className="card-title">Engine</h2>
                                    <Link href="/engineModule"><img src="/engine.png" alt="" className="object-contain hover:object-scale-down h-28 w-56" /></Link>
                                    <Link href="/engineModule"><button className="btn btn-primary">Check Engine</button></Link>
                                </div>
                            </div>
                        </td>
                        <td>
                            <div className="card w-96 bg-green-300 text-black">
                                <div className="card-body">
                                    <h2 className="card-title">Doors</h2>
                                    <Link href="/doorsModule"><img src="/doors.png" alt="" className="object-contain hover:object-scale-down h-28 w-56" /></Link>
                                    <Link href="/engineModule"><button className="btn btn-primary">Check Doors</button></Link>
                                </div>
                            </div>
                        </td>
                    </tr>
                </table>
            </div>
        </>
    )
}

export default UDSpage
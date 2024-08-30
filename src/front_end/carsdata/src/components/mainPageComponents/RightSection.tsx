import Link from 'next/link';
import React from 'react';

const RightSection = () => {
    return (
        <>
            <div className="w-full max-w-sm mx-auto space-y-4">
                <div className="m-4 h-[35%] bg-white shadow-md rounded-lg p-6">
                    <h2 className="text-xl font-bold mb-2">Over the air update</h2>
                    <Link href="/ota"><img src="/ota.png" alt="" className="object-contain hover:object-scale-down h-[65%] w-[100%]" /></Link>
                    <Link href="/ota"><button className="m-1 w-[35%] btn btn-xs btn-primary">Make OTA</button></Link>
                </div>

                <div className="m-4 h-[35%] bg-white shadow-md rounded-lg p-6">
                    <h2 className="text-xl font-bold mb-2">Unified Diagnostic Services</h2>
                    <Link href="/uds"><img src="/uds.png" alt="" className="object-contain hover:object-scale-down h-[65%] w-[100%]" /></Link>
                    <Link href="/uds"><button className="m-1 w-[35%] btn btn-xs btn-primary">Get UDS</button></Link>
                </div>
            </div>
        </>
    )
}

export default RightSection

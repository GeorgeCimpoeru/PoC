import React from 'react';
import Image from "next/image";
import Link from 'next/link';

const LeftSideBar = () => {
    return (
        <div className="w-[6%] h-[96%] bg-blue-700 rounded-lg m-5 p-6 shadow-xl">
            <div className="flex items-center justify-center mt-20">
                <Link href="/ota">
                    <h2>OTA</h2>
                    <Image
                        src="/otaIcon.png"
                        alt="Vercel Logo"
                        className="dark:invert hover:object-scale-down"
                        width={30}
                        height={30}
                        priority
                    />
                </Link>
            </div>
            <div className="flex items-center justify-center mt-20">
                <Link href="/UDS">
                    <h2>UDS</h2>
                    <Image
                        src="/udsIcon.png"
                        alt="Vercel Logo"
                        className="dark:invert hover:object-scale-down"
                        width={30}
                        height={30}
                        priority
                    />
                </Link>
            </div>
        </div>
    )
}

export default LeftSideBar
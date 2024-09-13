import React from 'react';
import Link from "next/link";

interface NavBarMainProps {
    isVinSubmitted: boolean;
}

const NavBarMain = ({ isVinSubmitted }: NavBarMainProps) => {
    return (
        <div className="fixed top-0 left-0 w-full bg-base-100 shadow-md z-50">
            <div className="flex items-center justify-between px-4 py-2">
                <a className="btn btn-ghost text-2xl font-bold pl-20">Drive Insight</a>

                <div className="flex-none">
                    <ul className="menu menu-horizontal px-1">
                        {isVinSubmitted && (
                            <>
                                <li><Link href="/ota">OTA</Link></li>
                                <li><Link href="/UDS">UDS</Link></li>
                            </>
                        )}
                        <li><Link href="/login">Log In</Link></li>
                        <li><Link href="/signup">Sign Up</Link></li>
                    </ul>
                </div>
            </div>
        </div>
    );
}

export default NavBarMain;

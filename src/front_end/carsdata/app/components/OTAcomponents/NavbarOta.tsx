import React from 'react'
import Image from 'next/image'

const NavbarOta = () => {
    return (
        <nav className="bg-gray-300 rounded-lg m-5 p-6 shadow-xl flex justify-between items-center">
            <div className="text-gray-800 text-2xl font-bold">Over-the-Air Update</div>
            <div className="dropdown dropdown-click">
                <div tabIndex={0} role="button" className="btn bg-blue-500 m-1 relative">Select device
                    <Image
                        src="/dropdownarrow.png"
                        alt="Dropdown arrow icon"
                        className="dark:invert m-1 hover:object-scale-down"
                        width={10}
                        height={10}
                        priority
                    />
                </div>
                <ul tabIndex={0} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                    <li><a>MCU 1</a></li>
                    <li><a>MCU 2</a></li>
                    <li><a>ECU 1</a></li>
                    <li><a>ECU 2</a></li>
                    <li><a>ECU 3</a></li>
                </ul>
            </div>

        </nav>
    )
}

export default NavbarOta

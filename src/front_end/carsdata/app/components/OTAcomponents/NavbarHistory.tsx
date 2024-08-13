import React from 'react';
import Image from 'next/image';


const NavbarHistory = (props: any) => {

    return (
        <><div style={{
            position: 'fixed',
            top: 0,
            left: 0,
            width: '100vw',
            height: '100vh',
            backgroundImage: 'url(/car2.jpg)',
            backgroundSize: 'cover',
            backgroundPosition: 'center',
            opacity: 0.4,
            zIndex: -1,
        }}>

        </div><nav className="bg-gray-300 rounded-lg m-5 p-6 shadow-xl flex justify-between items-center">
                <div className="text-gray-800 text-2xl font-bold">Over-the-Air History</div>
                <div className="dropdown dropdown-click">
                    <div tabIndex={0} role="button" className="btn bg-blue-500 m-1 relative">History & Updates
                        <Image
                            src="/dropdownarrow.png"
                            alt="Dropdown arrow icon"
                            className="dark:invert m-1 hover:object-scale-down"
                            width={10}
                            height={10}
                            priority />
                    </div>
                    <ul tabIndex={0} className="dropdown-content menu bg-base-100 rounded-box z-[1] w-52 p-2 shadow">
                        <li><a onClick={() => props.checkedButton(1)}>History Updates</a></li>
                        <li><a onClick={() => props.checkedButton(2)}>Version Control</a></li>
                        <li><a onClick={() => props.checkedButton(3)}>x</a></li>
                        <li><a onClick={() => props.checkedButton(4)}>y</a></li>
                        <li><a onClick={() => props.checkedButton(5)}>z</a></li>
                    </ul>
                </div>
            </nav></>
    )
}

export default NavbarHistory


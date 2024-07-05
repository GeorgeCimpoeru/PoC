'use client';
import React, { useState } from 'react';
import OpenMenu from '../components/mainPageComponents/OpenedMenu';
import CloseMenu from '../components/mainPageComponents/ClosedMenu';
import Map from '../components/mainPageComponents/Map';
import Divider from '../components/mainPageComponents/Divider';
import RightSection from '../components/mainPageComponents/RightSection';

const MainPage = () => {
    const [menuOpen, setMenuOpen] = useState(false);

    const toggleMenu = () => {
        setMenuOpen(!menuOpen);
    };

    const handleOutsideClick = (event: MouseEvent) => {
        const target = event.target as HTMLElement;
        if (!target.closest('#burgerMenu') && !target.closest('#dropdownMenu')) {
            setMenuOpen(false);
        }
    };

    React.useEffect(() => {
        window.addEventListener('click', handleOutsideClick);
        return () => {
            window.removeEventListener('click', handleOutsideClick);
        };
    }, []);

    return (
        <div className="h-screen flex flex-col">
            <nav className="bg-blue-600 p-8 flex justify-between items-center">
                <div className="text-white text-2xl font-bold">carsData</div>
                <div className="md:hidden">
                    <button id="burgerMenu" className="text-white focus:outline-none" onClick={toggleMenu}>☰</button>
                </div>
                <OpenMenu />
            </nav>
            <input type="text" className="m-4 w-[20%]" placeholder="Search" />
            <div className="flex w-full h-3/4 flex-col lg:flex-row">
                <Map></Map>
                <Divider></Divider>
                <RightSection></RightSection>
            </div>

            {menuOpen && (
                <CloseMenu />
            )}
        </div>
      
    )
}

export default MainPage
'use client';
import React, { useState } from 'react'
import MainElements from '../components/mainPageComponents/mainElements';
import OpenMenu from '../components/mainPageComponents/openedMenu';
import CloseMenu from '../components/mainPageComponents/closedMenu';

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
                    <button id="burgerMenu" className="text-white focus:outline-none" onClick={toggleMenu}>
                        ☰
                    </button>
                </div>
                <OpenMenu />
            </nav>

            <MainElements />

            {menuOpen && (
                <CloseMenu />
            )}
        </div>
    )
}

export default MainPage
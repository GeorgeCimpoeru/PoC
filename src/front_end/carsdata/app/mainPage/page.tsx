'use client';
import React from 'react';
import LeftSideBar from '../components/mainPageComponents/LeftSideBar';
import NavBar from '../components/mainPageComponents/NavBar';
import VINsearch from '../components/mainPageComponents/VINsearch';

const MainPage = () => {
    return (
        <div className="flex w-full h-screen">
            <LeftSideBar></LeftSideBar>
            <div className="w-[90%] h-full flex flex-col">
                <NavBar></NavBar>
                <VINsearch></VINsearch>
            </div>
        </div>
    )
}

export default MainPage
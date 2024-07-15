'use client';
import React from 'react';
import LeftSideBar from '../components/mainPageComponents/LeftSideBar';
import NavBar from '../components/mainPageComponents/NavBar';
import Map from '../components/mainPageComponents/Map';

const MainPage = () => {
    let fruits: Array<string> = ['Apple', 'Orange', 'Banana'];
    return (
        <div className="flex w-full h-screen">
            <LeftSideBar></LeftSideBar>
            <div className="w-[90%] h-full flex flex-col">
                <NavBar></NavBar>
                <Map></Map>
            </div>
        </div>
    )
}

export default MainPage
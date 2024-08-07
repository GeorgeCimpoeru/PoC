'use client';
import React from 'react';
import Carousel from '../components/mainPageComponents/Carousel';
import NavBarMain from '../components/mainPageComponents/NavBarMain';

const MainPage = () => {
    return (
        <div className="flex flex-col h-auto w-full">
            <NavBarMain></NavBarMain>
            <div className="flex flex-1">
                <Carousel></Carousel>
            </div>

            <div className="flex flex-row my-5">
                <div className="w-1/2 flex justify-center items-center">
                    <img src="./history.png" alt="Example" className="max-w-full h-auto" />
                </div>
                <div className="w-1/2 flex flex-col justify-center items-start px-4">
                    <h1 className="text-2xl font-bold mb-1">
                        Automotive Over-The-Air
                    </h1>
                    <p className="text-left text-base">
                        Automotive Over-The-Air (Automotive OTA) is changing the industry. In particular, applications such as software updates, live diagnostics and data collection already promise not only 
                        enormous savings potential but also enable many new opportunities to increase customer loyalty. Further applications will follow 
                        for sure. However, the challenges and efforts involved in introducing such Automotive OTA solutions are considerable. 
                        A good integration into existing processes is crucial for successful Automotive OTA projects. (example text)
                    </p>
                </div>
            </div>
        </div>


    )
}

export default MainPage;

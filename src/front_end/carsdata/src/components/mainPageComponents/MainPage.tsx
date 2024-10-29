'use client';
import React, { useState, useEffect } from 'react';
import Carousel from '@/src/components/mainPageComponents/Carousel';
import NavBarMain from '@/src/components/mainPageComponents/NavBarMain';
import Map from '@/src/components/mainPageComponents/Map';

const MainPage = () => {
    
    const [isVinSubmitted, setIsVinSubmitted] = useState(false);
    const [isModalOpen, setIsModalOpen] = useState(false);
    const [vin, setVin] = useState('');

    const handleOpenModal = () => {
        setIsModalOpen(true);
    };

    const handleCloseModal = () => {
        setIsModalOpen(false);
    };

    useEffect(() => {
        const storedVinSubmitted = sessionStorage.getItem('isVinSubmitted') === 'true';
        setIsVinSubmitted(storedVinSubmitted);
    }, []);

    const handleVinSubmit = async () => {
        try {
            const response = await fetch(`https://jsonplaceholder.typicode.com/posts`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    userId: 1,
                    id: 1,
                    title: vin,
                    body: 'Some body text',
                }),
            });

            if (!response.ok) {
                throw new Error('Network response was not ok');
            }

            const data = await response.json();
            console.log('VIN Submitted successfully:', data);

            sessionStorage.setItem('isVinSubmitted', 'true');
            setIsVinSubmitted(true);
            setIsModalOpen(false);
        } catch (error) {
            console.error('There was a problem with the fetch operation:', error);
        }
    };


    return (
        <div className="flex flex-col h-full w-full">
            <NavBarMain isVinSubmitted={isVinSubmitted} />
            <div className="relative flex flex-1">
                <Carousel />
                <div className="absolute inset-0 flex items-center justify-center">
                    <button
                        className="btn text-xl text-white font-semibold bg-red-500"
                        onClick={handleOpenModal}
                    >
                        Connect to vehicle
                    </button>
                </div>
            </div>

            {isModalOpen && (
                <div className="fixed inset-0 z-50 flex items-center justify-center bg-black bg-opacity-50">
                    <div className="bg-white p-6 rounded-lg shadow-lg">
                        <h2 className="text-xl font-bold mb-4">Enter VIN</h2>
                        <input
                            type="text"
                            value={vin}
                            onChange={(e) => setVin(e.target.value)}
                            placeholder="Enter VIN"
                            className="input input-bordered w-full mb-4"
                        />
                        <div className="flex justify-end space-x-4">
                            <button
                                className="btn bg-gray-500 text-white"
                                onClick={handleCloseModal}
                            >
                                Cancel
                            </button>
                            <button
                                className="btn bg-blue-500 text-white"
                                onClick={handleVinSubmit}
                            >
                                Send
                            </button>
                        </div>
                    </div>
                </div>
            )}

            <div className="pt-10 pb-10 flex flex-row my-8">
                <div className="w-1/2 flex justify-center items-center">
                    <img src="./images.png" alt="Example" className="w-36 h-36" />
                </div>
                <div className="w-1/2 flex flex-col justify-center items-start pr-20">
                    <h1 className="text-2xl font-bold mb-1">
                        Automotive Over-The-Air
                    </h1>
                    <p className="text-base">
                        Automotive Over-The-Air (Automotive OTA) is changing the industry. In particular, applications such as software updates, live diagnostics and data collection already promise not only
                        enormous savings potential but also enable many new opportunities to increase customer loyalty. Further applications will follow
                        for sure. However, the challenges and efforts involved in introducing such Automotive OTA solutions are considerable.
                        A good integration into existing processes is crucial for successful Automotive OTA projects. (example text)
                    </p>
                </div>
            </div>

            <div className="pt-10 pb-10 flex flex-row bg-gray-100 shadow-md">
                <div className="w-1/2 flex flex-col justify-center items-start pl-20">
                    <h1 className="text-2xl font-bold mb-1">
                        Key Use Cases
                    </h1>

                    <p className="text-base">
                        Today, most of the recall campaigns are software related. And the amount of software used in vehicles is still
                        rising rapidly. Software updates over the air offer a tremendous potential for cost savings by avoiding these
                        recalls. Functional and security issues can be fixed remotely. OTA software updates also provide the opportunity
                        to implement new business models with for instance new features installed on demand, after the vehicle has been
                        delivered.
                    </p>
                </div>
                <div className="w-1/2 flex justify-center items-center">
                    <img src="./use_case.png" alt="Example" className="w-64 h-36" />
                </div>
            </div>

            <div className="pt-10 pb-10 flex justify-center items-center">
                <Map />
            </div>
        </div>
    );
};

export default MainPage;

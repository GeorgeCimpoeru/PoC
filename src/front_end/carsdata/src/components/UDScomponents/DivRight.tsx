import React from 'react'
import Image from "next/image";

const DivRight = () => {
    return (
        <div className="w-[24%] h-screen flex flex-col bg-white">

            {/* <div className="avatar placeholder grid justify-items-end">
                <div className="w-fit h-fit">
                    <span className="mt-1">Nume utilizator</span>
                    <div className="bg-neutral text-neutral-content w-8 rounded-full">
                        <span className="text-xl">UI</span>
                    </div>
                </div>
            </div> */}

            <h3 className="text-3xl m-4">Specs</h3>
            <h3 className="text-2xl ml-3 mt-3">Total distance</h3>
            <h3 className="text-3xl ml-3">17 652km</h3>
            <h3 className="text-2xl ml-3 mt-3">Total</h3>
            <div>
                <div className="text-xl ml-3 mt-3 flex justify-between w-full max-w-sm">
                    <span>Battery Level</span>
                    <span>64%</span>
                </div>
                <progress className="progress progress-primary w-[90%] ml-3" value="64" max="100"></progress>
            </div>
            <div>
                <div className="text-xl ml-3 mt-3 flex justify-between w-full max-w-sm">
                    <span>Estimated range</span>
                    <span>261km</span>
                </div>
                <progress className="progress progress-primary w-[90%] ml-3" value="70" max="100"></progress>
            </div>
            <div className="divider w-[90%] ml-3"></div>
            <h3 className="text-2xl ml-3 mt-3">Total</h3>

            <div className="text-sm ml-3 mt-3 flex justify-evenly w-full">
                <div>
                    <span>Engine</span>
                    <Image
                        src="/icons8-propeller-24.png"
                        alt="Engine"
                        className="dark:invert hover:object-scale-down"
                        width={30}
                        height={30}
                        priority
                    />
                </div>
                <div>
                    <span>Headlights</span>
                    <Image
                        src="/icons8-car-light-24.png"
                        alt="Engine"
                        className="dark:invert hover:object-scale-down"
                        width={30}
                        height={30}
                        priority
                    />
                </div>
                <div>
                    <span>Tire</span>
                    <Image
                        src="/icons8-tire-50.png"
                        alt="Engine"
                        className="dark:invert hover:object-scale-down"
                        width={30}
                        height={30}
                        priority
                    />
                    
                </div>
            </div>
            <div className="divider w-[90%] ml-3"></div>
            <h3 className="text-2xl ml-3 mt-3">Temperature</h3>
            <div className="text-sm ml-3 mt-3 flex justify-evenly w-full">
                <span>Inside</span>
                <span>Outside</span>
            </div>
            <div className="text-2xl ml-3 mt-3 flex justify-evenly w-full">
                <span>25°C</span>
                <span>15°C</span>
            </div>
        </div>
    )
}

export default DivRight
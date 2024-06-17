import Link from "next/link";

const MainElements = () => {
    return (
        <div className="flex-grow flex flex-col justify-center items-center">
            <div className="p-15 flex flex-col md:flex-row md:space-x-16 space-y-16 md:space-y-0 items-center">


                <div className="hero min-h-screen bg-base-200">
                    <div className="hero-content flex-col lg:flex-row">
                        <Link href="/batteryModule"><img src="/battery.png" alt="" className="max-w-sm rounded-lg shadow-2xl" /></Link>
                        <div>
                            <h1 className="text-5xl font-bold">Battery module</h1>
                            <p className="py-6">Get informations about the battery status</p>
                            <button className="btn btn-primary"  > <Link href="/batteryModule">Check here </Link> </button>
                           
                        </div>
                    </div>
                </div>


                <div className="hero min-h-screen bg-base-200">
                    <div className="hero-content flex-col lg:flex-row">

                        <img src="/engine.png" className="max-w-sm rounded-lg shadow-2xl" />
                        <div>
                            <h1 className="text-5xl font-bold">Engine</h1>
                            <p className="py-6"> Get informations about the engine </p>
                            <button className="btn btn-primary">Check here</button>
                        </div>
                    </div>
                </div>

                <div className="hero min-h-screen bg-base-200">
                    <div className="hero-content flex-col lg:flex-row">
                        <img src="/doors.png" className="max-w-sm rounded-lg shadow-2xl" />
                        <div>
                            <h1 className="text-5xl font-bold">Doors</h1>
                            <p className="py-6">Get informations about the doors</p>
                            <button className="btn btn-primary">Check here</button>
                        </div>
                    </div>
                </div>
            </div>


        </div >


    )
}

export default MainElements

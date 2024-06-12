import Link from "next/link";

const MainElements = () => {
    return (
        <div className="flex-grow flex flex-col justify-center items-center">
            <div className="flex flex-col md:flex-row md:space-x-16 space-y-16 md:space-y-0 items-center">
                <div className="w-72 h-72 bg-gray-300">
                    <Link href="/batteryModule"><img src="/battery.png" alt="" className="w-full h-full object-cover" /></Link>
                </div>
                <div className="w-72 h-72 bg-gray-300">
                    <Link href="/engineModule"><img src="/engine.png" alt="" className="w-full h-full object-cover" /></Link>
                </div>
                <div className="w-72 h-72 bg-gray-300">
                    <Link href="/doorsModule"><img src="/doors.png" alt="" className="w-full h-full object-cover" /></Link>
                </div>
            </div>
        </div>
    )
}

export default MainElements

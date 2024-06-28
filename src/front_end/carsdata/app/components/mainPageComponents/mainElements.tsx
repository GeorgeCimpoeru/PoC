import Link from "next/link";

const MainElements = () => {
    return (
        <div className="flex w-full h-3/4 flex-col lg:flex-row">
            <div className="m-4 card bg-base-300 rounded-box grid w-3/4 h-3/4 flex-grow place-items-center">
                <div className="relative w-full h-full">
                    <iframe className="absolute top-0 left-0 w-full h-full"
                        src="https://www.google.com/maps/embed?pb=!1m14!1m8!1m3!1d12080.73732861526!2d-74.0059418!3d40.7127847!3m2!1i1024!2i768!4f13.1!3m3!1m2!1s0x0%3A0x0!2zM40zMDA2JzEwLjAiTiA3NMKwMjUnMzcuNyJX!5e0!3m2!1sen!2sus!4v1648482801994!5m2!1sen!2sus"
                    >
                    </iframe>
                </div>
            </div>

            <div className="divider lg:divider-horizontal h-screen lg:h-3/4"></div>

            <div className="m-4 card bg-base-300 rounded-box grid w-1/4 h-3/4 flex-grow place-items-center">
                <div className="m-5 bg-base-100 text-accent-content grid w-1/4 place-content-center rounded">
                    <span className="flex items-center space-x-2">
                        <Link href="/batteryModule"><img src="/battery.png" alt="" className="object-contain hover:object-scale-down h-28 w-56" /></Link>
                        <button className="btn btn-primary"><Link href="/batteryModule">Check battery module</Link></button>
                    </span>
                </div>
                <div className="m-5 bg-base-100 text-accent-content grid w-1/4 place-content-center rounded">
                    <span className="flex items-center space-x-2">
                        <Link href="/engineModule"><img src="/engine.png" alt="" className="object-contain hover:object-scale-down h-28 w-56" /></Link>
                        <button className="btn btn-primary"><Link href="/engineModule">Check engine module</Link></button>
                    </span>
                </div>
                <div className="m-5 bg-base-100 text-accent-content grid w-1/4 place-content-center rounded">
                    <span className="flex items-center space-x-2">
                        <Link href="/doorsModule"><img src="/doors.png" alt="" className="object-contain hover:object-scale-down h-28 w-56" /></Link>
                        <button className="btn btn-primary"><Link href="/doorsModule">Check doors module</Link></button>
                    </span>
                </div>
            </div>
        </div>
    )
}

export default MainElements

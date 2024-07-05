const OpenMenu = () => {
    return (
        <div className="hidden md:flex space-x-4">
            <a href="#" className="text-white hover:text-gray-200">Profile</a>
            <a href="#" className="text-white hover:text-gray-200">Device</a>
            <button className="text-white hover:text-gray-200">Logout</button>
        </div>
    )
}

export default OpenMenu

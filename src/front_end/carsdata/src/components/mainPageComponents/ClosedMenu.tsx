const CloseMenu = () => {
    return (
        <div id="dropdownMenu" className="absolute top-16 right-4 bg-white shadow-lg rounded-lg py-2 w-40">
            <a href="#" className="block px-4 py-2 text-gray-800 hover:bg-gray-200">Profile</a>
            <a href="#" className="block px-4 py-2 text-gray-800 hover:bg-gray-200">Device</a>
            <button className="w-full text-left px-4 py-2 text-gray-800 hover:bg-gray-200">Logout</button>
        </div>
    )
}

export default CloseMenu

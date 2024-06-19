import React from 'react'

const EnginesModule = async () => {
    const res = await fetch(
        'https://pocapi.pythonanywhere.com/api?service=engine_module',
        { cache: 'no-store'}
    );
    const jsonData = await res.json();

    return (
        <div className="h-screen flex flex-col">
            <nav className="bg-blue-600 p-8 flex justify-between items-center">
                <div className="text-white text-2xl font-bold">Engine Module</div>
            </nav>
            <div>
                {/* <pre>{JSON.stringify(jsonData, null, 2)}</pre> */}

                <h1>{jsonData.method}</h1>
                <h1>{jsonData.query_params.current_speed}</h1>
                <h1>{jsonData.query_params.engine_state}</h1>
                <h1>{jsonData.query_params.fuel_consumption}</h1>
                <h1>{jsonData.query_params.fuel_used}</h1>
                <h1>{jsonData.query_params.power_output}</h1>
                <h1>{jsonData.query_params.serial_number}</h1>
                <h1>{jsonData.query_params.state_of_running}</h1>
                <h1>{jsonData.query_params.torque}</h1>
                <h1>{jsonData.query_params.weight}</h1>
                <h1>{jsonData.service}</h1>
            </div>
        </div>
    )
}

export default EnginesModule
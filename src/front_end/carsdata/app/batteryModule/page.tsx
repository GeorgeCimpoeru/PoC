import React from 'react'

const BatteryModule = async () => {
    const res = await fetch(
        'https://pocapi.pythonanywhere.com/api?service=read_data_by_identifier',
        { cache: 'no-store' }
    );
    const jsonData = await res.json();

    return (
        <div className="h-screen flex flex-col">
            <nav className="bg-blue-600 p-8 flex justify-between items-center">
                <div className="text-white text-2xl font-bold">Battery Module</div>
            </nav>
            <div>
                <pre>{JSON.stringify(jsonData, null, 2)}</pre>

                <h1>{jsonData.method}</h1>
                <h1>{jsonData.query_params.data_Format}</h1>
                <h1>{JSON.stringify(jsonData.query_params.encryption)}</h1>
                <h1>{JSON.stringify(jsonData.query_params.identifier)}</h1>
                <h1>{jsonData.service}</h1>
            </div>
        </div>
    )
}

export default BatteryModule
import React, { useState, useEffect } from 'react';

const EngineInfo = () => {
    const [data, setData] = useState(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState(null);

    useEffect(() => {
        const fetchData = async () => {
            try {
                const response = await fetch('http://127.0.0.1:5000/api/read_info_engine', {
                    cache: 'no-store'
                });
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                const result = await response.json();
                console.log(result)
                setData(result);
            } catch (error) {
                setError(error);
            } finally {
                setLoading(false);
            }
        };

        fetchData();
    }, []); 

    if (error) {
        return <div>Error: {error.message}</div>;
    }

    if (data) {
        console.log(data);
    }

    if (!data) {
        return <div>No data available</div>;
    }

    return (
        <div className="overflow-x-auto">
            <h2>Engine Info</h2>
            <table className="table">
                <thead>
                    <tr>
                        <th>Attribute</th>
                        <th>Value</th>
                    </tr>
                </thead>
                <tbody>
                    {Object.entries(data).map(([key, value]) => (
                        <tr key={key}>
                            <td>{key}</td>
                            <td>{value}</td>
                        </tr>
                    ))}
                </tbody>
            </table>
        </div>
    );
};

export default EngineInfo;

import React from 'react';

const BatteryInfo = async () => {

    // const [data, setData] = useState(null);
    // const [loading, setLoading] = useState(true);
    // const [error, setError] = useState(null);
    // useEffect(() => {
    //     const fetchData = async () => {
    //         try {
    //             const response = await fetch('http://127.0.0.1:5000/api/read_info_battery', {
    //                 cache: 'no-store'
    //             });
    //             if (!response.ok) {
    //                 throw new Error('Network response was not ok');
    //             }
    //             const result = await response.json();
    //             console.log(result)
    //             setData(result);
    //         } catch (error) {
    //             setError(error);
    //         } finally {
    //             setLoading(false);
    //         }
    //     };
    //     fetchData();
    // }, []); // Empty dependency array ensures this runs once after the initial render
    // if (loading) {
    //     return <div>Loading...</div>;
    // }
    // if (error) {
    //     return <div>Error: {error.message}</div>;
    // }
    // if (data) {
    //     console.log(data);
    // }
    // if (!data) {
    //     return <div>No data available</div>;
    // }


    const res = await fetch(
        'https://api.agify.io/?name=meelad',
        { cache: 'no-store' }
    );

    const jsonData = await res.json();

    // const response = await fetch('https://api.agify.io/?name=meelad', {
    //     cache: 'no-store'
    // });
    // const result = await response.json();

    return (
        <div className="overflow-x-auto">
            <h2>Battery Info</h2>
            <table className="table">
                <thead>
                    <tr>
                        <th>Attribute</th>
                        <th>Value</th>
                    </tr>
                </thead>
                <tbody>
                    {jsonData.stringify}
                    {/* {result.item.map((key: any, value: any) => (
                        <tr key={key}>
                            <td>{key}</td>
                            <td>{value}</td>
                        </tr>
                    ))} */}
                    {/* {Object.entries(jsonData).map(([key, value]) => (
                        <tr key={key}>
                            <td>{key}</td>
                            <td>{value}</td>
                        </tr>
                    ))} */}
                </tbody>
            </table>
        </div>
    );
};

export default BatteryInfo;

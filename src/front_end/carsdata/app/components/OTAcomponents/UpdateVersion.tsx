'use client'
import React, { useState } from 'react';

const UpdateVersion = () => {
    const [response, setResponse] = useState<any>(null);
    const [error, setError] = useState<string | null>(null);
    const [isPopupVisible, setIsPopupVisible] = useState(false);
    const [data, setData] = useState<any>(null);

    const handleUpdate = async () => {
        try {
            const response = await fetch('http://127.0.0.1:5000/api/update_to_version', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    ecu_id: "123",
                    version: "2"
                })
            });

            console.log("Response received:", response);
            if (!response.ok) {
                throw new Error(`Network response was not ok: ${response.statusText}`);
            }

            const result = await response.json();
            console.log("Data received:", result);
            setData(result);
            setError(null);
        } catch (error: unknown) {
            console.error("Fetch error:", error);
            if (error instanceof Error) {
                setError(error.message);
            } else {
                setError('An unknown error occurred');
            }
        } finally {
            setIsPopupVisible(true);
        }
    };

    const closePopup = () => {
        setIsPopupVisible(false);
    };

    return (
        <div>
            <button onClick={handleUpdate} className="bg-blue-500 text-white p-2 rounded">
                Feature update
            </button>

            {isPopupVisible && (
                <div className="fixed inset-0 flex items-center justify-center bg-black bg-opacity-50">
                    <div className="bg-white p-6 rounded shadow-lg relative">
                        <button onClick={closePopup} className="absolute top-0 right-0 p-2">
                            X
                        </button>
                        <h2>Update Status</h2>
                        {data && <div className="mt-4 text-blue-700">Update successful! {JSON.stringify(data)}</div>}
                        {error && <div className="mt-4 text-red-500">Update failed: {error}</div>}
                    </div>
                </div>
            )}
        </div>
    );
}

export default UpdateVersion;

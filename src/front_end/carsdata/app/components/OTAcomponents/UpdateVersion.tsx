import React, { useState } from 'react';
import axios from 'axios';

async function updateECUVersion(ecuId: string, version: string): Promise<any> {
    const url = 'http://127.0.0.1:5000/api/update_to_version';
    const payload = {
        ecu_id: ecuId,
        version: version
    };

    try {
        const response = await axios.post(url, payload, {
            headers: {
                'Content-Type': 'application/json'
            }
        });
        return response.data;
    } catch (error) {
        console.error('Error updating ECU version:', error);
        throw error;
    }
}

const MyComponent: React.FC = () => {
    const [response, setResponse] = useState<any>(null);
    const [error, setError] = useState<string | null>(null);
    const [isPopupVisible, setIsPopupVisible] = useState(false);

    const handleUpdate = async () => {
        try {
            const result = await updateECUVersion('123', '2');
            setResponse(result);
            setError(null);
        } catch (err) {
            if (err instanceof Error) {
                setError(err.message);
            } else {
                setError('An unexpected error occurred');
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
                        {response && <div className="mt-4 text-blue-700">Update successful! {JSON.stringify(response)}</div>}
                        {error && <div className="mt-4 text-red-500">Update failed: {error}</div>}
                    </div>
                </div>
            )}
        </div>
    );
}

export default MyComponent;

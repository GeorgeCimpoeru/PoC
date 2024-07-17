import React, { useState } from 'react';

const DeploymentHistory = () => {
    const [showPage, setShowPage] = useState(false);

    const handleClick = () => {
        setShowPage(true);
    };

    const handleClose = () => {
        setShowPage(false);
    };

    return (
        <div>
            <button onClick={handleClick} className="custom-btn ml-6">
                Deployment history
            </button>

            {showPage && (
                <div className="page-content">
                    <div className="close-btn" onClick={handleClose}>
                        X
                    </div>
                    <h2>History deployment page</h2>
                    <p>Here you can see the history of software updates and downgrades.</p>

                    <div className="overflow-x-auto">
                        <table className="table">
                            {/* head */}
                            <thead>
                                <tr>
                                    <th></th>
                                    <th>Name</th>
                                    <th>Job</th>
                                    <th>Favorite Color</th>
                                </tr>
                            </thead>
                            <tbody>
                                {/* row 1 */}
                                <tr>
                                    <th>1</th>
                                    <td>Cy Ganderton</td>
                                    <td>Quality Control Specialist</td>
                                    <td>Blue</td>
                                </tr>
                                {/* row 2 */}
                                <tr>
                                    <th>2</th>
                                    <td>Hart Hagerty</td>
                                    <td>Desktop Support Technician</td>
                                    <td>Purple</td>
                                </tr>
                                {/* row 3 */}
                                <tr>
                                    <th>3</th>
                                    <td>Brice Swyre</td>
                                    <td>Tax Accountant</td>
                                    <td>Red</td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>
            )}

            <style jsx>{`
                .custom-btn {
                    padding: 10px 20px;
                    font-size: 16px;
                    border: 1px solid transparent;
                    border-radius: 4px;
                    background-color: #007bff;
                    color: white;
                    cursor: pointer;
                    transition: background-color 0.3s ease;
                }

                .custom-btn:hover {
                    background-color: #0056b3;
                }

                .page-content {
                    margin-top: 20px;
                    padding: 20px;
                    border: 1px solid #ccc;
                    border-radius: 4px;
                    background-color: #f8f9fa;
                    position: relative; /* Necesar pentru a poziționa butonul de închidere corect */
                }

                .close-btn {
                    position: absolute;
                    top: 10px;
                    right: 10px;
                    cursor: pointer;
                    font-size: 18px;
                    color: #555;
                }

                .close-btn:hover {
                    color: #333;
                }

                .overflow-x-auto {
                    /* Stilizare pentru a permite scroll orizontal */
                    overflow-x: auto;
                }

                .table {
                    /* Stilizare pentru tabel */
                    width: 100%;
                    border-collapse: collapse;
                }

                .table th, .table td {
                    padding: 8px;
                    border: 1px solid #ccc;
                }

                .table th {
                    background-color: #f0f0f0;
                }
            `}</style>
        </div>
    );
}

export default DeploymentHistory;



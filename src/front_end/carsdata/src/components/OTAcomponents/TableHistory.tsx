import React from 'react';
import ModalString from '../sharedComponents/ModalString';

const TableHistory = (props: any) => {
    return (
        <div className="overflow-x-auto m-5 relative">
            <table
                aria-label="simple table"
                style={{
                    backgroundColor: 'white',
                    color: 'black',
                    width: '100%',
                    borderCollapse: 'collapse',
                }}
            >
                <thead>
                    <tr>
                        <th
                            style={{
                                backgroundColor: '#f0f0f0',
                                fontSize: '1.25rem',
                                fontWeight: 'bold',
                                border: '1px solid #ddd',
                                padding: '8px',
                                textAlign: 'center',
                            }}
                        >
                            Artifact
                        </th>
                        <th
                            style={{
                                backgroundColor: '#f0f0f0',
                                fontSize: '1.25rem',
                                fontWeight: 'bold',
                                border: '1px solid #ddd',
                                padding: '8px',
                                textAlign: 'center',
                            }}
                        >
                            Status
                        </th>
                        <th
                            style={{
                                backgroundColor: '#f0f0f0',
                                fontSize: '1.25rem',
                                fontWeight: 'bold',
                                border: '1px solid #ddd',
                                padding: '8px',
                                textAlign: 'center',
                            }}
                        >
                            Start time
                        </th>
                        <th
                            style={{
                                backgroundColor: '#f0f0f0',
                                fontSize: '1.25rem',
                                fontWeight: 'bold',
                                border: '1px solid #ddd',
                                padding: '8px',
                                textAlign: 'center',
                            }}
                        >
                            Size
                        </th>
                        <th
                            style={{
                                backgroundColor: '#f0f0f0',
                                fontSize: '1.25rem',
                                fontWeight: 'bold',
                                border: '1px solid #ddd',
                                padding: '8px',
                                textAlign: 'center',
                            }}
                        >
                            Uploaded by
                        </th>
                        <th
                            style={{
                                backgroundColor: '#f0f0f0',
                                fontSize: '1.25rem',
                                fontWeight: 'bold',
                                border: '1px solid #ddd',
                                padding: '8px',
                                textAlign: 'center',
                            }}
                        >
                            Logs
                        </th>
                    </tr>
                </thead>
                <tbody>
                    {props.listOfUpdates.map((item: any, index: any) => (
                        <tr key={index}>
                            <td
                                style={{
                                    border: '1px solid #ddd',
                                    padding: '8px',
                                    textAlign: 'center',
                                }}
                            >
                                {item.artifact}
                            </td>
                            <td
                                style={{
                                    border: '1px solid #ddd',
                                    padding: '8px',
                                    textAlign: 'center',
                                    fontWeight: 'bold',
                                    color: item.status === 'Failed' ? 'red' : item.status === 'Succeded' ? 'green' : 'black',
                                }}
                            >
                                {item.status}
                            </td>
                            <td
                                style={{
                                    border: '1px solid #ddd',
                                    padding: '8px',
                                    textAlign: 'center',
                                }}
                            >
                                {item.startTime}
                            </td>
                            <td
                                style={{
                                    border: '1px solid #ddd',
                                    padding: '8px',
                                    textAlign: 'center',
                                }}
                            >
                                {item.size}
                            </td>
                            <td
                                style={{
                                    border: '1px solid #ddd',
                                    padding: '8px',
                                    textAlign: 'center',
                                }}
                            >
                                {item.uploadedBy}
                            </td>
                            <td
                                style={{
                                    border: '1px solid #ddd',
                                    padding: '8px',
                                    textAlign: 'center',
                                }}
                            >
                                <label htmlFor="my_modal_1" className="w-10 btn btn-sm">Log info</label>
                            </td>
                            <ModalString id="my_modal_1" cardTitle={'Log info'} />
                        </tr>
                    ))}
                </tbody>
            </table>

            <div className="absolute right-0 top-0 m-5">
            </div>
        </div>
    );
};

export default TableHistory;

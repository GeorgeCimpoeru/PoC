import React from 'react';
import ModalString from '../sharedComponents/ModalString';

const TableOta = (props: any) => {
    return (
        <div className="overflow-x-auto m-5 relative">
            <table className="table" aria-label="simple table">
                <thead>
                    <tr>
                        <td align="center" className="text-xl font-bold">Artifact</td>
                        <td align="center" className="text-xl font-bold">Status</td>
                        <td align="center" className="text-xl font-bold">Start time</td>
                        <td align="center" className="text-xl font-bold">Size</td>
                        <td align="center" className="text-xl font-bold">Uploaded by</td>
                        <td align="center" className="text-xl font-bold">Logs</td>
                    </tr>
                </thead>
                <tbody>
                    {props.listOfUpdates.map((item: any, index: any) => (
                        <tr key={index}>
                            <td align="center">{item.artifact}</td>
                            <td align="center">Failed</td>
                            <td align="center">Incomplete</td>
                            <td align="center">Completed</td>
                            <td align="center">Failed</td>
                            <td align="center"> <label htmlFor="my_modal_1" className="w-10 btn btn-sm">Log info</label></td>
                            <ModalString id="my_modal_1" cardTitle={'Door'} />
                        </tr>
                    ))}
                </tbody>
            </table>

            <div className="absolute right-0 top-0 m-5">
            </div>
        </div>
    )
}

export default TableOta

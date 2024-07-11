import React from 'react'

const TableOta = () => {
    return (
        <div className="overflow-x-auto m-5 relative">
            <table className="table-auto w-full bg-white shadow-xl rounded-lg border-collapse">
                <thead>
                    <tr>
                        <th className="px-4 py-2 border">Artifact</th>
                        <th className="px-4 py-2 border">Status</th>
                        <th className="px-4 py-2 border">Upload time</th>
                        <th className="px-4 py-2 border">Size</th>
                        <th className="px-4 py-2 border">Uploaded by
                        </th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td className="border px-4 py-2">Software update 1</td>
                        <td className="border px-4 py-2">Uploaded</td>
                        <td className="border px-4 py-2">July 9, 2024 16:23 PM</td>
                        <td className="border px-4 py-2">15.67 KB</td>
                        <td className="border px-4 py-2 border-r">x@randstaddigital.com</td>
                    </tr>
                    <tr>
                        <td className="border px-4 py-2">Software update 2</td>
                        <td className="border px-4 py-2">Failed</td>
                        <td className="border px-4 py-2">July 9, 2024 16:23 PM</td>
                        <td className="border px-4 py-2">15.67 KB</td>
                        <td className="border px-4 py-2 border-r">x@randstaddigital.com</td>
                    </tr>
                    <tr>
                        <td className="border px-4 py-2">Software update 3</td>
                        <td className="border px-4 py-2">Incomplete</td>
                        <td className="border px-4 py-2">July 9, 2024 16:23 PM</td>
                        <td className="border px-4 py-2">15.67 KB</td>
                        <td className="border px-4 py-2 border-r">x@randstaddigital.com</td>
                    </tr>
                </tbody>
            </table>
            <div className="absolute right-0 top-0 m-5">
            </div>
        </div>
    )
}

export default TableOta

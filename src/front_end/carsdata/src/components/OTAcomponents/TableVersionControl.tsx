'use client'
import React, { useEffect, useState } from 'react';
import { Table, Tooltip } from 'antd';
import UpgradeButton from './UpgradeButton';
import DowngradeButton from './DowngradeButton';

interface DeviceRecord {
    key: string;
    deviceName: string;
    currentVersion: string;
    softVersionsAvailable: string[];
}

const TableVersionControl = () => {
    const [newSoftVersions, setNewSoftVersions] = useState<string[]>([])
    const getNewSoftVersions = async () => {
        console.log("Getting new soft versions...");
        try {
            await fetch('http://127.0.0.1:5000/api/drive_update_data', {
                method: 'GET',
                mode: 'cors',
            }).then(response => response.json())
                .then(data => {
                    const versionsArray: string[] = [];
                    for (let i = 0; i < data.children[3].children.length; ++i) {
                        const versionNumber = data.children[3].children[i].name.match(/\d+(\.\d+)*|\d+/)?.[0];
                        if (versionNumber) {
                            versionsArray.push(versionNumber);
                        }
                    }

                    for (let i = 0; i < data.children[4].children.length; ++i) {
                        const versionNumber = data.children[4].children[i].name.match(/\d+(\.\d+)*|\d+/)?.[0];
                        if (versionNumber) {
                            versionsArray.push(versionNumber);
                        }
                    }
                    setNewSoftVersions(versionsArray);
                    console.log(data);

                });
        } catch (error) {
            console.log(error);
        }
    };

    useEffect(() => {
        getNewSoftVersions();
    }, []);

    const columns = [
        {
            title: (
                <span style={{ fontSize: '1.25rem', fontWeight: 'bold' }}>Device name</span>
            ),
            dataIndex: 'deviceName',
            key: 'deviceName',
            render: (text: string) => (
                <span style={{ fontSize: '1rem', fontWeight: 'normal' }}>{text}</span>
            ),
        },
        {
            title: (
                <span style={{ fontSize: '1.25rem', fontWeight: 'bold' }}>Current version</span>
            ),
            dataIndex: 'currentVersion',
            key: 'currentVersion',
            render: (text: string) => <span style={{ fontSize: '1rem' }}>{text}</span>,
        },
        {
            title: (
                <span style={{ fontSize: '1.25rem', fontWeight: 'bold' }}>Soft Versions Available</span>
            ),
            key: 'softVersionsAvailable',
            render: (_text: any, record: DeviceRecord) => {
                const versionsWithSpaces = record.softVersionsAvailable.join(';   ');
                return (
                    <Tooltip title={versionsWithSpaces}>
                        <span
                            style={{
                                whiteSpace: 'nowrap',
                                overflow: 'hidden',
                                textOverflow: 'ellipsis',
                                display: 'inline-block',
                                maxWidth: '200px',
                                fontSize: '1rem',
                            }}
                        >
                            {versionsWithSpaces}
                        </span>
                    </Tooltip>
                );
            },
        },
        {
            title: (
                <span style={{ fontSize: '1.25rem', fontWeight: 'bold' }}>Actions</span>
            ),
            key: 'actions',
            render: (_text: any, record: DeviceRecord) => (
                <div style={{ display: 'flex', gap: '8px', alignItems: 'center' }}>
                    <UpgradeButton />
                    <DowngradeButton />
                </div>
            ),
        },
    ];

    const data: DeviceRecord[] = [
        {
            key: '1',
            deviceName: 'Device X',
            currentVersion: '1.2.0',
            softVersionsAvailable: newSoftVersions,
        },
    ];

    return (
        <div>
            <Table
                columns={columns}
                dataSource={data}
                pagination={false}
                style={{ marginRight: '33px', marginLeft: '20px', fontFamily: 'Arial, sans-serif' }}
                rowClassName="custom-table-row"
            />
        </div>
    );
};
export default TableVersionControl;


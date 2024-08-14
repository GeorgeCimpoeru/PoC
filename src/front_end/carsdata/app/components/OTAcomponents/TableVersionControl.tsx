import React from 'react';
import { Table, Tooltip } from 'antd';
import UpgradeButton from './UpgradeButton';
import DowngradeButton from './DowngradeButton';

interface DeviceRecord {
    key: string;
    deviceName: string;
    currentVersion: string;
    softVersionsAvailable: string[];
}

const VersionControl = () => {
    const columns = [
        {
            title: <span style={{ fontSize: '20px', fontWeight: 'bold' }}>Device name</span>,
            dataIndex: 'deviceName',
            key: 'deviceName',
            render: (text: string) => <span style={{ fontSize: '20px', fontWeight: 'normal' }}>{text}</span>,
        },
        {
            title: <span style={{ fontSize: '20px', fontWeight: 'bold' }}>Current version</span>,
            dataIndex: 'currentVersion',
            key: 'currentVersion',
            render: (text: string) => <span>{text}</span>,
        },
        {
            title: <span style={{ fontSize: '20px', fontWeight: 'bold' }}>Soft Versions Available</span>,
            key: 'softVersionsAvailable',
            render: (_text: any, record: DeviceRecord) => {
                const versionsWithSpaces = record.softVersionsAvailable.join(';   ');
                return (
                    <Tooltip title={versionsWithSpaces}>
                        <span style={{ whiteSpace: 'nowrap', overflow: 'hidden', textOverflow: 'ellipsis', display: 'inline-block', maxWidth: '200px' }}>
                            {versionsWithSpaces}
                        </span>
                    </Tooltip>
                );
            },
        },
        {
            title: <span style={{ fontSize: '20px', fontWeight: 'bold' }}>Actions</span>,
            key: 'actions',
            render: (_text: any, record: DeviceRecord) => (
                <div style={{ display: 'flex', gap: '8px', alignItems: 'center' }}>
                    <UpgradeButton/>
                    <DowngradeButton/>
                </div>
            ),
        },
    ];

    const data: DeviceRecord[] = [
        {
            key: '1',
            deviceName: 'Device X',
            currentVersion: '1.2.0',
            softVersionsAvailable: ['1.2.0', '1.2.1', '1.2.2'],
        },
        
    ];

    return (
        <div>
            <Table
                columns={columns}
                dataSource={data}
                pagination={false}
                style={{ marginRight: '33px', marginLeft: '20px' }}
            />
        </div>
    );
};

export default VersionControl;

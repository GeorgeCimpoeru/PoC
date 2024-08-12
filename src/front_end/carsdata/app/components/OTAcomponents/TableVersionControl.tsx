import React, { useState } from 'react';
import { Table, Button, Modal, List } from 'antd';
import UpgradeButton from './UpgradeButton';
import DowngradeButton from './DowngradeButton';

interface DeviceRecord {
    key: string;
    deviceName: string;
    currentVersion: string;
    softVersionsAvailable: string[];
}

const VersionControl = () => {
    const [isModalOpen, setIsModalOpen] = useState(false);
    const [selectedVersions, setSelectedVersions] = useState<string[]>([]);

    const handleOpenModal = (versions: string[]) => {
        setSelectedVersions(versions);
        setIsModalOpen(true);
    };

    const handleCloseModal = () => {
        setIsModalOpen(false);
        setSelectedVersions([]);
    };

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
            render: (_text: any, record: DeviceRecord) => (
                <Button type="primary" onClick={() => handleOpenModal(record.softVersionsAvailable)}>
                    Show Versions
                </Button>
            ),
        },
        {
            title: <span style={{ fontSize: '20px', fontWeight: 'bold' }}>Actions</span>,
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
            currentVersion: '1.2.3',
            softVersionsAvailable: ['1.2.0', '1.2.1', '1.2.2', '1.2.3'],
        }
    ];

    return (
        <div>
            <Table
                columns={columns}
                dataSource={data}
                pagination={false}
                style={{ marginRight: '33px', marginLeft: '20px' }}
            />

            <Modal
                title={<span style={{ fontSize: '18px', fontWeight: 'bold' }}>Available Versions</span>}
                open={isModalOpen}
                onCancel={handleCloseModal}
                footer={null}
            >
                <List
                    bordered
                    dataSource={selectedVersions}
                    renderItem={version => <List.Item>{version}</List.Item>}
                />
            </Modal>
        </div>
    );
};

export default VersionControl;

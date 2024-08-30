'use client'
import React, { useState } from 'react';
import { Button, Modal } from 'antd';

const DowngradeButton = () => {
    const [isPopupVisible, setIsPopupVisible] = useState(false);
    const [isVersionPopupVisible, setIsVersionPopupVisible] = useState(false);
    const [selectedVersion, setSelectedVersion] = useState<string | null>(null);
    const [message, setMessage] = useState<string | null>(null);
    const [error, setError] = useState<string | null>(null);

    const showVersionPopup = () => setIsVersionPopupVisible(true);

    const handleVersionSelect = (version: string) => {
        setSelectedVersion(version);
        setIsVersionPopupVisible(false);
        handleDowngrade(version);
    };

    const handleDowngrade = async (version: string) => {
        try {
            const response = await fetch('http://127.0.0.1:5000/api/downgrade_to_version', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    ecu_id: "0x10",
                    version: "1"
                })
            });

            if (!response.ok) {
                throw new Error(`Network response was not ok: ${response.statusText}`);
            }

            const result = await response.json();

            if (result.ERROR || result["No of errors"] > 0) {
                setError(`Downgrade failed: ${result.ERROR} - Number of errors: ${result["No of errors"]}`);
                setMessage(null);
            } else if (result["already_installed"] === true) {
                setMessage('Already installed!');
                setError(null);
            } else {
                setMessage('Downgraded successfully!');
                setError(null);
            }
        } catch (error: unknown) {
            setError(error instanceof Error ? error.message : 'An unknown error occurred');
            setMessage(null);
        } finally {
            setIsPopupVisible(true);
        }
    };

    const closePopup = () => setIsPopupVisible(false);

    return (
        <div>
            <Button
                type="default"
                danger
                onClick={showVersionPopup}
                className="upgrade-downgrade-btn"
            >
                Downgrade
            </Button>

            <Modal
                title="Select Version"
                open={isVersionPopupVisible}
                onCancel={() => setIsVersionPopupVisible(false)}
                footer={[
                    <Button key="version1" onClick={() => handleVersionSelect('1')}>
                        Version 1.2.0
                    </Button>,
                    <Button key="version2" onClick={() => handleVersionSelect('2')}>
                        Version 1.2.1
                    </Button>,
                    <Button key="version3" onClick={() => handleVersionSelect('3')}>
                        Version 1.2.2
                    </Button>
                ]}
            >
                <p>Select one of the following versions for Downgrade:</p>
            </Modal>

            <Modal
                title="Downgrade Status"
                open={isPopupVisible}
                onCancel={closePopup}
                footer={[
                    <Button key="ok" onClick={closePopup}>
                        OK
                    </Button>
                ]}
            >
                {message && <div className="text-green-700">{message}</div>}
                {error && <div className="text-red-500">{error}</div>}
            </Modal>
        </div>
    );
};

export default DowngradeButton;

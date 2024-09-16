'use client'
import React, { useState } from 'react';
import { Button, Modal } from 'antd';

const UpgradeButton = () => {
    const [isVersionPopupVisible, setIsVersionPopupVisible] = useState(false);
    const [isProgressModalVisible, setIsProgressModalVisible] = useState(false);
    const [isStatusModalVisible, setIsStatusModalVisible] = useState(false);
    const [selectedVersion, setSelectedVersion] = useState<string | null>(null);
    const [message, setMessage] = useState<string | null>(null);
    const [error, setError] = useState<string | null>(null);
    const [progress, setProgress] = useState<number>(0);
    const [isOverlayVisible, setIsOverlayVisible] = useState(false);

    const showVersionPopup = () => setIsVersionPopupVisible(true);

    const handleVersionSelect = (version: string) => {
        setSelectedVersion(version);
        setIsVersionPopupVisible(false);
        handleUpdate(version);
    };

    const handleUpdate = async (version: string) => {
        try {
            setProgress(0);
            setIsOverlayVisible(true);
            setIsProgressModalVisible(true);

            const simulateProgress = setInterval(() => {
                setProgress(prev => {
                    if (prev < 100) {
                        return prev + 10;
                    } else {
                        clearInterval(simulateProgress);
                        return 100;
                    }
                });
            }, 500);

            // Simulate version update
            if (version === '1') {
                setMessage('Success: Version 1.2.0 installed successfully!');
                setError(null);
            } else if (version === '2') {
                setMessage(null);
                setError('Error: Failed to install Version 1.2.1');
            } else if (version === '3') {
                setMessage('Success: Version 1.2.2 is already installed');
                setError(null);
            }

            // Uncomment and update the URL as necessary
            // const response = await fetch('http://127.0.0.1:5000/api/update_to_version', { 
            //     method: 'POST',
            //     headers: {
            //         'Content-Type': 'application/json'
            //     },
            //     body: JSON.stringify({
            //         ecu_id: "0x11",
            //         version: version
            //     })
            // });

            // if (!response.ok) {
            //     throw new Error(`Network response was not ok: ${response.statusText}`);
            // }

            // const result = await response.json();

            // if (result.ERROR || result["No of errors"] > 0) {
            //     setError(`Update failed: ${result.ERROR} - Number of errors: ${result["No of errors"]}`);
            //     setMessage(null);
            // } else if (result["already_installed"] === true) {
            //     setMessage('Already installed');
            //     setError(null);
            // } else {
            //     setMessage('Downloaded successfully!');
            //     setError(null);
            // }
        } catch (error: unknown) {
            setError(error instanceof Error ? error.message : 'An unknown error occurred');
            setMessage(null);
        } finally {
            // Delay hiding the progress modal and showing the status modal
            setTimeout(() => {
                setIsOverlayVisible(false);
                setIsProgressModalVisible(false);
                setIsStatusModalVisible(true);
            }, 4000); // Wait for 4 seconds
        }
    };

    const closePopup = () => {
        setIsStatusModalVisible(false);
        setProgress(0);
    };

    return (
        <div>
            <Button
                type="primary"
                onClick={showVersionPopup}
                className="upgrade-downgrade-btn"
            >
                Upgrade
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
                <p>Select one of the following versions for Upgrade:</p>
            </Modal>

            {isProgressModalVisible && (
                <Modal
                    title="Updating..."
                    open={true}
                    footer={null}
                    closable={false}
                    maskClosable={false}
                >
                    <progress value={progress} max={100} style={{ width: '100%' }}></progress>
                    <div>Wait for the response...</div>
                </Modal>
            )}

            {isOverlayVisible && (
                <div
                    style={{
                        position: 'fixed',
                        top: 0,
                        left: 0,
                        width: '100%',
                        height: '100%',
                        backgroundColor: 'rgba(0, 0, 0, 0.5)',
                        zIndex: 999,
                    }}
                />
            )}

            {isStatusModalVisible && (
                <Modal
                    title="Update Status"
                    open={true}
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
            )}
        </div>
    );
};

export default UpgradeButton;

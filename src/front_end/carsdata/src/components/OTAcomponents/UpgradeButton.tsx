'use client'
import React, { useState, useEffect } from 'react';
import { Button, Modal } from 'antd';

const UpgradeButton = (props: any) => {
    const [isVersionPopupVisible, setIsVersionPopupVisible] = useState(false);
    const [isProgressModalVisible, setIsProgressModalVisible] = useState(false);
    const [isStatusModalVisible, setIsStatusModalVisible] = useState(false);
    const [selectedVersion, setSelectedVersion] = useState<string | null>(null);
    const [message, setMessage] = useState<string | null>(null);
    const [error, setError] = useState<string | null>(null);
    const [progress, setProgress] = useState<number>(0);
    const [isOverlayVisible, setIsOverlayVisible] = useState(false);

    const showVersionPopup = () => setIsVersionPopupVisible(true);

    const updateToVersion = async (version: string) => {
        let ecuId: string = "";
        if (props.device === "MCU") {
            ecuId = "10";
        } else if (props.device === "Battery") {
            ecuId = "11";
        } else if (props.device === "Engine") {
            ecuId = "12";
        } else if (props.device === "Doors") {
            ecuId = "13";
        } else if (props.device === "HVAC") {
            ecuId = "14";
        }
        console.log({
            ecu_id: ecuId,
            update_file_version: props.softVersDataForUpdate[version].swVersion,
            update_file_type: props.softVersDataForUpdate[version].type,
        });
        console.log("Updating version...")
        try {
            await fetch('http://127.0.0.1:5000/api/update_to_version', {
                method: 'POST',
                // mode: 'no-cors',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    update_file_version: props.softVersDataForUpdate[version].swVersion,
                    update_file_type: "zip",//props.softVersDataForUpdate[version].type,
                    ecu_id: ecuId,
                })
            }).then(response => response.json())
                .then(data => {
                    console.log(data);
                });
        } catch (error: unknown) {
            setError(error instanceof Error ? error.message : 'An unknown error occurred');
            setMessage(null);
        }
    };
    const closePopup = () => {
        setIsStatusModalVisible(false);
        setProgress(0);
    };

    useEffect(() => {
        
    }, [props.softVersions]); /////////?????????????

    return (
        <div className="m-1">
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
                footer={props.softVersions.map((version: any, index: any) => (
                    <Button className="m-1" key={version} onClick={() => updateToVersion(index)}>
                        {`Version ${version}`}
                    </Button>
                ))}>
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
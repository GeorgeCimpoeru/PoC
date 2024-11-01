'use client'
import React, { useState, useEffect } from 'react';
import { Button, Modal } from 'antd';
import {displayLoadingCircle, displayErrorPopup, removeLoadingCicle} from '../sharedComponents/LoadingCircle';
import logger from '@/src/utils/Logger';

const UpgradeButton = (props: any) => {
    logger.init();
    
    const [isVersionPopupVisible, setIsVersionPopupVisible] = useState(false);

    const showVersionPopup = () => setIsVersionPopupVisible(true);

    const updateToVersion = async (version: string) => {
        setIsVersionPopupVisible(false);
        displayLoadingCircle();
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
            update_file_version: props.softVersions[version],
            update_file_type: "zip",
            ecu_id: ecuId,
        })
        console.log("Updating version...")
        try {
            await fetch(`http://127.0.0.1:5000/api/update_to_version`, {
                method: 'POST',
                // mode: 'no-cors',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    update_file_version: props.softVersions[version],
                    update_file_type: "zip",
                    ecu_id: ecuId,
                })
            }).then(response => response.json())
                .then(data => {
                    console.log(data);
                    removeLoadingCicle();
                    if (data?.errors === "No errors.") {
                        displayErrorPopup("Upgrade successful");
                    } else {
                        displayErrorPopup("Upgrade failed");
                    }
                });
        } catch (error: any) {
            console.log(error);
            removeLoadingCicle();
        }
    };

    useEffect(() => {
        
    }, [props.softVersions]);

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
                    <Button className="m-1" key={'{version}-${index}'} onClick={() => updateToVersion(index)}>
                        {`Version ${version}`}
                    </Button>
                ))}>
                <p>Select one of the following versions for Upgrade:</p>
            </Modal>
        </div>
    );
};

export default UpgradeButton;
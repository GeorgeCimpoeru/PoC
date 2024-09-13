'use client'
import React, { useState, useEffect } from 'react';
import { Button, Modal } from 'antd';

const UpgradeButton = (props: any) => {
    const [isVersionPopupVisible, setIsVersionPopupVisible] = useState(false);
    let popupElement: HTMLDivElement | null = null;
    let popupStyleElement: HTMLStyleElement | null = null;
    let overlayElement: HTMLDivElement | null = null;

    const displayLoadingCircle = () => {
        if (popupElement || popupStyleElement || overlayElement) {
            return;
        }
    
        overlayElement = document.createElement('div');
        overlayElement.style.position = 'fixed';
        overlayElement.style.top = '0';
        overlayElement.style.left = '0';
        overlayElement.style.width = '100vw';
        overlayElement.style.height = '100vh';
        overlayElement.style.backgroundColor = 'rgba(0, 0, 0, 0.5)';
        overlayElement.style.zIndex = '999';
        overlayElement.style.pointerEvents = 'all';
        overlayElement.style.cursor = 'not-allowed';
    
        document.body.appendChild(overlayElement);
    
        popupElement = document.createElement('div');
        popupElement.style.position = 'fixed';
        popupElement.style.top = '50%';
        popupElement.style.left = '50%';
        popupElement.style.transform = 'translate(-50%, -50%)';
        popupElement.style.padding = '20px';
        popupElement.style.backgroundColor = 'rgba(0, 0, 0, 0.8)';
        popupElement.style.borderRadius = '10px';
        popupElement.style.zIndex = '1000';
        popupElement.style.textAlign = 'center';
    
        const loadingCircle = document.createElement('div');
        loadingCircle.style.width = '40px';
        loadingCircle.style.height = '40px';
        loadingCircle.style.border = '5px solid white';
        loadingCircle.style.borderTop = '5px solid transparent';
        loadingCircle.style.borderRadius = '50%';
        loadingCircle.style.animation = 'spin 1s linear infinite';
    
        popupElement.appendChild(loadingCircle);
    
        document.body.appendChild(popupElement);
    
        popupStyleElement = document.createElement('style');
        popupStyleElement.type = 'text/css';
        popupStyleElement.innerText = `
            @keyframes spin {
                0% { transform: rotate(0deg); }
                100% { transform: rotate(360deg); }
            }`;
        document.head.appendChild(popupStyleElement);
    };

    const displayErrorPopup = (text: string) => {
        const popup = document.createElement('div');
        popup.innerText = text;
        popup.style.position = 'fixed';
        popup.style.top = '50%';
        popup.style.left = '50%';
        popup.style.transform = 'translate(-50%, -50%)';
        popup.style.padding = '20px';
        popup.style.backgroundColor = 'rgba(0, 0, 0, 0.8)';
        popup.style.color = 'white';
        popup.style.borderRadius = '10px';
        popup.style.zIndex = '1000';
        popup.style.textAlign = 'center';

        document.body.appendChild(popup);

        setTimeout(() => {
            document.body.removeChild(popup);
        }, 2000);
    };

    const removeLoadingCicle = () => {
        if (popupElement && popupStyleElement && overlayElement) {
            document.body.removeChild(popupElement);
            document.head.removeChild(popupStyleElement);
            document.body.removeChild(overlayElement);
    
            popupElement = null;
            popupStyleElement = null;
            overlayElement = null;
        }
    };

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
        // removeLoadingCicle();
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
        </div>
    );
};

export default UpgradeButton;
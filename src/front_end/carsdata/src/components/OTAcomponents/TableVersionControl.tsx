'use client'
import React, { useEffect, useState } from 'react';
import UpgradeButton from './UpgradeButton';
import DowngradeButton from './DowngradeButton';

const TableVersionControl = (props: any) => {
    const [newSoftVersions, setNewSoftVersions] = useState<string[]>([]);
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

    const displayErrorPopup = () => {
        const popup = document.createElement('div');
        popup.innerText = "API connection failed";
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
        }, 1000);
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

    const getNewSoftVersions = async () => {
        console.log("Getting new soft versions...");
        displayLoadingCircle();
        setNewSoftVersions([]);
        try {
            await fetch(`http://127.0.0.1:5000/api/drive_update_data`, {
                method: 'GET',
                mode: 'cors',
            }).then(response => response.json())
                .then(data => {
                    console.log(data);
                    const versionsArray: string[] = [];
                    let aux = 8; // MCU
                    if (props.device === "Battery") {
                        aux = 7;
                    } else if (props.device === "Engine") {
                        aux = 2;
                    } else if (props.device === "Doors") {
                        aux = 1;
                    } else if (props.device === "HVAC") {
                        aux = 0;
                    }
                    for (let i = 0; i < data.children[aux].children.length; ++i) {
                        versionsArray.push(data.children[aux].children[i].sw_version);
                    }
                    setNewSoftVersions(versionsArray);
                });
        } catch (error) {
            console.log(error);
            setNewSoftVersions([]);
            displayErrorPopup();
            removeLoadingCicle();
        }
        removeLoadingCicle();
        console.log(newSoftVersions);
    };

    useEffect(() => {
        getNewSoftVersions();
    }, [props.device]); /////////?????????????

    return (
        <div className="rounded-sm border border-stroke bg-white px-5 pb-2.5 pt-6 shadow-default dark:border-strokedark dark:bg-boxdark sm:px-7.5 xl:pb-1 m-6">
            <div className="flex flex-col">
                <div className="grid grid-cols-3 rounded-sm bg-gray-2 dark:bg-meta-4 sm:grid-cols-4">
                    <div className="p-2.5 xl:p-5">
                        <h5 className="text-sm font-bold uppercase xsm:text-base">
                            Device
                        </h5>
                    </div>
                    <div className="p-2.5 xl:p-5">
                        <h5 className="text-sm font-bold uppercase xsm:text-base">
                            Current version
                        </h5>
                    </div>
                    <div className="p-2.5 xl:p-5">
                        <h5 className="text-sm font-bold uppercase xsm:text-base">
                            Soft Versions Available
                        </h5>
                    </div>
                    <div className="p-2.5 xl:p-5">
                        <h5 className="text-sm font-bold uppercase xsm:text-base">
                            Actions
                        </h5>
                    </div>
                </div>
                <div className="w-full h-px mt-4 bg-gray-300"></div>
                <div className="grid grid-cols-3 rounded-sm bg-gray-2 dark:bg-meta-4 sm:grid-cols-4">
                    <div className="m-1 p-2.5 xl:p-5">
                        <h5 className="text-sm font-medium uppercase xsm:text-base">
                            {props.device}
                        </h5>
                    </div>
                    <div className="m-1 p-2.5 xl:p-5">
                        <h5 className="text-sm font-medium uppercase xsm:text-base">
                            1.0
                        </h5>
                    </div>
                    <div className="m-1 p-2.5 xl:p-5">
                        <h5 className="text-sm font-medium uppercase xsm:text-base">
                            {newSoftVersions.join(';  ')}
                        </h5>
                    </div>
                    <div className="p-2.5 xl:p-5">
                        <h5 className="flex">
                            <UpgradeButton device={props.device} softVersions={newSoftVersions} />
                            <DowngradeButton />
                        </h5>
                    </div>
                </div>
            </div>
        </div>
    );
};

export default TableVersionControl;

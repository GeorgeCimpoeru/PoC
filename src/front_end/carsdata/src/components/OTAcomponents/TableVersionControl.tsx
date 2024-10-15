'use client'
import React, { useEffect, useState } from 'react';
import UpgradeButton from './UpgradeButton';
import DowngradeButton from './DowngradeButton';
import {removeLoadingCicle} from '../sharedComponents/LoadingCircle';

const TableVersionControl = (props: any) => {
    const [newSoftVersions, setNewSoftVersions] = useState<string[]>([]);

    const getNewSoftVersions = () => {
        setNewSoftVersions(props.versions.split('; '));
        removeLoadingCicle();
    };

    useEffect(() => {
        getNewSoftVersions();
    }, [props.device, props.versions]);

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
                            {newSoftVersions.join('; ')}
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

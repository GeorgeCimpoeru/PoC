import React from 'react'

const BatteryLife = (props: any) => {
    const batteryCapacity = props.doorparam
    const deviceConsumption = props.cigarette_lighter
    const batteryLife = batteryCapacity / deviceConsumption;
    return (
        <div className="card h-64 w-96 bg-green-300 text-black">
            <div className="card-body">
                <h2 className="card-title">Battery Life</h2>
                <p><strong>Battery Capacity:</strong> {props.doorparam} mAh</p>
                <p><strong>Device Consumption:</strong> {props.cigarette_lighter} mAh/hour</p>
                <p><strong>Battery Life:</strong> {batteryLife} hours</p>
                <div className="card-actions justify-end">
                </div>
            </div>
        </div>
    )
}

export default BatteryLife
import React, { useState } from 'react'

const ModalUDS = (props: any) => {
    const [input, setInput] = useState('');

    const handleInputChange = (event: any) => {
        setInput(event.target.value);
    }

    const handleInputClick = () => {
        setInput('');
    }

    const handleClickSaveBtn = () => {
        props.writeInfoBattery(props.param, input);
    }

    return (
        <>
            <input type="checkbox" id={props.id} className="modal-toggle" />
            <div className="modal text-black" role="dialog">
                <div className="modal-box">
                    <h3 className="text-lg font-bold">{props.cardTitle}</h3>
                    <p className="py-4">Insert new value for {props.cardTitle}</p>
                    <input type="number" placeholder="Type here" className="input input-bordered w-full max-w-xs" value={input} onClick={handleInputClick} onChange={handleInputChange} />
                    <input type="number" placeholder="Type here" className="input input-bordered w-full max-w-xs" value={input} onClick={handleInputClick} onChange={handleInputChange} />
                    <div className="modal-action">
                        <label htmlFor={props.id} className="btn">Close</label>
                        <label htmlFor={props.id} className="btn" onClick={handleClickSaveBtn}>Save</label>
                        <label htmlFor={props.id} className="btn" onClick={handleClickSaveBtn}>Save</label>
                    </div>
                </div>
            </div>
        </>
    )
}

export default ModalUDS;
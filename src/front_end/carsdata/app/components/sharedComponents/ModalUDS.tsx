import React, { useState } from 'react'

const ModalUDS = (props: any) => {
    const [input, setInput] = useState('');

    const handleChange = (event: any) => {
        props.handleInputChange(event.target.value, props.id);
        setInput(event.target.value);
    }

    const handleClick = () => {
        setInput('');
        props.handleInputClick(props.id);
    }

    const handleClose = () => {
        const checkbox = document.getElementById(props.id) as HTMLInputElement;
        checkbox.checked = false;
    }

    return (
        <>
            <input type="checkbox" id={props.id} className="modal-toggle" />
            <div className="modal text-black" role="dialog">
                <div className="modal-box">
                    <h3 className="text-lg font-bold">{props.cardTitle}</h3>
                    <p className="py-4">Insert new value for {props.cardTitle}</p>
                    <input type="number" placeholder="Type here" className="input input-bordered w-full max-w-xs" value={input} onClick={handleClick} onChange={handleChange} />
                    <div className="modal-action">
                        <label htmlFor={props.id} className="btn">Close</label>
                        <label htmlFor={props.id} className="btn" onClick={props.changeAPIdata}>Save</label>
                    </div>
                </div>
            </div>
        </>
    )
}

export default ModalUDS
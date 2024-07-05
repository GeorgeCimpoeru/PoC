import React, { useState } from 'react';

const Modal = (props: any) => {
    const [input, setInput] = useState('');

    const handleChange = (event: any) => {
        props.handleInputChange(event.target.value);
        setInput(event.target.value);
    }

    const handleClick = () => {
        setInput('');
        props.handleInputClick();
    }

    return (
        <>
            <input type="checkbox" id={props.id} className="modal-toggle" />
            <div className="modal" role="dialog">
                <div className="modal-box">
                    <h3 className="text-lg font-bold">{props.cardTitle}</h3>
                    <p className="py-4">Insert new value for {props.cardTitle}</p>
                    <input type="text" placeholder="Type here" className="input input-bordered w-full max-w-xs" value={input} onClick={handleClick} onChange={handleChange} />
                    <div className="modal-action">
                        <label htmlFor={props.id} className="btn">Save</label>
                    </div>
                </div>
            </div>
        </>
    );
};

export default Modal;
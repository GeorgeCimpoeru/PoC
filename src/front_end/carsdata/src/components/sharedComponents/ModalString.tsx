import React, { useState } from 'react';

const ModalString = (props: any) => {
    const [input, setInput] = useState('');

    const handleChange = (event: any) => {
        props.handleInputChange(event.target.value, props.id);
        setInput(event.target.value);
    }

    const handleClick = () => {
        setInput('');
        props.handleInputClick();
    }

    const handleClose = () => {
        const checkbox = document.getElementById(props.id) as HTMLInputElement;
        checkbox.checked = false;
    }

    return (
        <>
            <input type="checkbox" id={props.id} className="modal-toggle" />
            <div className="modal" role="dialog" onClick={handleClose}>
                <div className="modal-box" style={{ width: '80%', maxWidth: '1400px', height: '80%', maxHeight: '600px', position: 'relative' }} onClick={(e) => e.stopPropagation()}>
                    <h3 className="text-lg font-bold">{props.cardTitle}</h3>
                    <p className="py-4">Insert new value for {props.cardTitle}</p>
                    <input type="text" placeholder="Type here" className="input input-bordered w-full max-w-xs"
                        value={input} onClick={handleClick} onChange={handleChange} />
                    <div className="modal-action" style={{ position: 'absolute', bottom: '10px', right: '10px' }}>
                        <label htmlFor={props.id} className="btn">Close</label>
                    </div>
                </div>
            </div>
        </>
    );
};

export default ModalString;


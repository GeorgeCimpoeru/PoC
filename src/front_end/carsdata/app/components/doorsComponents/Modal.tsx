
import React, { useState } from 'react';

const Modal = (props: any) => {
    const [input, setInput] = useState('');

    const handleSelectChange = (event: React.ChangeEvent<HTMLSelectElement>) => {
        const selectedValue = event.target.value;
        setInput(selectedValue);
        props.handleInputChange(selectedValue);
    };

    return (
        <>
            <input type="checkbox" id={props.id} className="modal-toggle" />
            <div className="modal" role="dialog">
                <div className="modal-box">
                    <h3 className="text-lg font-bold">{props.cardTitle}</h3>
                    <p className="py-4">Please insert new value for {props.cardTitle}</p>
                    <select
                        className="input input-bordered block py-2.5 px-0 w-full text-sm text-gray-500 bg-transparent border-0 border-b-2 border-gray-200 appearance-none dark:text-gray-400 dark:border-gray-700 focus:outline-none focus:ring-0 focus:border-gray-200 peer"
                        value={input}
                        onChange={handleSelectChange}>

                        <option value="" disabled>Select an option</option>
                        <option value="Front Left">Front Left</option>
                        <option value="Front Right">Front Right</option>
                        <option value="Rear Left">Rear Left</option>
                        <option value="Rear Right">Rear Right</option>
                    </select>
                    <div className="modal-action">
                        <label htmlFor={props.id} className="btn">Save</label>
                    </div>
                </div>
            </div>
        </>
    );
};

export default Modal;

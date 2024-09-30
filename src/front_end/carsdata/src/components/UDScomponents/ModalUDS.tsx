import React, { useState } from 'react'

const ModalUDS = (props: any) => {
    const [input, setInput] = useState('');

    const handleInputChange = (event: any) => {
<<<<<<< HEAD:src/front_end/carsdata/src/components/sharedComponents/ModalUDS.tsx
        props.handleInputChange(event.target.value, props.id);
=======
>>>>>>> development:src/front_end/carsdata/src/components/UDScomponents/ModalUDS.tsx
        setInput(event.target.value);
    }

    const handleInputClick = () => {
<<<<<<< HEAD:src/front_end/carsdata/src/components/sharedComponents/ModalUDS.tsx
=======
    const handleInputClick = () => {
>>>>>>> development:src/front_end/carsdata/src/components/UDScomponents/ModalUDS.tsx
        setInput('');
    }

    const handleClickSaveBtn = () => {
<<<<<<< HEAD:src/front_end/carsdata/src/components/sharedComponents/ModalUDS.tsx
        props.writeInfoBattery("");
=======
        props.writeInfoBattery(props.param, input);
>>>>>>> development:src/front_end/carsdata/src/components/UDScomponents/ModalUDS.tsx
    }

    return (
        <>
            <input type="checkbox" id={props.id} className="modal-toggle" />
            <div className="modal text-black" role="dialog">
                <div className="modal-box">
                    <h3 className="text-lg font-bold">{props.cardTitle}</h3>
                    <p className="py-4">Insert new value for {props.cardTitle}</p>
                    <input type="number" placeholder="Type here" className="input input-bordered w-full max-w-xs" value={input} onClick={handleInputClick} onChange={handleInputChange} />
<<<<<<< HEAD:src/front_end/carsdata/src/components/sharedComponents/ModalUDS.tsx
                    <div className="modal-action">
                        <label htmlFor={props.id} className="btn">Close</label>
                        <label htmlFor={props.id} className="btn" onClick={handleClickSaveBtn}>Save</label>
=======
                    <input type="number" placeholder="Type here" className="input input-bordered w-full max-w-xs" value={input} onClick={handleInputClick} onChange={handleInputChange} />
                    <div className="modal-action">
                        <label htmlFor={props.id} className="btn">Close</label>
                        <label htmlFor={props.id} className="btn" onClick={handleClickSaveBtn}>Save</label>
                        <label htmlFor={props.id} className="btn" onClick={handleClickSaveBtn}>Save</label>
>>>>>>> development:src/front_end/carsdata/src/components/UDScomponents/ModalUDS.tsx
                    </div>
                </div>
            </div>
        </>
    )
}

export default ModalUDS;

import React, { useState } from "react";

const ModalVIN = () => {
    const [isOpen, setIsOpen] = useState(false);

    return (
        <div>
            <button className="btn" onClick={() => setIsOpen(true)}>Open Modal</button>
            {isOpen && (
                <div className="fixed inset-0 z-50 flex items-center justify-center bg-black bg-opacity-50">
                    <div className="modal-box">
                        <h3 className="font-bold text-lg">Hello!</h3>
                        <p className="py-4">Press ESC key or click the button below to close</p>
                        <div className="modal-action">
                            <button className="btn" onClick={() => setIsOpen(false)}>Close</button>
                        </div>
                    </div>
                </div>
            )}
        </div>
    );
};

export default ModalVIN;

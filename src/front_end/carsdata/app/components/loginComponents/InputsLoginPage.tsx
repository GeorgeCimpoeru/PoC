import React from 'react';

const InputsLoginPage = () => {
    return (
        <div className="text-center md:text-left flex-grow">
            <input
                className="text-sm w-full px-4 py-2 border border-solid border-gray-300 rounded"
                type="text"
                placeholder="Email Address"
            />
            <input
                className="text-sm w-full px-4 py-2 border border-solid border-gray-300 rounded mt-4"
                type="password"
                placeholder="Password"
            />
        </div>
    )
}

export default InputsLoginPage

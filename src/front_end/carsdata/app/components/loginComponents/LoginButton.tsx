'use client';
import React from 'react';

const LoginButton = () => {
    const handleLogin = () => {
        console.log('Login button clicked');
        // here we will create an event listener for all the logic behind login
    };
    return (
        <div>
            <button
                id="loginButton"
                className="mt-4 bg-blue-600 hover:bg-blue-700 px-4 py-2 text-white uppercase rounded text-xs tracking-wider"
                type="button"
                onClick={handleLogin}
            >
                Login
            </button>
        </div>
    )
}

export default LoginButton

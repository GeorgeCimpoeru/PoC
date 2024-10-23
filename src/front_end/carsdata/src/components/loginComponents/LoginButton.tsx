'use client';
import React from 'react';
import logger from '@/src/utils/Logger';

const LoginButton = () => {
    logger.init();
    
    const handleLogin = () => {
        console.log('Login button clicked');
        // here we will create an event listener for all the logic behind login
    };
    return (
        <div className="m-4">
            <button
                id="loginButton"
                className="btn btn-primary"
                type="button"
                onClick={handleLogin}
            >
                Log In
            </button>
        </div>
    )
}

export default LoginButton

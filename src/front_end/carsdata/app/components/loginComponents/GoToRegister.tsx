import React from 'react'

const GoToRegister = () => {
    return (
        <div className="mt-4 font-semibold text-sm text-slate-500 text-center md:text-left">
            Don&apos;t have an account?{" "}
            <a
                className="text-red-600 hover:underline hover:underline-offset-4"
                href="#"
            >
                Register
            </a>
        </div>
    )
}

export default GoToRegister

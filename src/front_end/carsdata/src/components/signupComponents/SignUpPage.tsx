'use client';
import React, { useState } from 'react';
import EmailInput from '@/src/components/signupComponents/EmailInput';
import PasswordInput from '@/src/components/signupComponents/PasswordInput';
import ConfirmPasswordInput from '@/src/components/signupComponents/ConfirmPasswordInput';
import SignUpButton from '@/src/components/signupComponents/SignUpButton';
import Link from "next/link";
import { database } from '@/src/app/firebaseConfig';

const SignUpPage = () => {
    const [email, setEmail] = useState("");
    const [password, setPassword] = useState("");
    const [confPassword, setConfPassword] = useState("");

    return (
        <div className='bg-gray-200 w-full min-h-screen flex justify-center items-center'>
            <div className='card card-compact w-96 bg-base-100 shadow-xl'>
                <div className='w-100 p-2 bg-white rounded-x1 fit-content'>
                    <form id='signUpForm'>
                        <div className='card-actions justify-center'>
                            <h2 className='p-1 my-1 card-title'>Sign Up</h2>
                        </div>
                        <EmailInput onEmailType={setEmail} />
                        <PasswordInput onPasswordType={setPassword} />
                        <ConfirmPasswordInput onConfPasswordType={setConfPassword} />
                        <SignUpButton email={email} password={password} confPassword={confPassword} database={database} />
                    </form>
                    <div className='card-actions justify-center'>
                        <h2 className='p-1 my-1'>
                            <Link href="/login">Back to Sign In {email}</Link>
                        </h2>
                    </div>
                </div>
            </div>
        </div>
    )
}

export default SignUpPage
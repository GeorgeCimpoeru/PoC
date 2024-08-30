import React from 'react';

const PasswordInput = (props: any) => {
    return (
        <div className='p-1 my-1 card-actions justify-center'>
            <input
                id='signUpPasswordInput'
                type='password'
                placeholder='Password'
                className='input input-bordered w-full max-w-xs'
                onChange={e => props.onPasswordType(e.target.value)}
            /><br></br>
        </div>
    )
}

export default PasswordInput
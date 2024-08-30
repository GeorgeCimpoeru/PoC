import React from 'react';

const ConfirmPasswordInput = (props: any) => {
    return (
        <div className='p-1 my-1 card-actions justify-center'>
            <input
                id='signUpConfEmailInput'
                type='password'
                placeholder='Confirm password'
                className='input input-bordered w-full max-w-xs'
                onChange={e => props.onConfPasswordType(e.target.value)}
            /><br></br>
        </div>
    )
}

export default ConfirmPasswordInput
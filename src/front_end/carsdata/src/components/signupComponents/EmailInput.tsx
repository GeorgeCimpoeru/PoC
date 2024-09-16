import React from 'react';

const EmailInput = (props: any) => {
    return (
        <div className='p-1 my-1 card-actions justify-center'>
            <input
                id='signUpEmailInput'
                type='text'
                placeholder='Email Address'
                className='input input-bordered w-full max-w-xs'
                onChange={e => props.onEmailType(e.target.value)}
            /><br></br>
        </div>
    )
}

export default EmailInput
'use client';
import React from 'react'
import styles from './SignUp.module.css'

const SignUpPage = () => {
  return (
    <div className='bg-gray-200 w-full min-h-screen flex justify-center items-center'>
      <div /*className={styles.div}*/ className='w-100 p-2 bg-white rounded-x1 fit-content'>
        <form /*onSubmit={handleSubmit}*/>
          <p className='items-center'>Sign Up</p>
          <label>Email</label><br></br>
          <input
            type="text"
            // value={inputValue}
            // onChange={handleInputChange}
            placeholder="Type email address"
          /><br></br>
          <label>Password</label><br></br>
          <input
            type="text"
            // value={inputValue}
            // onChange={handleInputChange}
            placeholder="Type email address"
          /><br></br>
          <button className='btn btn-primary' onClick={() => console.log('Click')}>Sign Up</button>
        </form>
      </div>
    </div>
  )
}

export default SignUpPage
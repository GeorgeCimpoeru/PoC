'use client';
import React from 'react';
import { push, ref, set } from 'firebase/database';
import { database } from '../firebaseConfig';


const SignUpButton = (props: any) => {

  const makeSignUp = () => {
    try {
      const usersRef = ref(database, 'users');
      const newDataRef = push(usersRef);
      set(newDataRef, {
        user: props.email,
        password: props.password,
      });
      alert('User registered');
    } catch(error) {
      alert('Database connection failed, try again later');
    }
  }

  return (
    <div className='p-1 my-1 card-actions justify-center'>
      <button className='btn btn-primary' onClick={makeSignUp}>Sign Up</button>
    </div>
  )
}

export default SignUpButton

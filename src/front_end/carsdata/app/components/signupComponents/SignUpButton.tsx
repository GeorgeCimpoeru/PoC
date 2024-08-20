'use client';
import React from 'react';
import { equalTo, get, orderByChild, push, query, ref } from 'firebase/database';

const SignUpButton = (props: any) => {

    const usersRef = ref(props.database, 'users');
    // const emailQuery = query(usersRef, orderByChild('users'), equalTo(props.email));

    const makeSignUp = async () => {
        //TO DO
        // alert('User 1');
        // try {
        //     alert('User 2');
        //     // Get the query result
        //     const snapshot = await get(emailQuery);
        //     alert('User 3');
        //     if (snapshot.exists()) {
        //         alert('User already exists');
        //     } else {
        //         // Push the new user data
        //         push(usersRef, {
        //             user: props.email,
        //             password: props.password
        //         });
        //         alert('User added successfully');
        //     }
        //     alert('User 4');
        // } catch (error) {
        //     alert('Error checking or adding user:');
        // }

        try {
            await push(usersRef, {
                user: props.email,
                password: props.password
            });
        } catch (error) {
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

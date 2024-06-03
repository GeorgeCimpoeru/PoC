import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyDL3BZBO0Q2iS5T9ZBtlc-jcM8qKKLdCkg",
  authDomain: "carsdata-8db4b.firebaseapp.com",
  databaseURL: "https://carsdata-8db4b-default-rtdb.europe-west1.firebasedatabase.app/",
  projectId: "carsdata-8db4b",
  storageBucket: "carsdata-8db4b.appspot.com",
  messagingSenderId: "696758433401",
  appId: "1:696758433401:web:9e198093e0feaf88f546a8",
  measurementId: "G-DGG26LFL3B"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);


export { database };
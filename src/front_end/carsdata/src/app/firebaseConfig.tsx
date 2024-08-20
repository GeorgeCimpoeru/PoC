import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
    apiKey: "AIzaSyDMDT80uPX8NbGpHybiPN8gdGJt4pRjdPU",
    authDomain: "nextjsrelatimetest.firebaseapp.com",
    databaseURL: "https://nextjsrelatimetest-default-rtdb.europe-west1.firebasedatabase.app",
    projectId: "nextjsrelatimetest",
    storageBucket: "nextjsrelatimetest.appspot.com",
    messagingSenderId: "439663572632",
    appId: "1:439663572632:web:545489a284b216d7a2c2ed",
    measurementId: "G-3QL2LV98FN"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

export { database };
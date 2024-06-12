import Image from "next/image";

export default function Home() {
    return (
        <main className="flex min-h-screen flex-col items-center justify-between p-24">
            <h1>this is home page</h1>
            <Link href="/login">Log In</Link>
            <Link href="/signup">Sign Up</Link>
            <Link href="/mainPage">MainPage</Link>
        </main>
    );
}

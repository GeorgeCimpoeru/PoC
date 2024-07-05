import Link from "next/link";

export default function Home() {
    return (
        <main className="flex min-h-screen flex-col items-center justify-between p-24">
            <div className="card bg-base-100 w-96 shadow-xl">
                <div className="card-body">
                    <h1 className="card-title">Home Page</h1>
                    <div className="card-actions justify-center">
                        <button className="w-40 btn btn-primary">
                            <Link href="/login">Log In</Link>
                        </button>
                    </div>
                    <div className="card-actions justify-center">
                        <button className="w-40 btn btn-primary">
                            <Link href="/signup">Sign Up</Link>
                        </button>
                    </div>
                    <div className="card-actions justify-center">
                        <button className="w-40 btn btn-primary">
                            <Link href="/mainPage">MainPage</Link>
                        </button>
                    </div>
                </div>
            </div>
        </main>
    );
}

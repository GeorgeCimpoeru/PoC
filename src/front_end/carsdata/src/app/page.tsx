import Link from "next/link";

export default function Home() {
    return (
        <main className="flex min-h-screen flex-col items-center justify-between p-24">
            <div className="card bg-base-100 w-96 shadow-xl">
                <div className="card-body">
                    <h1 className="card-title">Proof of Concept</h1>
                    <div className="card-actions justify-center">
                        <Link className="w-40 btn btn-primary" href="/login">Log In</Link>
                    </div>
                    <div className="card-actions justify-center">
                        <Link className="w-40 btn btn-primary" href="/signup">Sign Up</Link>
                    </div>
                    <div className="card-actions justify-center">
                        <Link className="w-40 btn btn-primary" href="/mainPage">Home</Link>
                    </div>
                </div>
            </div>
        </main>
    );
}

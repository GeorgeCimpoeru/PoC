import Link from "next/link";

const GoToRegister = () => {
    return (
        <div className="m-3 font-semibold text-sm text-slate-500 text-center md:text-left">
            <Link href="/signup">Do not have an account? Sign Up</Link>
        </div>
    )
}

export default GoToRegister

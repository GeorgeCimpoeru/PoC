import React from 'react'
import Link from "next/link";

const GoToRegister = () => {
  return (
    <div className="mt-4 font-semibold text-sm text-slate-500 text-center md:text-left">
      <Link href="/signup">Don't have an account? Sign Up</Link>
    </div>
  )
}

export default GoToRegister

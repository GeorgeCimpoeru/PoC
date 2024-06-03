import Image from "next/image";
import Link from "next/link";

export default function Home() {
  return (
    <main className="flex min-h-screen flex-col items-center justify-between p-24">
      <h1>this is home page</h1>
      <Link href="/login">Go To Login</Link>
      <Link href="/signup">Sign Up</Link>
    </main>
  );
}

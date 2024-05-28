import Image from "next/image";
import Link from "next/link";

export default function Home() {
  return (
    <main>
      <h1>this is home page</h1>
      <Link href="/login">Go To Login</Link>
    </main>
  );
}

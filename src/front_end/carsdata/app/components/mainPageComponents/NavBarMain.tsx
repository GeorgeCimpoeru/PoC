import React from 'react';
import Link from "next/link";

const NavBarMain = () => {
  return (
    <div className="navbar bg-base-100">
      <div className="flex-1">
        <a className="btn btn-ghost text-xl font-bold text-5xl ml-20">PoC</a>
      </div>
      <div className="flex-none">
        <ul className="menu menu-horizontal px-1">
          <li><Link href="/ota">OTA Page</Link></li>
          <li><Link href="/uds">UDS Page</Link></li>
          <li><a>Login</a></li>
          <li>
            <details>
              <summary>Contact Page</summary>
              <ul className="bg-base-100 rounded-t-none p-2">
                <li><a>Link 1</a></li>
                <li><a>Link 2</a></li>
              </ul>
            </details>
          </li>
        </ul>
      </div>
    </div>
  )
}

export default NavBarMain
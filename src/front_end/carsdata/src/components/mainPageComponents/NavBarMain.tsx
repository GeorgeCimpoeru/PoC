import React from 'react';
import Link from "next/link";

interface NavBarMainProps {
  isVinSubmitted: boolean;
}

const NavBarMain = ({ isVinSubmitted }: NavBarMainProps) => {
  return (
    <div className="fixed top-0 left-0 w-full bg-base-100 shadow-md z-50">
      <div className="flex items-center justify-between px-4 py-2">
        <a className="btn btn-ghost text-xl font-bold text-5xl pl-20">PoC</a>

        <div className="flex-none">
          <ul className="menu menu-horizontal px-1">
            {isVinSubmitted && (
              <>
                <li><Link href="/ota">OTA Page</Link></li>
                <li><Link href="/UDS">UDS Page</Link></li>
              </>
            )}
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
    </div>
  );
}

export default NavBarMain;

import React, { useState, useEffect } from 'react';
// import MyComponent from './UpdateVersion'; 
import UpdateVersion from './UpdateVersion';

const NavbarOtaSlides = () => {
    const [currentSlide, setCurrentSlide] = useState(0);

    const slides = [
        { id: "slide1", src: "/otaupdate2.jpeg" },
        { id: "slide2", src: "/otaupdate.png" }
    ];

    useEffect(() => {
        const interval = setInterval(() => {
            setCurrentSlide((prevSlide) => (prevSlide + 1) % slides.length);
        }, 3000);

        return () => clearInterval(interval);
    }, [slides.length]);

    return (
        <>
            {/* <nav className="bg-gray-300 rounded-lg m-5 p-6 shadow-xl flex justify-between items-center">
                <div className="text-gray-800 text-2xl font-bold">Over-the-Air Update</div>
                <button>
                    <UpdateVersion></UpdateVersion>
                </button>
            </nav> */}
            <div className="relative mx-5 flex justify-center">
                <div className="carousel max-w-4xl w-full h-96 relative overflow-hidden">
                    {slides.map((slide, index) => (
                        <div
                            key={slide.id}
                            className={`carousel-item absolute w-full h-full transition-opacity duration-1000 ease-in-out ${index === currentSlide ? 'opacity-100' : 'opacity-0'}`}
                        >
                            <img
                                src={slide.src}
                                className="w-full h-full object-cover"
                                alt={`Slide ${index + 1}`}
                            />
                        </div>
                    ))}
                </div>
                <div className="absolute top-1/2 left-1/2 transform -translate-x-1/2 -translate-y-1/2">
                    <button>
                        <UpdateVersion></UpdateVersion>
                    </button>
                </div>
            </div>
        </>
    );
}

export default NavbarOtaSlides;

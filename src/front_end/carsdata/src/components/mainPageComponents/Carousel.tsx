import React, { useState, useEffect } from 'react';
import Image from 'next/image';

const Carousel = () => {
    const [currentSlide, setCurrentSlide] = useState(0);
    const slides = [
        // './resized.jpg',
        './porsche.jpg',
        './banner.jpg',
        './vw(3).jpg'
    ];

    useEffect(() => {
        const interval = setInterval(() => {
            setCurrentSlide((prevSlide) => (prevSlide + 1) % slides.length);
        }, 5000); 

        return () => clearInterval(interval); 
    }, [slides.length]);

    return (
        <div className="carousel w-full">
            {slides.map((slide, index) => (
                <div
                    key={index}
                    className={`carousel-item relative w-full ${index === currentSlide ? 'block' : 'hidden'}`}
                >
                    <img src={slide} className="w-full" />
                </div>
            ))}
            {/* <div className="absolute left-5 right-5 top-1/2 flex -translate-y-1/2 transform justify-between">
                <button
                    onClick={() => setCurrentSlide((prevSlide) => (prevSlide - 1 + slides.length) % slides.length)}
                    className="btn btn-circle"
                >
                    ❮
                </button>
                <button
                    onClick={() => setCurrentSlide((prevSlide) => (prevSlide + 1) % slides.length)}
                    className="btn btn-circle"
                >
                    ❯
                </button>
            </div> */}
        </div>
    );
}

export default Carousel;

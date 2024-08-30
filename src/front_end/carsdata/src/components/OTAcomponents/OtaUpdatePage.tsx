import React from 'react';
import TableVersionControl from './TableVersionControl';

const OtaUpdatePage = () => {
    return (
        <>
            <div style={{
                position: 'fixed',
                top: 0,
                left: 0,
                width: '100vw',
                height: '100vh',
                backgroundImage: 'url(/car1.jpg)',
                backgroundSize: 'cover',
                backgroundPosition: 'center',
                opacity: 0.4,
                zIndex: -1, 
            }}>
            </div>

            <div style={{ margin: '60px 0', position: 'relative', zIndex: 1 }}>
                <TableVersionControl />
            </div>

            
            <div className="absolute top-1/2 left-1/2 transform -translate-x-1/2 -translate-y-1/2 z-10">
      
            </div>
        </>
    );
}

export default OtaUpdatePage;


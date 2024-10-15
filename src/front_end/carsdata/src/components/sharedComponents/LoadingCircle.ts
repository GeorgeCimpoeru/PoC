/* Methods for the Loading Screen */

let popupElement: HTMLDivElement | null = null;
let popupStyleElement: HTMLStyleElement | null = null;
let overlayElement: HTMLDivElement | null = null;

export function displayLoadingCircle () {
    if (popupElement || popupStyleElement || overlayElement) {
        return;
    }
    overlayElement = document.createElement('div');
    overlayElement.style.position = 'fixed';
    overlayElement.style.top = '0';
    overlayElement.style.left = '0';
    overlayElement.style.width = '100vw';
    overlayElement.style.height = '100vh';
    overlayElement.style.backgroundColor = 'rgba(0, 0, 0, 0.5)';
    overlayElement.style.zIndex = '999';
    overlayElement.style.pointerEvents = 'all';
    overlayElement.style.cursor = 'not-allowed';

    document.body.appendChild(overlayElement);

    popupElement = document.createElement('div');
    popupElement.style.position = 'fixed';
    popupElement.style.top = '50%';
    popupElement.style.left = '50%';
    popupElement.style.transform = 'translate(-50%, -50%)';
    popupElement.style.padding = '20px';
    popupElement.style.backgroundColor = 'rgba(0, 0, 0, 0.8)';
    popupElement.style.borderRadius = '10px';
    popupElement.style.zIndex = '1000';
    popupElement.style.textAlign = 'center';
      
    const loadingCircle = document.createElement('div');
    loadingCircle.style.width = '40px';
    loadingCircle.style.height = '40px';
    loadingCircle.style.border = '5px solid white';
    loadingCircle.style.borderTop = '5px solid transparent';
    loadingCircle.style.borderRadius = '50%';
    loadingCircle.style.animation = 'spin 1s linear infinite';

    popupElement.appendChild(loadingCircle);

    document.body.appendChild(popupElement);

    popupStyleElement = document.createElement('style');
    popupStyleElement.type = 'text/css';
    popupStyleElement.innerText = `
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }`;
    document.head.appendChild(popupStyleElement);
};

export function displayErrorPopup (text: string = 'API connection failed') {
    const popup = document.createElement('div');
    popup.innerText = text;
    popup.style.position = 'fixed';
    popup.style.top = '50%';
    popup.style.left = '50%';
    popup.style.transform = 'translate(-50%, -50%)';
    popup.style.padding = '20px';
    popup.style.backgroundColor = 'rgba(0, 0, 0, 0.8)';
    popup.style.color = 'white';
    popup.style.borderRadius = '10px';
    popup.style.zIndex = '1000';
    popup.style.textAlign = 'center';

    document.body.appendChild(popup);

    setTimeout(() => {
        document.body.removeChild(popup);
    }, 1000);
};

export function removeLoadingCicle()
    {
    if (popupElement && popupStyleElement && overlayElement) {
        document.body.removeChild(popupElement);
        document.head.removeChild(popupStyleElement);
        document.body.removeChild(overlayElement);
        popupElement = null;
        popupStyleElement = null;
        overlayElement = null;
    }
};
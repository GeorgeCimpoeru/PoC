function displayResponse(data) {
    const responseContainer = document.getElementById('response-output');
    responseContainer.textContent = JSON.stringify(data, null, 2);
}

function sendFrame() {
    let canId = document.getElementById('can-id').value;
    let canData = document.getElementById('can-data').value;

    if (!canId.match(/^0x[0-9A-Fa-f]{1,4}$/) || !canData.match(/^(0x[0-9A-Fa-f]{1,2},){0,7}0x[0-9A-Fa-f]{1,2}$/)) {
        alert('Invalid CAN ID or CAN Data format.');
        return;
    }

    fetch('/api/send_frame', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ can_id: canId, can_data: canData })
    })
    .then(response => response.json())
    .then(data => displayResponse(data))
    .catch(error => {
        console.error('Error:', error);
        displayResponse({ status: 'Error', message: error.toString() });
    });
}

function requestIds() {
    fetch('/api/request_ids', { method: 'POST' })
    .then(response => response.json())
    .then(data => displayResponse(data))
    .catch(error => {
        console.error('Error:', error);
        displayResponse({ status: 'Error', message: error.toString() });
    });
}

function updateToVersion() {
    let ecu_id = prompt("Enter ECU ID number:");
    let version = prompt("Enter version number:");
    fetch('/api/update_to_version', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ ecu_id: ecu_id, version: version })
    })
    .then(response => response.json())
    .then(data => displayResponse(data))
    .catch(error => {
        console.error('Error:', error);
        displayResponse({ status: 'Error', message: error.toString() });
    });
}

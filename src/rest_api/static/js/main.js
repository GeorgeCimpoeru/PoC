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
    .then(data => console.log(data))
    .catch(error => console.error('Error:', error));
}

function requestIds() {
    fetch('/api/request_ids', { method: 'POST' })
    .then(response => response.json())
    .then(data => console.log(data))
    .catch(error => console.error('Error:', error));
}

function updateToVersion() {
    let version = prompt("Enter version number:");
    fetch('/api/update_to_version', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ version: version })
    })
    .then(response => response.json())
    .then(data => console.log(data))
    .catch(error => console.error('Error:', error));
}

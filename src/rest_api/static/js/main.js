// Utility function to display JSON response in the output container
function displayResponse(data) {
    const responseContainer = document.getElementById('response-output');
    responseContainer.textContent = JSON.stringify(data, null, 2);
}

// Utility function to fetch logs and update the log table
function fetchLogs() {
    fetch('/api/logs')
        .then(response => response.json())
        .then(data => {
            const logBody = document.getElementById('log-body');
            logBody.innerHTML = '';

            data.logs.reverse().forEach((log, index) => {
                const row = document.createElement('tr');
                row.innerHTML = `<td>${index + 1}</td><td>${log}</td>`;
                logBody.appendChild(row);
            });
        })
        .catch(error => {
            console.error('Error fetching logs:', error);
        });
}

/**
 * Performs an API request using the Fetch API.
 * This method is a refactored version designed to avoid code repetition
 * by consolidating the fetch logic into a single function.
 * @param {string} url - The URL endpoint for the API request.
 * @param {string} method - The HTTP method for the request (e.g., 'GET', 'POST').
 * @param {Object|null} body - The body of the request, to be sent as JSON. Defaults to null if no body is provided.
 * @returns {Promise} - A promise that resolves to the JSON response from the server.
 */
function performApiRequest(url, method, body = null) {
    return fetch(url, {
        method: method,
        headers: {
            'Content-Type': 'application/json',
        },
        body: body ? JSON.stringify(body) : null,
    }).then(response => response.json())
      .then(data => {
          displayResponse(data);
          fetchLogs();
      })
      .catch(error => {
          console.error('Error:', error);
      });
}

function sendFrame() {
    const canId = document.getElementById('can-id').value;
    const canData = document.getElementById('can-data').value;
    if (!canId || !canData) {
        alert('CAN ID and CAN Data cannot be empty.');
        return;
    }

    performApiRequest('/api/send_frame', 'POST', { can_id: canId, can_data: canData });
}

function requestIds() {
    performApiRequest('/api/request_ids', 'GET');
}

function updateToVersion() {
    const type = prompt('Enter file type:');
    const version = prompt('Enter sw version:');

    performApiRequest('/api/update_to_version', 'POST', { update_file_type: type, update_file_version: version });
}

function readInfoBattery() {
    performApiRequest('/api/read_info_battery', 'GET');

}

function readInfoEngine() {
    performApiRequest('/api/read_info_engine', 'GET');
}


function gDriveReadData() {
    fetch('/api/drive_update_data', {
        method: 'GET',
    }).then(response => response.json())
      .then(data => {
          document.getElementById('response-output').textContent = JSON.stringify(data);
          fetchLogs();
      });
}

function readInfoDoors() {
    performApiRequest('/api/read_info_doors', 'GET');
}

function writeInfoDoors() {
    const data = {
        door: prompt('Enter Door Parameter:') || null,
        serial_number: prompt('Enter Serial Number:') || null,
        lighter_voltage: prompt('Enter Cigarette Lighter Voltage:') || null,
        light_state: prompt('Enter Light State:') || null,
        belt: prompt('Enter Belt Card State:') || null,
        windows_closed: prompt('Enter Window Status:') || null,
    };
    performApiRequest('/api/write_info_doors', 'POST', data);
}

function writeInfoBattery() {
    const data = {
        battery_level: prompt('Enter Battery Energy Level:') || null,
        voltage: prompt('Enter Battery Voltage:') || null,
        battery_state_of_charge: prompt('Enter Battery State of Charge:') || null,
        percentage: prompt('Enter Battery Percentage:') || null,
        // temperature: prompt('Enter Battery Temperature:') || null,
        // life_cycle: prompt('Enter Battery Life Cycle:') || null,
        // fully_charged: prompt('Enter Battery Fully Charged Status:') || null,
        // range_battery: prompt('Enter Battery Range:') || null,
        // charging_time: prompt('Enter Battery Charging Time:') || null,
        // device_consumption: prompt('Enter Device Consumption:') || null
    };
    performApiRequest('/api/write_info_battery', 'POST', data);
}


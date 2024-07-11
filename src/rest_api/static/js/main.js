function displayResponse(data) {
    const responseContainer = document.getElementById('response-output');
    responseContainer.textContent = JSON.stringify(data, null, 2);
}

function sendFrame() {
    const canId = document.getElementById('can-id').value;
    const canData = document.getElementById('can-data').value;
    fetch('/api/send_frame', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ can_id: canId, can_data: canData }),
    }).then(response => response.json())
      .then(data => {
          document.getElementById('response-output').textContent = JSON.stringify(data);
          fetchLogs(); 
      });
}

function requestIds() {
    fetch('/api/request_ids', {
        method: 'GET',
    }).then(response => response.json())
      .then(data => {
          document.getElementById('response-output').textContent = JSON.stringify(data);
          fetchLogs(); 
      });
}

function updateToVersion() {
    const ecuId = prompt('Enter ECU ID:');
    const version = prompt('Enter Version:');
    fetch('/api/update_to_version', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ ecu_id: ecuId, version: version }),
    }).then(response => response.json())
      .then(data => {
          document.getElementById('response-output').textContent = JSON.stringify(data);
          fetchLogs(); 
      });
}

function readInfoBattery() {
    fetch('/api/read_info_battery', {
        method: 'GET',
    }).then(response => response.json())
      .then(data => {
          document.getElementById('response-output').textContent = JSON.stringify(data);
          fetchLogs(); 
      });
}

function readInfoEngine() {
    fetch('/api/read_info_engine', {
        method: 'GET',
    }).then(response => response.json())
      .then(data => {
          document.getElementById('response-output').textContent = JSON.stringify(data);
          fetchLogs(); 
      });
}

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
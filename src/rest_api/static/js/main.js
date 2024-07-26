function displayResponse(data) {
    const responseContainer = document.getElementById('response-output');
    responseContainer.textContent = JSON.stringify(data, null, 2);
}

function sendFrame() {
    const canId = document.getElementById('can-id').value;
    const canData = document.getElementById('can-data').value;
    if (!canId || !canData) {
        alert('CAN ID and CAN Data cannot be empty.');
        return;
    }
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


function fetchLogs() {
    fetch('/api/logs', {
        method: 'GET',
    }).then(response => response.json())
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



    function readInfoDoors() {
        fetch('/api/read_info_doors', {
            method: 'GET',
        }).then(response => response.json())
          .then(data => {
              document.getElementById('response-output').textContent = JSON.stringify(data);
              fetchLogs(); 
          });
    }
    
    function writeInfoDoors() {
        const doorParam = prompt('Enter Door Parameter:');
        const serialNumber = prompt('Enter Serial Number:');
        const cigaretteLighterVoltage = prompt('Enter Cigarette Lighter Voltage:');
        const lightState = prompt('Enter Light State:');
        const beltCard = prompt('Enter Belt Card State:');
        const windowStatus = prompt('Enter Window Status:');
    
    
        const data = {
             Door_param: doorParam ? parseInt(doorParam) : null,
            Serial_number: serialNumber || null,
            Cigarette_Lighter_Voltage: cigaretteLighterVoltage ? parseFloat(cigaretteLighterVoltage) : null,
            Light_state: lightState || null,
            BeltCard: beltCard || null,
            WindowStatus: windowStatus || null,
         };
    
        fetch('/api/write_info_doors', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
        }).then(response => response.json())
          .then(data => {
              displayResponse(data);
              fetchLogs(); 
          });
    }
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

function writeInfoBattery()
{
    const battery_level=  prompt('Enter Battery Level:');
    const voltage=  prompt('Enter Voltage:');
    const battery_state_of_charge= prompt('Enter Battery State of charge:');
    const percentage= prompt('Enter Battery percentage:');
    // const life_cycle=  prompt('Enter Battery life cycle:');
    // const serial_number= prompt('Enter Battery serial number:');
    // const range_battery= prompt('Enter Battery Range:');
    // const charging_time= prompt('Enter Battery Charging Time:');
    // const device_consumption= prompt('Enter Battery Device consumption:');

    // Check if any required field is empty
    if (!battery_level || !voltage || !battery_state_of_charge || !percentage) 
    {
        alert('All fields are required. Please provide values for all inputs.');
        return;
    }

    // Determine fully_charged value based on percentage
    // const fully_charged = parseFloat(percentage) === 100;

    const data = {
        Battery_level:parseFloat(battery_level),
        Voltage:parseFloat(voltage),
        Battery_state_of_charge:parseFloat(battery_state_of_charge),
        Percentage:parseFloat(percentage),
        // Life_cycle:life_cycle,
        // Fully_charged:fully_charged, // It should be a boolean value
        // Serial_number:serial_number,
        // Range_battery:range_battery,
        // Charging_time:charging_time,
        // Device_consumption:device_consumption

    };
    
    fetch('/api/write_info_battery', {
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

function readInfoEngine() {
    fetch('/api/read_info_engine', {
        method: 'GET',
    }).then(response => response.json())
      .then(data => {
          document.getElementById('response-output').textContent = JSON.stringify(data);
          fetchLogs(); 
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
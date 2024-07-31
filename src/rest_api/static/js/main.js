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
        const door = prompt('Enter Door Parameter:');
        const serial_number = prompt('Enter Serial Number:');
        const lighter_voltage = prompt('Enter Cigarette Lighter Voltage:');
        const light_state = prompt('Enter Light State:');
        const belt = prompt('Enter Belt Card State:');
        const windows_closed = prompt('Enter Window Status:');
    
    
        const data = {
            door: door || null,
            serial_number: serial_number || null,
            lighter_voltage: lighter_voltage || null,
            light_state: light_state || null,
            belt: belt || null,
            windows_closed: windows_closed || null,
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

    function writeInfoBattery() {
        const energyLevel = prompt('Enter Battery Energy Level:');
        const voltage = prompt('Enter Battery Voltage:');
        const stateOfCharge = prompt('Enter Battery State of Charge:');
        const percentage = prompt('Enter Battery Percentage:');
        const temperature = prompt('Enter Battery Temperature:');
        const lifeCycle = prompt('Enter Battery Life Cycle:');
        const fullyCharged = prompt('Enter Battery Fully Charged Status:');
        const range = prompt('Enter Battery Range:');
        const chargingTime = prompt('Enter Battery Charging Time:');
        const deviceConsumption = prompt('Enter Device Consumption:');
        

        const data = {
            energy_level: energyLevel || null,
            voltage: voltage || null,
            battery_state_of_charge: stateOfCharge || null,
            percentage: percentage || null,
            temperature: temperature || null,
            life_cycle: lifeCycle || null,
            fully_charged: fullyCharged || null,
            range_battery: range || null,
            charging_time: chargingTime || null,
            device_consumption: deviceConsumption || null
        };
        
        
        fetch('/api/write_info_battery', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
        })
        .then(response => response.json())
        .then(data => {
            displayResponse(data);
            fetchLogs(); 
        })
        .catch(error => {
            console.error('Error:', error);
        });
    }
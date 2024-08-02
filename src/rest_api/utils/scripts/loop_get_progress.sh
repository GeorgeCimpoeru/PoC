#!/bin/bash
# Designed to loop a request to retrive the status of update to version endpoint

while true; do
    curl -X GET http://127.0.0.1:5000/api/update_status/0x10
    sleep 1
done
#!/bin/bash

# while true; do
#     response=$(curl -s -X GET http://localhost:5000/update_status/0x10)
#     echo "Response: $response"
#     if [[ "$response" == *"completed"* ]]; then
#         echo "Update completed."
#         break
#     fi
#     sleep 5
# done

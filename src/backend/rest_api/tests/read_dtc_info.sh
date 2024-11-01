#!/bin/bash

# ===============================================
# Automated Testing Script for /read_dtc_info API
# ===============================================
#
# This script performs automated testing of the /read_dtc_info
# endpoint, checking for various HTTP responses and validating
# the response content. All results are logged with timestamps.
#
# Usage:
#   ./test_read_dtc_info.sh
#
# Ensure that:
#   - The Flask server is running on http://127.0.0.1:5000
#   - curl is installed on your system
#
# ===============================================

# Set the base URL for the API
BASE_URL="http://127.0.0.1:5000/api/read_dtc_info"

# Define the log file with current timestamp
LOG_DIR="./test_logs"
mkdir -p "$LOG_DIR"
LOG_FILE="$LOG_DIR/test_read_dtc_info_$(date '+%Y%m%d_%H%M%S').log"

# Function to log messages with timestamps
log() {
    local message="$1"
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $message" | tee -a "$LOG_FILE"
}

# Initialize counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Define test cases as an array of associative arrays
declare -A TEST_CASES

# Test Case 1: 200 OK - Successful Request
TEST_CASES["Test_200_OK_description"]="Successful request with valid parameters."
TEST_CASES["Test_200_OK_command"]="curl -s -w \"%{http_code}\" -o /tmp/response_body.txt -X GET \"$BASE_URL?subfunc=1&dtc_mask_bits=testFailed&dtc_mask_bits=confirmedDTC&ecu_id=0x11\""
TEST_CASES["Test_200_OK_expected_code"]="200"
TEST_CASES["Test_200_OK_expected_body_contains"]="CAN_ID"

# Test Case 2: 400 Bad Request - Invalid ECU ID Format
TEST_CASES["Test_400_Invalid_ECU_description"]="Invalid ECU ID format (non-hexadecimal)."
TEST_CASES["Test_400_Invalid_ECU_command"]="curl -s -w \"%{http_code}\" -o /tmp/response_body.txt -X GET \"$BASE_URL?ecu_id=invalid_hex&subfunc=1&dtc_mask_bits=testFailed\""
TEST_CASES["Test_400_Invalid_ECU_expected_code"]="400"
TEST_CASES["Test_400_Invalid_ECU_expected_body_contains"]="Invalid ecu"

# Test Case 3: 400 Bad Request - Unsupported ECU ID
TEST_CASES["Test_400_Unsupported_ECU_description"]="Unsupported ECU ID (valid hex but not supported)."
TEST_CASES["Test_400_Unsupported_ECU_command"]="curl -s -w \"%{http_code}\" -o /tmp/response_body.txt -X GET \"$BASE_URL?ecu_id=0x99&subfunc=1&dtc_mask_bits=testFailed\""
TEST_CASES["Test_400_Unsupported_ECU_expected_code"]="400"
TEST_CASES["Test_400_Unsupported_ECU_expected_body_contains"]="not supported"

# Test Case 4: 400 Bad Request - Invalid Sub-function
TEST_CASES["Test_400_Invalid_Subfunc_description"]="Invalid sub-function (not 1 or 2)."
TEST_CASES["Test_400_Invalid_Subfunc_command"]="curl -s -w \"%{http_code}\" -o /tmp/response_body.txt -X GET \"$BASE_URL?ecu_id=0x11&subfunc=3&dtc_mask_bits=testFailed\""
TEST_CASES["Test_400_Invalid_Subfunc_expected_code"]="400"
TEST_CASES["Test_400_Invalid_Subfunc_expected_body_contains"]="Invalid sub-function"

# Test Case 5: 400 Bad Request - Invalid DTC Mask Bits
TEST_CASES["Test_400_Invalid_DTC_Mask_description"]="Invalid DTC mask bits (unsupported bit names)."
TEST_CASES["Test_400_Invalid_DTC_Mask_command"]="curl -s -w \"%{http_code}\" -o /tmp/response_body.txt -X GET \"$BASE_URL?ecu_id=0x11&subfunc=1&dtc_mask_bits=invalidBit\""
TEST_CASES["Test_400_Invalid_DTC_Mask_expected_code"]="400"
TEST_CASES["Test_400_Invalid_DTC_Mask_expected_body_contains"]="Invalid DTC mask bits"

# Test Case 6: 400 Bad Request - Multiple Validation Errors
TEST_CASES["Test_400_Multiple_Errors_description"]="Multiple validation errors in request parameters."
TEST_CASES["Test_400_Multiple_Errors_command"]="curl -s -w \"%{http_code}\" -o /tmp/response_body.txt -X GET \"$BASE_URL?ecu_id=invalid_hex&subfunc=5&dtc_mask_bits=invalidBit1&dtc_mask_bits=invalidBit2\""
TEST_CASES["Test_400_Multiple_Errors_expected_code"]="400"
TEST_CASES["Test_400_Multiple_Errors_expected_body_contains"]="Invalid ecu"

# Test Case 7: 500 Internal Server Error - Simulated Server Error
# Note: Requires server-side support to trigger the error
TEST_CASES["Test_500_Simulated_Error_description"]="Simulated server error via special parameter."
TEST_CASES["Test_500_Simulated_Error_command"]="curl -s -w \"%{http_code}\" -o /tmp/response_body.txt -X GET \"$BASE_URL?ecu_id=0x11&subfunc=1&dtc_mask_bits=testFailed&cause_server_error=true\""
TEST_CASES["Test_500_Simulated_Error_expected_code"]="500"
TEST_CASES["Test_500_Simulated_Error_expected_body_contains"]="An unexpected error occurred"

# Function to run a single test case
run_test() {
    local test_name="$1"
    local description="${TEST_CASES["${test_name}_description"]}"
    local command="${TEST_CASES["${test_name}_command"]}"
    local expected_code="${TEST_CASES["${test_name}_expected_code"]}"
    local expected_body="${TEST_CASES["${test_name}_expected_body_contains"]}"

    ((TOTAL_TESTS++))
    log "===== Running Test: $test_name ====="
    log "Description: $description"

    # Execute the curl command and capture the HTTP status code
    eval $command
    local http_code=$?
    # Actually, the exit code of curl is not the HTTP status code. To capture the HTTP status code, we need to modify the curl command.

    # Adjusted approach: Capture HTTP status code and response body
    # Let's modify the command to store status code and body
    # We'll redefine the command execution here
    response=$($command)
    http_code=$response
    # Read the response body from /tmp/response_body.txt
    response_body=$(cat /tmp/response_body.txt)

    # Validate HTTP status code
    if [ "$http_code" -eq "${expected_code}" ]; then
        code_status="PASS"
    else
        code_status="FAIL"
    fi

    # Validate response body contains expected substring
    if [[ "$response_body" == *"$expected_body"* ]]; then
        body_status="PASS"
    else
        body_status="FAIL"
    fi

    # Determine overall test result
    if [ "$code_status" = "PASS" ] && [ "$body_status" = "PASS" ]; then
        ((PASSED_TESTS++))
        log "Result: PASS"
    else
        ((FAILED_TESTS++))
        log "Result: FAIL"
    fi

    # Log details
    log "Expected HTTP Code: $expected_code, Actual HTTP Code: $http_code"
    log "Expected Body Contains: \"$expected_body\""
    log "Actual Response Body: $response_body"

    if [ "$code_status" != "PASS" ]; then
        log "HTTP Code Mismatch: Expected $expected_code but got $http_code"
    fi

    if [ "$body_status" != "PASS" ]; then
        log "Response Body Mismatch: Expected to contain \"$expected_body\" but it did not."
    fi

    log "===== End of Test: $test_name =====\n"
}

# Main Execution Starts Here
log "=========================================="
log "Starting Automated Testing of /read_dtc_info"
log "Log File: $LOG_FILE"
log "Start Time: $(date '+%Y-%m-%d %H:%M:%S')"
log "==========================================\n"

# List of test names
TEST_NAMES=(
    "Test_200_OK"
    "Test_400_Invalid_ECU"
    "Test_400_Unsupported_ECU"
    "Test_400_Invalid_Subfunc"
    "Test_400_Invalid_DTC_Mask"
    "Test_400_Multiple_Errors"
    "Test_500_Simulated_Error"
)

# Run each test case
for test in "${TEST_NAMES[@]}"; do
    run_test "$test"
done

# Summary of Test Results
log "=========================================="
log "Testing Completed."
log "Total Tests: $TOTAL_TESTS"
log "Passed Tests: $PASSED_TESTS"
log "Failed Tests: $FAILED_TESTS"
log "End Time: $(date '+%Y-%m-%d %H:%M:%S')"
log "=========================================="

# Optional: Display summary in console
echo -e "\n===== Test Summary ====="
echo "Total Tests   : $TOTAL_TESTS"
echo "Passed Tests  : $PASSED_TESTS"
echo "Failed Tests  : $FAILED_TESTS"
echo "Log File      : $LOG_FILE"
echo "========================"

# Clean up temporary response file
rm -f /tmp/response_body.txt

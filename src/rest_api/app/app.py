from flask import Flask, request, jsonify

app = Flask(__name__)

def checksum_validation(params):
    checksum_type = params.get('checksumType')
    file_id = params.get('fileId')
    expected_checksum = params.get('expectedChecksum')
    
    def validate_checksum(checksum_type, file_id, expected_checksum):
        calculated_checksum = "ABCDEF1234567890"
        return calculated_checksum == expected_checksum

    is_valid = validate_checksum(checksum_type, file_id, expected_checksum)

    result = {
        "service": "checksum_validation",
        "params": params,
        "is_valid": is_valid
    }
    return result

def data_validation(params):
    return {"service": "data_validation", "params": params}

def routine_control(data):
    return {"service": "routine_control", "data": data}

def write_data_by_identifier(data):
    return {"service": "write_data_by_identifier", "data": data}

def read_data_by_identifier(params):
    return {"service": "read_data_by_identifier", "params": params}

def request_update_status(params):
    return {"service": "request_update_status", "params": params}

def authentication(data):
    return {"service": "authentication", "data": data}

def request_download(data):
    return {"service": "request_download", "data": data}

def read_dtc_information(params):
    dtc_name = params.get('DTCname')
    DTC_number = params.get('number_of_DTC')
    related_info = params.get('related_information')
    return {"service": "read_dtc_information", "dtc_name": dtc_name , "DTC_number" : DTC_number, "related_info" : related_info}

def tester_present(data):
    presence_flag = data.get('presence_flag')
    requestType = data.get('requestType')
    period = data.get('period')
    session = data.get('session')
    result = {"presence_flag" : presence_flag,
                "requestType" : requestType,
                "period" : period,
                "session" : session,
                }
    return {"service": "tester_present", "data": result}

def transfer_data(data):
    action = data.get('action')
    block_Sequence = data.get('block_Sequence')
    memorylocation = data.get('memorylocation')
    result = {
            "action": action,
            "block_Sequence" : block_Sequence,
            "memorylocation" : memorylocation
            }
    return {"service": "transfer_data", "data": result}

def clear_diagnostic_information(data):
    system = data.get('system')
    action = data.get('action')
    DTC = data.get('DTC')
    return {"service": "clear_diagnostic_information",
        "data": {
            "system": system,
            "action": action,
            "DTC": DTC,
        }}

def access_timing_parameters(data):
    return {"service": "access_timing_parameters", "data": data}

def read_memory_by_address(params):
    return {"service": "read_memory_by_address", "params": params}

def session_diagnostic_control(data):
    return {"service": "session_diagnostic_control", "data": data}

def request_transfer_exit(data):
    return {"service": "request_transfer_exit", "data": data}


@app.route('/api', methods=['GET', 'POST', 'PUT'])
def handle_request():
    service = request.args.get('service')
    params = request.json if request.method in ['POST', 'PUT'] else request.args

    if service == 'checksum_validation':
        return jsonify(checksum_validation(params))
    elif service == 'data_validation':
        return jsonify(data_validation(params))
    elif service == 'routine_control':
        return jsonify(routine_control(params))
    elif service == 'write_data_by_identifier':
        return jsonify(write_data_by_identifier(params))
    elif service == 'read_data_by_identifier':
        return jsonify(read_data_by_identifier(params))
    elif service == 'request_update_status':
        return jsonify(request_update_status(params))
    elif service == 'authentication':
        return jsonify(authentication(params))
    elif service == 'request_download':
        return jsonify(request_download(params))
    elif service == 'read_dtc_information':
        return jsonify(read_dtc_information(params))
    elif service == 'tester_present':
        return jsonify(tester_present(params))
    elif service == 'transfer_data':
        return jsonify(transfer_data(params))
    elif service == 'clear_diagnostic_information':
        return jsonify(clear_diagnostic_information(params))
    elif service == 'access_timing_parameters':
        return jsonify(access_timing_parameters(params))
    elif service == 'read_memory_by_address':
        return jsonify(read_memory_by_address(params))
    elif service == 'session_diagnostic_control':
        return jsonify(session_diagnostic_control(params))
    elif service == 'request_transfer_exit':
        return jsonify(request_transfer_exit(params))
    else:
        return jsonify({"error": "Invalid service"}), 400

if __name__ == '__main__':
    app.run(debug=True)

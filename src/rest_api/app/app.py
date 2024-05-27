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

def read_data_by_identifier(params):
    data_format = params.get('data_format', 'dummy')
    timeout = params.get('timeout', 5)  
    identifier = params.get('identifier')
    encryption = params.get('encryption', False)  

    return {"service": "read_data_by_identifier", "data_Format": data_format, "timeout": timeout, "identifier": identifier, "encryption": encryption}


def request_update_status(params):
    timestamp = params.get('timestamp')
    priority = params.get('priority')
    user_context = params.get('userContext')
    user_id = user_context.get('userId') if user_context else None
    roles = user_context.get('roles') if user_context else None
   
    return {"service": "request_update", "timestamp": timestamp, "priority": priority, "user_context": user_context , "user_id": user_id, "roles": roles}

def authentication(data):
    auth_type = data.get('auth_type')
    description = data.get('description')
    payload = data.get('payload')
    user = data.get('user')
    password = data.get('password')
    
    return {"service": "authentication", 'auth_type': auth_type, 'description': description, 'payload': payload, 'user': user, 'password': password}

def request_download(data):
    type = params.post('type')
    description = params.post('description')
    payload = params.post('payload')
    deviceId = params.post('deviceId')
    url = params.post('url')
    version = params.post('version')
    
    return {"service": "request_download", 'type': type, 'description': description, 'payload': payload, 'deviceId': deviceId, 'url': url, 'version': version}

def read_dtc_information(params):
    return {"service": "read_dtc_information", "params": params}

def tester_present(data):
    return {"service": "tester_present", "data": data}

def transfer_data(data):
    return {"service": "transfer_data", "data": data}

def clear_diagnostic_information(data):
    return {"service": "clear_diagnostic_information", "data": data}

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

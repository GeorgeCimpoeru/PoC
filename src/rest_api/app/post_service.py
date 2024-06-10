from service_registry import register_service

@register_service('routine_control', ['POST'])
def routine_control(data):
    """
    Control a specific routine by starting, stopping, or getting results.

    Args:
        data (dict): Dictionary containing the control parameters, including 'routineIdentifier', 'controlType', 'routineParameters', and 'timeout'.

    Returns:
        dict: Result of the routine control including the service name, data, and execution result.

    Relevant UDS Service: Routine Control
    SID: 0x31
    """
    routine_identifier = data.get('routineIdentifier')
    control_type = data.get('controlType')
    routine_parameters = data.get('routineParameters')
    timeout = data.get('timeout')

    def execute_routine_control(routine_identifier, control_type, routine_parameters, timeout):
        if control_type == "start":
            result = f"Routine {routine_identifier} started with parameters {routine_parameters} and timeout {timeout}"
        elif control_type == "stop":
            result = f"Routine {routine_identifier} stopped"
        elif control_type == "result":
            result = f"Result for routine {routine_identifier}: Success"
        else:
            result = "Invalid control type"
        return result

    result = execute_routine_control(routine_identifier, control_type, routine_parameters, timeout)

    response = {
        "service": "routine_control",
        "data": data,
        "result": result
    }
    return response

@register_service('authentication', ['POST'])
def authentication(data):
    """
    Perform authentication using provided credentials.

    Args:
        data (dict): Dictionary containing the authentication details, including 'auth_type', 'description', 'payload', 'user', and 'password'.

    Returns:
        dict: Result of the authentication including the service name and provided details.

    Relevant UDS Service: Authentication
    SID: 0x29
    """
    auth_type = data.get('auth_type')
    description = data.get('description')
    payload = data.get('payload')
    user = payload.get('user')
    password = payload.get('password')
    
    data = {"service": "authentication", "auth_type": auth_type, "description": description, 'payload': payload, 'user': user, 'password': password}
    return data

@register_service('request_download', ['POST'])
def request_download(data):
    """
    Request a download operation.

    Args:
        data (dict): Dictionary containing the download request details, including 'type', 'description', 'payload', 'deviceId', 'url', and 'version'.

    Returns:
        dict: Details of the download request including the service name and provided data.

    Relevant OTA Service: Request Download
    SID: 0x34
    """
    type_ = data.get('type')
    description = data.get('description')
    payload = data.get('payload')
    device_id = payload.get('deviceId')
    url = payload.get('url')
    version = payload.get('version')
    
    data = { "service": "request_download",'type': type_, 'description': description, 'payload': payload, 'device_id': device_id, 'url': url, 'version': version}
    return data

@register_service('tester_present', ['POST'])
def tester_present(data):
    """
    Indicate tester presence in a session.

    Args:
        data (dict): Dictionary containing the tester presence details, including 'presence_flag', 'requestType', 'period', and 'session'.

    Returns:
        dict: Details of the tester presence including the service name and provided data.

    Relevant UDS Service: Tester Present
    SID: 0x3E
    """
    presence_flag = data.get('presence_flag')
    request_type = data.get('requestType')
    period = data.get('period')
    session = data.get('session')
    result = {
        "presence_flag": presence_flag,
        "requestType": request_type,
        "period": period,
        "session": session,
    }
    return {"service": "tester_present", "data": result}

@register_service('transfer_data', ['POST'])
def transfer_data(data):
    """
    Transfer data with specified parameters.

    Args:
        data (dict): Dictionary containing the data transfer details, including 'action', 'block_Sequence', and 'memorylocation'.

    Returns:
        dict: Details of the data transfer including the service name and provided data.

    Relevant UDS Service: Transfer Data
    SID: 0x36
    """
    action = data.get('action')
    block_sequence = data.get('block_Sequence')
    memory_location = data.get('memorylocation')
    result = {
        "action": action,
        "block_Sequence": block_sequence,
        "memorylocation": memory_location
    }
    return {"service": "transfer_data", "data": result}

@register_service('access_timing_parameters', ['POST'])
def access_timing_parameters(data):
    """
    Access and modify timing parameters.

    Args:
        data (dict): Dictionary containing the timing parameters, including 'session', 'timingType', 'timingValue', and 'retryCount'.

    Returns:
        dict: Details of the timing parameter access including the service name and provided data.

    Relevant UDS Service: Access Timing Parameters
    SID: 0x83
    """
    session = data.get('session')
    timing_type = data.get('timingType')
    timing_value = data.get('timingValue')
    retry_count = data.get('retryCount')
    return {
        "service": "access_timing_parameters",
        "data": {
            "session": session,
            "timingType": timing_type,
            "timingValue": timing_value,
            "retryCount": retry_count
        }
    }

@register_service('session_diagnostic_control', ['POST'])
def session_diagnostic_control(data):
    """
    Control a diagnostic session.

    Args:
        data (dict): Dictionary containing the session control parameters, including 'session_type' and 'timeout'.

    Returns:
        dict: Details of the session control operation including the service name and provided data.

    Relevant UDS Service: Session Diagnostic Control
    SID: 0x10
    """
    session_type = data.get('session_type')
    timeout = data.get('timeout')
    return {
        "service": "session_diagnostic_control",
        "data": {
            "session_type": session_type,
            "timeout": timeout,
        }
    }
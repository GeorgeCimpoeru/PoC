from service_registry import register_service

@register_service('checksum_validation', ['GET'])
def checksum_validation(params):
    """
    Validate the checksum of a file.

    Args:
        params (dict): Dictionary containing the parameters for validation, including 'checksumType', 'fileId', and 'expectedChecksum'.

    Returns:
        dict: Result of the checksum validation including the service name, parameters, and validation result.

    """
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

@register_service('data_validation', ['GET'])
def data_validation(params):
    """
    Validate data according to specified rules.

    Args:
        params (dict): Dictionary containing the parameters for validation, including 'schemaVersion', 'dataId', and 'validationRules'.

    Returns:
        dict: Result of the data validation including the service name, parameters, and validation result.

    """
    schema_version = params.get('schemaVersion')
    data_id = params.get('dataId')
    validation_rules = params.get('validationRules')
    
    def validate_data(schema_version, data_id, validation_rules):
        validation_passed = True
        for rule in validation_rules:
            if rule == "rule1":
                validation_passed = validation_passed and True
            elif rule == "rule2":
                validation_passed = validation_passed and True
        return validation_passed

    is_valid = validate_data(schema_version, data_id, validation_rules)

    result = {
        "service": "data_validation",
        "params": params,
        "is_valid": is_valid
    }
    return result

@register_service('read_data_by_identifier', ['GET'])
def read_data_by_identifier(params):
    """
    Read data by a specific identifier.

    Args:
        params (dict): Dictionary containing the parameters for reading, including 'data_format', 'timeout', 'identifier', and 'encryption'.

    Returns:
        dict: Details of the read operation including the service name and parameters.

    Relevant UDS Service: Read Data by Identifier
    SID: 0x22
    """
    data_format = params.get('data_format', 'dummy')
    timeout = params.get('timeout', 5)  
    identifier = params.get('identifier')
    encryption = params.get('encryption', False)  

    query_params = {
        "data_Format": data_format,
        "timeout": timeout,
        "identifier": identifier,
        "encryption": encryption
    }

    result = {
        "service": "read_data_by_identifier", 
        "method": "GET",
        "query_params": query_params

    }
    return result

@register_service('request_update_status', ['GET'])
def request_update_status(params):
    """
    Request the update status.

    Args:
        params (dict): Dictionary containing the parameters for the request, including 'timestamp', 'priority', and 'userContext'.

    Returns:
        dict: Details of the update status request including the service name and parameters.

    Relevant OTA Service: Request Update Status
    SID: 0x32
    """
    timestamp = params.get('timestamp')
    priority = params.get('priority')
    user_context = params.get('userContext')
    user_id = user_context.get('userId') if user_context else None
    roles = user_context.get('roles') if user_context else None
   
    result = {
        "timestamp": timestamp,
        "priority": priority,
        "user_context": user_context,
        "user_id": user_id,
        "roles": roles
    }

    return result

@register_service('read_dtc_information', ['GET'])
def read_dtc_information(params):
    """
    Read Diagnostic Trouble Code (DTC) information.

    Args:
        params (dict): Dictionary containing the DTC information parameters, including 'DTCname', 'number_of_DTC', and 'related_information'.

    Returns:
        dict: Details of the DTC information read including the service name and query parameters.

    Relevant UDS Service: Read DTC Information
    SID: 0x19
    """
    dtc_name = params.get('DTCname')
    DTC_number = params.get('number_of_DTC')
    related_info = params.get('related_information')

    query_params = {
        "dtc_name": dtc_name,
        "DTC_number": DTC_number,
        "related_info": related_info
    }
    result = {
        "service": "read_dtc_information",
        "method": "GET",
        "query_params": query_params
    }
    return result

@register_service('read_memory_by_address', ['GET'])
def read_memory_by_address(params):
    """
    Read memory by address.

    Args:
        params (dict): Dictionary containing the memory read parameters, including 'address' and 'length'.

    Returns:
        dict: Details of the memory read operation including the service name, address, length, and read data.

    Relevant UDS Service: Read Memory by Address
    SID: 0x23
    """
    address = params.get('address', '0x1234')
    length = params.get('length', '16')
    data = "dummy_data"
    query_params = {
        "adress": address,
        "length": length,
        "data": data
    }

    result = {
        "service": "read_memory_by_address",
        "query_params": query_params
    }

    return result
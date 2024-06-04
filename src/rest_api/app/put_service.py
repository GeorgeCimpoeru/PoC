from service_registry import register_service

@register_service('write_data_by_identifier', ['PUT'])
def write_data_by_identifier(data):
    """
    Write data to a specific identifier.

    Args:
        data (dict): Dictionary containing the write parameters, including 'identifier', 'writeType', and 'dataPayload'.

    Returns:
        dict: Result of the write operation including the service name, data, and write result.

    Relevant UDS Service: Write Data by Identifier
    SID: 0x2E
    """
    identifier = data.get('identifier')
    write_type = data.get('writeType')
    data_payload = data.get('dataPayload')

    address = data_payload.get('address')
    data_to_write = data_payload.get('data')
    length = data_payload.get('length')

    def perform_write_operation(identifier, write_type, address, data, length):
        result = f"Data {data} written to address {address} with length {length} as {write_type} operation using identifier {identifier}"
        return result

    result = perform_write_operation(identifier, write_type, address, data_to_write, length)

    response = {
        "service": "write_data_by_identifier",
        "data": data,
        "result": result
    }
    return response


@register_service('clear_diagnostic_information', ['PUT'])
def clear_diagnostic_information(data):
    """
    Clear diagnostic information.

    Args:
        data (dict): Dictionary containing the diagnostic information to clear, including 'system', 'action', and 'DTC'.

    Returns:
        dict: Details of the clear diagnostic operation including the service name and provided data.

    Relevant UDS Service: Clear Diagnostic Information
    SID: 0x14
    """
    system = data.get('system')
    action = data.get('action')
    DTC = data.get('DTC')
    return {"service": "clear_diagnostic_information",
            "data": {
                "system": system,
                "action": action,
                "DTC": DTC,
            }}


@register_service('request_transfer_exit', ['PUT'])
def request_transfer_exit(data):
    """
    Request to exit a data transfer session.

    Args:
        data (dict): Dictionary containing the transfer exit parameters, including 'blockSequenceCounter', 'checksum', and 'transfer_result'.

    Returns:
        dict: Details of the transfer exit request including the service name and provided data.

    Relevant UDS Service: Request Transfer Exit
    SID: 0x37
    """
    block_sequence_counter = data.get('blockSequenceCounter', '10')
    checksum = data.get('checksum', 'ABCDEF1234567890')
    transfer_result = data.get('transferResult', 'success')
    return {"service": "request_transfer_exit",
            "data": {
                "blockSequenceCounter": block_sequence_counter,
                "checksum": checksum,
                "transfer_result": transfer_result,
            }}
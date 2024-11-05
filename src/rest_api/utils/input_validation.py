from functools import wraps
from flask import request, jsonify


def validate_update_request(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        data = request.get_json()
        errors = {}

        update_file_type = data.get('update_file_type')
        if update_file_type not in ['zip', 'tar']:
            errors['update_file_type'] = 'Invalid value. Allowed values are "zip" or "tar".'

        update_file_version = data.get('update_file_version')
        if not isinstance(update_file_version, str) or not update_file_version.replace('.', '').isdigit() or update_file_version.count('.') != 1:
            errors['update_file_version'] = 'Invalid format. Must be in the format "0.0" and contain only numbers.'

        ecu_id = data.get('ecu_id')
        valid_ecu_ids = ['0x10', '0x11', '0x12', '0x13', "0x14"]
        if ecu_id not in valid_ecu_ids:
            errors['ecu_id'] = 'Invalid value. No such device found.'

        if errors:
            return jsonify({'errors': errors}), 400

        return func(*args, **kwargs)
    return wrapper

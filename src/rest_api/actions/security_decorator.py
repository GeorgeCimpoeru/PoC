from functools import wraps
from flask import jsonify, request
from actions.secure_auth import Auth
from actions.diag_session import SessionManager
from configs.data_identifiers import *


def requires_auth(func):
    @wraps(func)
    def decorated_function(*args, **kwargs):

        if request.method == 'POST':
            is_manual_flow = request.get_json().get('is_manual_flow', None)
        elif request.method == 'GET':
            is_manual_flow = request.args.get('is_manual_flow', None)

        if is_manual_flow is None:
            return jsonify({"error": "Missing 'is_manual_flow' flag."}), 400

        is_manual_flow = is_manual_flow.lower() == 'true' if isinstance(is_manual_flow, str) else bool(is_manual_flow)

        if is_manual_flow:
            return func(*args, **kwargs)
        try:
            id = (API_ID << 8) + 0x10

            session = SessionManager(API_ID)
            session._change_session(id, 2)

            auth = Auth()
            auth_response = auth._auth_to()

            if "error" in auth_response:
                return jsonify({"error": "Authentication failed"}), 403

        except Exception as e:
            return jsonify({"error": f"An unexpected error occurred: {str(e)}"}), 500

        return func(*args, **kwargs)

    return decorated_function

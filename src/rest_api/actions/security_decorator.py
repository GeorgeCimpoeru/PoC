from functools import wraps  # noqa: E401
from flask import jsonify, request  # noqa: E401
from actions.secure_auth import Auth  # noqa: E401
from actions.diag_session import SessionManager  # noqa: F401
from configs.data_identifiers import *  # noqa: F401


def requires_auth(func):
    @wraps(func)
    def decorated_function(*args, **kwargs):

        if request.method == 'POST':
            is_manual_flow = request.get_json().get('is_manual_flow', None)
        elif request.method == 'GET':
            is_manual_flow = request.args.get('is_manual_flow', None)

        if is_manual_flow is None:
            return jsonify({"error": "The 'is_manual_flow' flag is required but was not provided in the request."}), 400
        elif isinstance(is_manual_flow, str) and is_manual_flow.strip() == '':
            return jsonify({"error": "The 'is_manual_flow' flag cannot be an empty string. Please provide the values 'true' or 'false'"}), 400

        if isinstance(is_manual_flow, str):
            is_manual_flow = is_manual_flow.lower()
            if is_manual_flow not in ['true', 'false']:
                return jsonify({"error": "The 'is_manual_flow' flag must be either 'true' or 'false'. You provided: '{}'".format(is_manual_flow)}), 400
        elif isinstance(is_manual_flow, bool):
            pass
        else:
            return jsonify({"error": "'The 'is_manual_flow' flag must be either 'true' or 'false'. You provided: '{}'".format(is_manual_flow)}), 400

        is_manual_flow = is_manual_flow.lower() == 'true' if isinstance(is_manual_flow, str) else bool(is_manual_flow)

        if is_manual_flow:
            return func(*args, **kwargs)
        try:
            session = SessionManager()
            session._change_session(2)

            auth = Auth()
            auth_response = auth._auth_to()

            if "error" in auth_response:
                return jsonify({"error": "Authentication failed"}), 403

        except Exception as e:
            return jsonify({"error": f"An unexpected error occurred: {str(e)}"}), 500

        return func(*args, **kwargs)

    return decorated_function

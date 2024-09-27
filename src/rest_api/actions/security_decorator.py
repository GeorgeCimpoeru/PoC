from functools import wraps
from flask import jsonify
from actions.secure_auth import Auth
from actions.diag_session import SessionManager
from configs.data_identifiers import *

def requires_auth(func):
    @wraps(func)
    def decorated_function(*args, **kwargs):
        try:
            id = (API_ID << 8) + 0x10 

            session = SessionManager(API_ID)
            response = session._change_session(id, 2)

            if response["status"] == "error":
                return jsonify({"error": "Session change failed", "message": response["message"]}), 500
            
            auth = Auth(API_ID, [0x10, 0x11, 0x12, 0x13, 0x14]) 
            auth_response = auth._auth_to()

            if "error" in auth_response:
                return jsonify({"error": "Authentication failed"}), 403
            
            return func(*args, **kwargs)

        except Exception as e:
            return jsonify({"error": f"An unexpected error occurred: {str(e)}"}), 500

    return decorated_function

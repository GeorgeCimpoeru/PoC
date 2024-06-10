from flask import Flask, request, jsonify, make_response
from service_registry import service_registry
from validate_http import method_required
from get_service import *
from post_service import *
from put_service import *


app = Flask(__name__)

@app.route('/api', methods=['GET', 'POST', 'PUT'])
@method_required
def api_route():
    """
    Route to handle API requests for different services. Supports GET, POST, and PUT methods.

    Returns:
        Response: JSON response containing the result of the service function or an error message.
    """
    service = request.args.get('service')
    
    if service in service_registry:
        service_info = service_registry[service]
        service_func = service_info['func']
        
        if request.method == 'GET':
            params = request.args.to_dict()
            result = service_func(params)
        elif request.method == 'POST':
            data = request.json
            result = service_func(data)
        elif request.method == 'PUT':
            data = request.json
            result = service_func(data)
            
        return jsonify(result)
    else:
        return jsonify({"error": "Service not found"}), 404

if __name__ == '__main__':
    app.run(debug=True)

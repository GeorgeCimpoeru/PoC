from flask import request, jsonify, make_response
from service_registry import service_registry

def method_required(func):
    """
    Decorator to check if the HTTP method used is within the allowed list for the service.

    Args:
        func (function): The API method to be wrapped by the decorator.

    Returns:
        function: The wrapped function that checks the HTTP method before execution.
    """
    def wrapper(*args, **kwargs):
        """
        Wrapper function that performs the HTTP method check.

        Args:
            *args: Variable length argument list.
            **kwargs: Arbitrary keyword arguments.

        Returns:
            Response: A Flask response object, either the result of the wrapped function
                      or an error response if the method is not allowed or the service is invalid.
        """
        service = request.args.get('service')
        
        if service in service_registry:
            allowed_methods = service_registry[service]['methods']
            
            if request.method not in allowed_methods:
                return make_response(jsonify({"error": "Method not allowed for this service"}), 405)
            
            return func(*args, **kwargs)
        else:
            return make_response(jsonify({"error": "Invalid service"}), 400)
    
    wrapper.__name__ = func.__name__
    return wrapper

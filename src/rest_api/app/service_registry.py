from collections import defaultdict

"""
Initialize a service registry as a defaultdict of dictionaries.
his will store the service functions and their allowed HTTP methods.
"""
service_registry = defaultdict(dict)

def register_service(service_name, methods):
    """
    Decorator to register a service with its allowed HTTP methods.

    Args:
        service_name (str): The name of the service to be registered.
        methods (list): A list of allowed HTTP methods for the service.

    Returns:
        function: The decorator function that registers the service.
    """
    def decorator(func):
        """
        The actual decorator function that registers the service.

        Args:
            func (function): The service function to be registered.

        Returns:
            function: The original service function, unchanged.
        """
        service_registry[service_name]['func'] = func
              
        service_registry[service_name]['methods'] = methods
        
        return func
      
    return decorator

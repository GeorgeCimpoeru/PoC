import os
import logging
from logging.handlers import RotatingFileHandler
from flask import request

def setup_logger():
    """
    Sets up the logger for the Flask application and returns a decorator for logging.

    Returns:
        function: A decorator function for logging requests and responses.
    """
    log_directory = os.path.join(os.path.dirname(__file__), 'log')
    os.makedirs(log_directory, exist_ok=True)
    log_file = os.path.join(log_directory, 'api.log')

    log_handler = RotatingFileHandler(log_file, maxBytes=10000, backupCount=3)
    log_handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    log_handler.setFormatter(formatter)

    def decorator(app):
        app.logger.addHandler(log_handler)
        app.logger.setLevel(logging.INFO)

        @app.before_request
        def log_request_info():
            app.logger.info(f'Request: {request.method} {request.url} - Body: {request.get_data()}')

        @app.after_request
        def log_response_info(response):
            app.logger.info(f'Response: {response.status} {response.get_data()}')
            return response
        
        return app

    return decorator

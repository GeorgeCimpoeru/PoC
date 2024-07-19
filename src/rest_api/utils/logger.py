import os
import logging
from logging.handlers import RotatingFileHandler
from flask import request
from functools import wraps


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


log_memory = []


class MemoryHandler(logging.Handler):
    def emit(self, record):
        log_entry = self.format(record)
        log_memory.append(log_entry)


class SingletonLogger:
    _instance = None
    _logger = None
    _logger_frame = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(SingletonLogger, cls).__new__(cls)
            cls._instance._initialize(*args, **kwargs)
        return cls._instance

    def _initialize(self, log_filename):
        log_directory = os.path.join(os.path.dirname(__file__), 'log')
        os.makedirs(log_directory, exist_ok=True)
        log_path = os.path.join(log_directory, log_filename)

        self._logger = logging.getLogger('custom_logger')
        self._logger.setLevel(logging.DEBUG)

        if not self._logger.hasHandlers():
            log_handler = RotatingFileHandler(log_path, maxBytes=10000, backupCount=3)
            log_handler.setLevel(logging.DEBUG)
            formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
            log_handler.setFormatter(formatter)
            self._logger.addHandler(log_handler)

            memory_handler = MemoryHandler()
            memory_handler.setLevel(logging.DEBUG)
            memory_handler.setFormatter(formatter)
            self._logger.addHandler(memory_handler)

            self._logger.propagate = False

        def logger_frame(cls):
            for attr_name in dir(cls):
                attr = getattr(cls, attr_name)
                if callable(attr) and not attr_name.startswith('__'):
                    setattr(cls, attr_name, log_method_calls(attr, self._logger))
            return cls

        self._logger_frame = logger_frame

    @property
    def logger(self):
        return self._logger

    @property
    def logger_frame(self):
        return self._logger_frame


def log_method_calls(method, logger):
    """
    Wrapper function to log method calls and arguments.

    Args:
        method (function): Method of the class to be wrapped.
        logger (logging.Logger): Logger instance to log messages.

    Returns:
        function: Wrapped function that logs method calls.
    """
    @wraps(method)
    def wrapper(self, *args, **kwargs):
        args_str = ', '.join([repr(a) for a in args])
        kwargs_str = ', '.join([f'{k}={repr(v)}' for k, v in kwargs.items()])
        arguments_str = ', '.join(filter(None, [args_str, kwargs_str]))
        log_message = f'Function "{method.__name__}" called with arguments: {arguments_str}'
        logger.info(log_message)
        return method(self, *args, **kwargs)

    return wrapper


def log_debug_message(logger, msg="Debug message"):
    logger.debug(msg)


def log_info_message(logger, msg="Info message"):
    logger.info(msg)


def log_warning_message(logger, msg="Warning message"):
    logger.warning(msg)


def log_error_message(logger, msg="Error message"):
    logger.error(msg)


def log_critical_message(logger, msg="Critical message"):
    logger.critical(msg)

from flask import Blueprint
from flasgger import swag_from
import os

api_bp = Blueprint('api', __name__)

YAML_FILE_PATH = os.path.abspath('utils/docs/api_doc.yaml')


def route_with_swagger(rule, **options):
    def decorator(f):
        endpoint_name = f.__name__
        f = api_bp.route(rule, **options)(f)
        f = swag_from(os.path.abspath('utils/docs/api_doc.yaml'), endpoint=endpoint_name)(f)
        return f
    return decorator

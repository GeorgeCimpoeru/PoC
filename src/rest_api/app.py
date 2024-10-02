from flask import Flask
from flasgger import Swagger
from config import *
from routes.main import main_bp
from flask_cors import CORS
from routes.api import api_bp


app = Flask(__name__)
app.config.from_object(Config)


swagger = Swagger(app, template_file=YAML_FILE_PATH)

app.register_blueprint(api_bp, url_prefix='/api')
app.register_blueprint(main_bp)
CORS(app)


if __name__ == '__main__':
    app.run(port=5001, debug=True)

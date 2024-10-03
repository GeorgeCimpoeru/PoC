from flask import Flask
from flasgger import Swagger
from config import *
from routes.main import main_bp
from flask_cors import CORS
from routes.api import api_bp
import multiprocessing
import subprocess

app = Flask(__name__)
app.config.from_object(Config)


swagger = Swagger(app, template_file=YAML_FILE_PATH)

app.register_blueprint(api_bp, url_prefix='/api')
app.register_blueprint(main_bp)
CORS(app)


# if __name__ == '__main__':
    # app.run(port=5001, debug=True)

def run_flask_app():
    app.run(port=5001, debug=True)

def run_can2udp():
    subprocess.run(['python3', 'can2udp2can.py'])

if __name__ == '__main__':
    flask_process = multiprocessing.Process(target=run_flask_app)
    can_process = multiprocessing.Process(target=run_can2udp)

    flask_process.start()
    can_process.start()

    flask_process.join()
    can_process.join()
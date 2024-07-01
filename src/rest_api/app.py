from flask import Flask
from config import Config
from routes.api import api_bp
from routes.main import main_bp
from can_interface import initialize_can_interface

app = Flask(__name__)
app.config.from_object(Config)

app.register_blueprint(api_bp, url_prefix='/api')
app.register_blueprint(main_bp)

# bus = None
bus = initialize_can_interface(app.config['CAN_INTERFACE'], True)

# def setup_can_interface():
    # global bus
    # bus = initialize_can_interface(app.config['CAN_INTERFACE'], True)
    
if __name__ == '__main__':
    # setup_can_interface()
    app.run(debug=True)

from flask import Flask
from config import Config
from routes.api import api_bp
from routes.main import main_bp

app = Flask(__name__)
app.config.from_object(Config)

app.register_blueprint(api_bp, url_prefix='/api')
app.register_blueprint(main_bp)

if __name__ == '__main__':
    app.run(debug=True)

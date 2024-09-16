# Flask CAN Interface Project

This project is a Flask-based web application to interact with a CAN bus. 
It provides a web interface to send and receive CAN frames, request IDs, and update firmware versions. 
The CAN interface is managed through the `python-can` library.

## Features

- Send custom CAN frames.
- Request IDs from the CAN interface.
- Update firmware versions on connected devices.
- Display JSON responses in the web interface.

## Prerequisites

- Python 3.8.10
- pip (Python package installer)
- A CAN interface (virtual or physical)
- [python-can](https://python-can.readthedocs.io/en/stable/)

## Installation

1. **Clone the repository:**
$ git clone ****
$ cd src/rest_api

2. **Create a virtual environment:**
$ python3 -m venv venv
$ source venv/bin/activate  # On Windows use `venv\Scripts\activate`

3. **Install the required packages:**
$ pip install -r requirements.txt

4. **Set up the CAN interface:**
Configure your CAN interface in the rest_api/config.py file. Default value of CAN_CHANNEL is "vcan0".

***Usage***
Initialize the CAN interface and start the Flask application:
$ python3 app.py
Open your web browser and go to:
http://127.0.0.1:5000
Interact with the web interface:

Enter CAN ID and CAN Data in HEX format (comma-separated for CAN Data and no spaces) and click "Send Frame".
Click "Request IDs" to request IDs from the CAN interface.
Click "Update to Version" to update firmware versions, entering the desired version when prompted.
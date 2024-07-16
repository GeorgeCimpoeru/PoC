import pytest
from src.rest_api.app import app
import json

@pytest.fixture
def client():
    app.config['TESTING'] = True
    with app.test_client() as client:
        yield client


def test_request_ids(client):
    response = client.post('/api/request_ids')
    assert response.status_code == 200
    assert isinstance(response.json, dict)  


def test_update_to_version(client):
    response = client.post('/api/update_to_version', json={'ecu_id': '1', 'version': '1234'})   
    assert response.headers['Content-Type'] == 'application/json', f"Expected 'application/json' content type, but got {response.headers['Content-Type']}"
    response_json = json.loads(response.data.decode('utf-8'))
    assert response.status_code == 200, f"Expected status code 200, but got {response.status_code}"


def test_read_info_bat(client):
    response = client.get('/api/read_info_battery')
    assert response.status_code == 200
    assert isinstance(response.json, dict)  


def test_send_frame(client):
    response = client.post('/api/send_frame', json={'can_id': '0x12', 'can_data': '0x11'})
    assert response.headers['Content-Type'] == 'application/json', f"Expected 'application/json' content type, but got {response.headers['Content-Type']}"
    response_json = json.loads(response.data.decode('utf-8'))
    assert response.status_code == 200 or response.status_code == 400 or response.status_code == 500
    assert isinstance(response.json, dict)  


def test_get_logs(client):
    response = client.get('/api/logs')
    assert response.status_code == 200
    assert 'logs' in response.json
    assert isinstance(response.json['logs'], list) 


def test_read_info_engine(client):
    response = client.get('/api/read_info_engine')  
    assert response.status_code == 200
    assert isinstance(response.json, dict) 


def test_read_info_doors(client):
    response = client.get('/api/read_info_doors')  
    assert response.status_code == 200
    assert isinstance(response.json, dict) 
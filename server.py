import socket
import threading
import logging
import json
import os
import time
import random
from datetime import datetime

# Configure logging
if not os.path.exists('logs'):
    os.makedirs('logs')

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('logs/zerofn_server.log')
    ]
)
logger = logging.getLogger('FortniteServer')

class FortniteAuthServer:
    def __init__(self, host='0.0.0.0', port=7777):
        self.host = host
        self.port = port
        self.running = True
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.auth_tokens = {}

    def start(self):
        try:
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5)
            logger.info(f'Server listening on {self.host}:{self.port}')

            while self.running:
                try:
                    client_socket, address = self.server_socket.accept()
                    logger.info(f'New connection from {address}')
                    client_thread = threading.Thread(target=self.handle_client, args=(client_socket, address))
                    client_thread.daemon = True
                    client_thread.start()
                except Exception as e:
                    logger.error(f'Error accepting connection: {e}')

        except Exception as e:
            logger.error(f'Server error: {e}')
            self.shutdown()

    def handle_client(self, client_socket, address):
        try:
            while self.running:
                data = client_socket.recv(4096)
                if not data:
                    break

                request = data.decode('utf-8')
                logger.debug(f'Received request from {address}: {request[:100]}...')

                # Parse the HTTP request
                request_lines = request.split('\r\n')
                request_line = request_lines[0]
                method, path, _ = request_line.split(' ')

                # Prepare HTTP response headers
                headers = [
                    'HTTP/1.1 200 OK',
                    'Content-Type: application/json',
                    'Access-Control-Allow-Origin: *',
                    'Access-Control-Allow-Methods: GET, POST, OPTIONS',
                    'Access-Control-Allow-Headers: Content-Type, Authorization',
                    'Connection: keep-alive'
                ]

                if '/account/api/oauth/token' in path:
                    response_body = self.generate_auth_token_response()
                elif '/fortnite/api/game/v2/profile' in path:
                    response_body = self.generate_profile_response()
                elif '/fortnite/api/cloudstorage/system' in path:
                    response_body = json.dumps([])
                elif '/fortnite/api/game/v2/enabled_features' in path:
                    response_body = json.dumps([])
                elif '/lightswitch/api/service/bulk/status' in path:
                    response_body = json.dumps([{
                        "serviceInstanceId": "fortnite",
                        "status": "UP",
                        "message": "Fortnite is online",
                        "maintenanceUri": None,
                        "allowedActions": ["PLAY", "DOWNLOAD"],
                        "banned": False,
                        "launcherInfoDTO": {
                            "appName": "Fortnite",
                            "catalogItemId": "4fe75bbc5a674f4f9b356b5c90567da5",
                            "namespace": "fn"
                        }
                    }])
                elif '/account/api/public/account' in path:
                    response_body = json.dumps({
                        "id": "ZeroFN",
                        "displayName": "ZeroFN",
                        "externalAuths": {}
                    })
                elif '/waitingroom/api/waitingroom' in path:
                    response_body = json.dumps({
                        "type": "ACTIVE", 
                        "timeToWait": 0,
                        "expectedWait": 0,
                        "status": "ACTIVE",
                        "message": "No wait time"
                    })
                else:
                    response_body = json.dumps({'status': 'ok'})

                headers.append(f'Content-Length: {len(response_body)}')
                response = '\r\n'.join(headers) + '\r\n\r\n' + response_body
                
                client_socket.send(response.encode())
                logger.debug(f'Sent response to {address}')

        except Exception as e:
            logger.error(f'Error handling client {address}: {e}')
        finally:
            client_socket.close()
            logger.info(f'Client {address} disconnected')

    def generate_auth_token(self):
        token = f'eg1~ZeroFN_{random.randint(1000000, 9999999)}'
        self.auth_tokens[token] = time.time()
        return token

    def generate_auth_token_response(self):
        token = self.generate_auth_token()
        return json.dumps({
            'access_token': token,
            'expires_in': 28800,
            'expires_at': '9999-12-31T23:59:59.999Z',
            'token_type': 'bearer',
            'refresh_token': token,
            'refresh_expires': 28800,
            'refresh_expires_at': '9999-12-31T23:59:59.999Z',
            'account_id': 'ZeroFN',
            'client_id': 'ec684b8c687f479fadea3cb2ad83f5c6',
            'internal_client': True,
            'client_service': 'fortnite',
            'scope': 'basic_profile friends_list openid presence',
            'displayName': 'ZeroFN',
            'app': 'fortnite',
            'in_app_id': 'ZeroFN',
            'device_id': 'ZeroFN'
        })

    def generate_profile_response(self):
        return json.dumps({
            'profileRevision': 1,
            'profileId': 'athena',
            'profileChanges': [],
            'baseRevision': 1,
            'serverTime': datetime.now().isoformat(),
            'responseVersion': 1,
            'profileCommandRevision': 1,
            'status': 'ok',
            'commandRevision': 1
        })

    def shutdown(self):
        logger.info('Shutting down server...')
        self.running = False
        try:
            self.server_socket.close()
        except Exception:
            pass
        logger.info('Server shutdown complete')


if __name__ == '__main__':
    print('Starting ZeroFN Server...')
    print('Server logs will appear below:')
    print('-' * 50)
    server = FortniteAuthServer()
    try:
        server.start()
    except KeyboardInterrupt:
        print('\nShutting down server (Ctrl+C pressed)')
        server.shutdown()

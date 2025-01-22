import socket
import threading
import logging
import json
import os
import time
import random
from datetime import datetime

# Configure logging
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

                if '/account/api/oauth/token' in request:
                    response = self.generate_auth_token_response()
                elif '/fortnite/api/game/v2/profile' in request:
                    response = self.generate_profile_response()
                else:
                    response = json.dumps({'status': 'ok'})

                client_socket.send(response.encode())
                logger.debug(f'Sent response to {address}')
        except Exception as e:
            logger.error(f'Error handling client {address}: {e}')
        finally:
            client_socket.close()
            logger.info(f'Client {address} disconnected')

    def generate_auth_token(self):
        token = f'zerofn_{random.randint(1000000, 9999999)}'
        self.auth_tokens[token] = time.time()
        return token

    def generate_auth_token_response(self):
        token = self.generate_auth_token()
        return json.dumps({
            'access_token': token,
            'expires_in': 28800,
            'token_type': 'bearer',
            'client_id': 'ZeroFN',
            'internal_client': True,
            'client_service': 'fortnite'
        })

    def generate_profile_response(self):
        return json.dumps({
            'profileId': 'default',
            'profileChanges': [],
            'serverTime': datetime.now().isoformat(),
            'profileCommandRevision': 1,
            'status': 'ok'
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

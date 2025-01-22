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
        self.cosmetics = self.load_cosmetics()
        self.active_sessions = {}
        self.matchmaking_queue = []
        self.match_instances = {}

    def load_cosmetics(self):
        # Enhanced cosmetics list
        return {
            "characters": [
                "CID_001_Athena_Commando_F_Default",
                "CID_002_Athena_Commando_F_Default",
                "CID_003_Athena_Commando_F_Default",
                "CID_004_Athena_Commando_F_Default",
                "CID_005_Athena_Commando_M_Default",
                "CID_006_Athena_Commando_M_Default",
                "CID_007_Athena_Commando_M_Default",
                "CID_008_Athena_Commando_M_Default",
                # Add more character IDs here
            ],
            "backpacks": [
                "BID_001_BlackKnight",
                "BID_002_RustLord",
                "BID_003_SpaceExplorer",
                # Add more backpack IDs here 
            ],
            "pickaxes": [
                "Pickaxe_Lockjaw",
                "Pickaxe_Default",
                # Add more pickaxe IDs here
            ],
            "gliders": [
                "Glider_Default",
                "Glider_Founder",
                # Add more glider IDs here
            ],
            "emotes": [
                "EID_DanceMoves",
                "EID_Floss", 
                "EID_Fresh",
                "EID_RideThePony",
                # Add more emote IDs here
            ]
        }

    def start(self):
        try:
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5)
            logger.info(f'Server listening on {self.host}:{self.port}')

            # Start matchmaking thread
            matchmaking_thread = threading.Thread(target=self.matchmaking_loop)
            matchmaking_thread.daemon = True
            matchmaking_thread.start()

            # Start session cleanup thread
            cleanup_thread = threading.Thread(target=self.cleanup_loop)
            cleanup_thread.daemon = True 
            cleanup_thread.start()

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

                # Extract auth token from headers if present
                auth_token = None
                for line in request_lines:
                    if line.startswith('Authorization: bearer '):
                        auth_token = line.split(' ')[2]

                # Prepare HTTP response headers
                headers = [
                    'HTTP/1.1 200 OK',
                    'Content-Type: application/json',
                    'Access-Control-Allow-Origin: *',
                    'Access-Control-Allow-Methods: GET, POST, OPTIONS',
                    'Access-Control-Allow-Headers: Content-Type, Authorization',
                    'Connection: keep-alive'
                ]

                # Enhanced endpoint handling with session management
                if '/account/api/oauth/token' in path:
                    response_body = self.generate_auth_token_response()
                elif '/fortnite/api/game/v2/profile' in path:
                    response_body = self.generate_profile_response(auth_token)
                elif '/fortnite/api/game/v2/matchmaking/account' in path:
                    response_body = self.handle_matchmaking_request(auth_token, request)
                elif '/fortnite/api/game/v2/chat' in path:
                    response_body = self.handle_chat_request(auth_token, request)
                elif '/fortnite/api/cloudstorage/system' in path:
                    response_body = self.get_cloud_storage()
                elif '/fortnite/api/game/v2/enabled_features' in path:
                    response_body = json.dumps([""])
                elif '/lightswitch/api/service/bulk/status' in path:
                    response_body = self.get_service_status()
                elif '/account/api/public/account' in path:
                    response_body = self.get_account_info(auth_token)
                elif '/waitingroom/api/waitingroom' in path:
                    response_body = self.get_waiting_room_status()
                elif '/fortnite/api/matchmaking/session/findPlayer' in path:
                    response_body = self.find_match_session(auth_token)
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
            if auth_token in self.active_sessions:
                del self.active_sessions[auth_token]

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
            'displayName': 'ZeroFN Player',
            'app': 'fortnite',
            'in_app_id': 'ZeroFN',
            'device_id': 'ZeroFN'
        })

    def generate_profile_response(self, auth_token):
        # Enhanced profile with more stats and items
        return json.dumps({
            'profileRevision': 1,
            'profileId': 'athena',
            'profileChanges': [{
                'changeType': 'fullProfileUpdate',
                'profile': {
                    '_id': 'ZeroFN',
                    'Update': datetime.now().isoformat(),
                    'created': datetime.now().isoformat(),
                    'updated': datetime.now().isoformat(),
                    'rvn': 1,
                    'wipeNumber': 1,
                    'accountId': 'ZeroFN',
                    'profileId': 'athena',
                    'version': 'no_version',
                    'items': {
                        'sandbox_loadout': {
                            'templateId': 'CosmeticLocker:cosmeticlocker_athena',
                            'attributes': {
                                'locker_slots_data': {
                                    'slots': {
                                        'Character': {'items': self.cosmetics['characters']},
                                        'Backpack': {'items': self.cosmetics['backpacks']},
                                        'Pickaxe': {'items': self.cosmetics['pickaxes']},
                                        'Glider': {'items': self.cosmetics['gliders']},
                                        'Dance': {'items': self.cosmetics['emotes']}
                                    }
                                }
                            }
                        }
                    },
                    'stats': {
                        'attributes': {
                            'season_num': 1,
                            'lifetime_wins': 999,
                            'book_level': 100,
                            'book_xp': 999999,
                            'season_level': 100,
                            'season_xp': 999999,
                            'battlestars': 999,
                            'matchmaking_region': 'NAE'
                        }
                    }
                }
            }],
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

    def cleanup_loop(self):
        while self.running:
            # Cleanup expired sessions and tokens
            current_time = time.time()
            expired_tokens = [token for token, timestamp in self.auth_tokens.items() 
                            if current_time - timestamp > 28800]
            for token in expired_tokens:
                del self.auth_tokens[token]
                if token in self.active_sessions:
                    del self.active_sessions[token]
            time.sleep(60)

    def matchmaking_loop(self):
        while self.running:
            if len(self.matchmaking_queue) >= 2:
                # Match players and create game session
                player1 = self.matchmaking_queue.pop(0)
                player2 = self.matchmaking_queue.pop(0)
                match_id = f'match_{random.randint(1000, 9999)}'
                self.match_instances[match_id] = {
                    'players': [player1, player2],
                    'start_time': time.time(),
                    'state': 'starting'
                }
            time.sleep(1)


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

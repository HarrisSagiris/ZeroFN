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
    def __init__(self, host='0.0.0.1', port=7777):  # Changed to listen on 0.0.0.1:7777
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
        self.last_update = time.time()
        self.update_interval = 1.0  # Update every second

    def load_cosmetics(self):
        # Enhanced cosmetics list with popular Fortnite items
        return {
            "characters": [
                "CID_001_Athena_Commando_F_Default",
                "CID_286_Athena_Commando_F_NeonCat", # Lynx
                "CID_313_Athena_Commando_M_KpopFashion", # IKONIK
                "CID_434_Athena_Commando_F_StealthHonor", # Renegade Raider
                "CID_028_Athena_Commando_F", # Brite Bomber
                "CID_081_Athena_Commando_M_BlueDragon", # Blue Team Leader
                "CID_175_Athena_Commando_M_Celestial", # Galaxy
                "CID_342_Athena_Commando_M_StreetRacer", # Travis Scott
            ],
            "backpacks": [
                "BID_004_BlackKnight",
                "BID_023_GhoulTrooper", 
                "BID_095_GalaxyStar",
                "BID_122_RustLord",
                "BID_142_SpaceExplorer",
            ],
            "pickaxes": [
                "Pickaxe_ID_012_District", # Raider's Revenge
                "Pickaxe_ID_013_Teslacoil", # AC/DC
                "Pickaxe_ID_015_Holiday", # Candy Axe
                "Pickaxe_ID_029_WinterCamo", # Cold Steel
            ],
            "gliders": [
                "Glider_ID_001_Default",
                "Glider_ID_002_Victory", 
                "Glider_ID_003_Founder",
                "Glider_ID_008_StormSail",
            ],
            "emotes": [
                "EID_DanceMoves",
                "EID_Floss",
                "EID_TakeTheL", 
                "EID_Fresh",
                "EID_RideThePony",
                "EID_Dab",
                "EID_WormDance",
                "EID_RobotDance"
            ]
        }

    def start(self):
        try:
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5)  # Allow multiple connections
            logger.info(f'ZeroFN Server listening on {self.host}:{self.port}')
            logger.info('Waiting for Fortnite client connections...')

            # Start update thread
            update_thread = threading.Thread(target=self.update_loop)
            update_thread.daemon = True
            update_thread.start()

            while self.running:
                try:
                    client_socket, address = self.server_socket.accept()
                    logger.info(f'Fortnite client connected from {address}')
                    
                    # Handle each client in a separate thread
                    client_thread = threading.Thread(
                        target=self.handle_client,
                        args=(client_socket, address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except Exception as e:
                    logger.error(f'Connection error: {e}')
                    continue

        except Exception as e:
            logger.error(f'Server error: {e}')
            self.shutdown()

    def update_loop(self):
        while self.running:
            current_time = time.time()
            if current_time - self.last_update >= self.update_interval:
                self.last_update = current_time
                self.update_game_state()
            time.sleep(0.1)  # Small sleep to prevent CPU overuse

    def update_game_state(self):
        # Update active sessions
        for session_id in list(self.active_sessions.keys()):
            session = self.active_sessions[session_id]
            session['last_update'] = datetime.now().isoformat()

        # Update matchmaking queue
        if len(self.matchmaking_queue) >= 2:
            match_id = f'match_{random.randint(1000, 9999)}'
            players = self.matchmaking_queue[:2]
            self.matchmaking_queue = self.matchmaking_queue[2:]
            self.match_instances[match_id] = {
                'players': players,
                'start_time': datetime.now().isoformat(),
                'status': 'in_progress'
            }

    def handle_client(self, client_socket, address):
        try:
            client_socket.settimeout(1)
            session_id = f'session_{random.randint(1000, 9999)}'
            self.active_sessions[session_id] = {
                'address': address,
                'connected_time': datetime.now().isoformat(),
                'last_update': datetime.now().isoformat()
            }
            
            while self.running:
                try:
                    data = client_socket.recv(4096)
                    if not data:
                        break

                    request = data.decode('utf-8')
                    logger.info(f'Received request from client: {request[:100]}...')

                    request_lines = request.split('\r\n')
                    request_line = request_lines[0]
                    method, path, _ = request_line.split(' ')

                    response_body = ""
                    if "/account/api/oauth/token" in path:
                        response_body = self.generate_auth_token_response()
                    elif "/fortnite/api/game/v2/profile" in path:
                        response_body = self.generate_profile_response()
                    elif "/content/api/pages/fortnite-game" in path:
                        response_body = self.generate_game_info()
                    elif "/lightswitch/api/service/bulk/status" in path:
                        response_body = self.generate_lightswitch_response()
                    elif "/account/api/public/account" in path:
                        response_body = self.generate_account_response()
                    else:
                        response_body = self.generate_auth_token_response()

                    headers = [
                        'HTTP/1.1 200 OK',
                        'Content-Type: application/json',
                        'Access-Control-Allow-Origin: *',
                        'Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE',
                        'Access-Control-Allow-Headers: Accept, Content-Type, Authorization',
                        'Connection: keep-alive',
                        f'Content-Length: {len(response_body)}'
                    ]

                    response = '\r\n'.join(headers) + '\r\n\r\n' + response_body
                    client_socket.send(response.encode())
                    logger.info(f'Sent response to client')

                except socket.timeout:
                    continue
                except Exception as e:
                    logger.error(f'Error handling request: {e}')
                    break

        except Exception as e:
            logger.error(f'Client handler error: {e}')
        finally:
            if session_id in self.active_sessions:
                del self.active_sessions[session_id]
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
            'account_id': 'ZeroFN_Player',
            'client_id': 'ec684b8c687f479fadea3cb2ad83f5c6',
            'internal_client': True,
            'client_service': 'fortnite',
            'scope': 'basic_profile friends_list openid presence',
            'displayName': 'ZeroFN Player',
            'app': 'fortnite',
            'in_app_id': 'ZeroFN_Player',
            'device_id': 'ZeroFN_Device'
        })

    def generate_profile_response(self):
        return json.dumps({
            "profileRevision": 1,
            "profileId": "athena",
            "profileChangesBaseRevision": 1,
            "profileChanges": [{
                "_type": "fullProfileUpdate",
                "profile": {
                    "_id": "ZeroFN_Player",
                    "Update": "Profile Updated",
                    "Created": "Profile Created",
                    "Updated": datetime.now().isoformat()
                }
            }],
            "serverTime": datetime.now().isoformat(),
            "responseVersion": 1
        })

    def generate_game_info(self):
        return json.dumps({
            "jcr:isCheckedOut": True,
            "jcr:baseVersion": "a7ca237317f1e7883b3279-c38f-4aa7-a325-e099e4bf71e5",
            "_title": "Fortnite Game",
            "lastModified": datetime.now().isoformat(),
            "_activeDate": datetime.now().isoformat(),
            "_locale": "en-US",
            "battleroyalenews": {
                "news": {
                    "motds": [{
                        "entryType": "Website",
                        "image": "https://i.imgur.com/example.jpg",
                        "tileImage": "https://i.imgur.com/example.jpg",
                        "title": "Welcome to ZeroFN",
                        "body": "Enjoy your game!",
                    }]
                }
            }
        })

    def generate_lightswitch_response(self):
        return json.dumps([{
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

    def generate_account_response(self):
        return json.dumps({
            "id": "ZeroFN_Player",
            "displayName": "ZeroFN Player",
            "externalAuths": {}
        })

    def shutdown(self):
        logger.info('Shutting down ZeroFN server...')
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

import socket
import threading
import json
import time
import requests
import base64
from http.server import HTTPServer, BaseHTTPRequestHandler
from datetime import datetime
from pathlib import Path

class FortniteServer:
    def __init__(self):
        self.host = '127.0.0.1'
        self.port = 7777
        self.client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl" # Custom registered client ID
        self.client_secret = "Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA" # Official Epic client secret
        self.auth_tokens = {}
        self.lobbies = {}
        self.matchmaking_queue = []
        
        # Initialize HTTP server for Epic auth callback
        self.http_server = HTTPServer((self.host, self.port), self.create_callback_handler())
        
        # Initialize TCP server for game connections
        self.game_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
        self.game_server.bind((self.host, self.port+1))
        self.game_server.listen(5)
        
        print(f'[INFO] HTTP Server listening on {self.host}:{self.port}')
        print(f'[INFO] Game Server listening on {self.host}:{self.port+1}')

    def create_callback_handler(self):
        outer_instance = self
        
        class CallbackHandler(BaseHTTPRequestHandler):
            def do_GET(self):
                if self.path.startswith('/epic/auth'):
                    auth_code = self.path.split('code=')[1]
                    outer_instance.handle_epic_auth(auth_code)
                    
                    self.send_response(200)
                    self.send_header('Content-Type', 'text/html')
                    self.end_headers()
                    success_html = """
                    <html>
                        <head>
                            <style>
                                body { 
                                    background: #0b0d17;
                                    color: white;
                                    font-family: 'Segoe UI', sans-serif;
                                    display: flex;
                                    justify-content: center;
                                    align-items: center;
                                    height: 100vh;
                                    margin: 0;
                                }
                                .success-card {
                                    background: rgba(21, 24, 35, 0.95);
                                    padding: 2rem;
                                    border-radius: 15px;
                                    text-align: center;
                                    box-shadow: 0 8px 32px rgba(0,0,0,0.3);
                                }
                                .success-icon {
                                    color: #00c853;
                                    font-size: 48px;
                                }
                            </style>
                        </head>
                        <body>
                            <div class="success-card">
                                <h2>✓ Login Successful!</h2>
                                <p>You can now close this window and return to the launcher.</p>
                            </div>
                        </body>
                    </html>
                    """
                    self.wfile.write(success_html.encode())

    def handle_epic_auth(self, auth_code):
        auth_str = f"{self.client_id}:{self.client_secret}"
        auth_bytes = auth_str.encode('ascii')
        auth_b64 = base64.b64encode(auth_bytes).decode('ascii')
        
        headers = {
            'Authorization': f'Basic {auth_b64}',
            'Content-Type': 'application/x-www-form-urlencoded'
        }
        
        data = {
            'grant_type': 'authorization_code',
            'code': auth_code,
            'token_type': 'eg1'
        }
        
        response = requests.post(
            'https://account-public-service-prod.ol.epicgames.com/account/api/oauth/token',
            headers=headers,
            data=data
        )
        
        if response.status_code == 200:
            self.auth_tokens[auth_code] = response.json()
            print(f'[INFO] Successfully authenticated user')
            print(f'[INFO] Access token: {self.auth_tokens[auth_code]["access_token"]}')
        else:
            print(f'[ERROR] Auth failed: {response.text}')

    def handle_game_client(self, client, addr):
        player_id = str(addr[1])
        
        while True:
            try:
                data = client.recv(1024).decode()
                if not data:
                    break
                    
                request = json.loads(data)
                response = self.handle_game_request(request, player_id)
                client.send(json.dumps(response).encode())
                
            except Exception as e:
                print(f'[ERROR] Client error: {e}')
                break
                
        if player_id in self.matchmaking_queue:
            self.matchmaking_queue.remove(player_id)
        client.close()

    def handle_game_request(self, request, player_id):
        req_type = request.get('type')
        
        if req_type == 'login':
            return {
                'status': 'success',
                'accountId': player_id,
                'displayName': f'Player_{player_id}',
                'inventory': self.get_default_inventory()
            }
            
        elif req_type == 'loadout':
            return {
                'status': 'success',
                'skins': self.get_available_skins(),
                'emotes': self.get_available_emotes()
            }
            
        elif req_type == 'matchmaking':
            self.matchmaking_queue.append(player_id)
            if len(self.matchmaking_queue) >= 2:
                lobby_id = f'lobby_{int(time.time())}'
                players = self.matchmaking_queue[:2]
                self.lobbies[lobby_id] = players
                self.matchmaking_queue = self.matchmaking_queue[2:]
                return {
                    'status': 'match_found',
                    'lobby_id': lobby_id,
                    'players': players
                }
            return {
                'status': 'queued'
            }
            
        return {'status': 'error', 'message': 'Invalid request type'}

    def get_default_inventory(self):
        return {
            'vbucks': 13500,
            'battle_pass': True,
            'level': 100
        }

    def get_available_skins(self):
        return [
            {'id': 'CID_001', 'name': 'Renegade Raider'},
            {'id': 'CID_002', 'name': 'Skull Trooper'},
            {'id': 'CID_003', 'name': 'Ghoul Trooper'},
            {'id': 'CID_004', 'name': 'Black Knight'}
        ]

    def get_available_emotes(self):
        return [
            {'id': 'EID_001', 'name': 'Floss'},
            {'id': 'EID_002', 'name': 'Take The L'},
            {'id': 'EID_003', 'name': 'Orange Justice'}
        ]

    def start(self):
        # Start HTTP server thread for auth callbacks
        http_thread = threading.Thread(target=self.http_server.serve_forever)
        http_thread.daemon = True
        http_thread.start()
        
        # Accept game clients
        while True:
            client, addr = self.game_server.accept()
            print(f'[INFO] Game client connected from {addr[0]}:{addr[1]}')
            client_thread = threading.Thread(target=self.handle_game_client, args=(client, addr))
            client_thread.daemon = True
            client_thread.start()

if __name__ == '__main__':
    server = FortniteServer()
    server.start()

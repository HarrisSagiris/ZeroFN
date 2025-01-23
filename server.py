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
        self.online_players = {}  # Track online players
        self.friend_lists = {}    # Track friend lists
        self.party_invites = {}   # Track party invites
        self.player_seasons = {}  # Track which season each player is on
        self.party_chat = {}      # Party chat history
        
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
        self.online_players[player_id] = {
            'addr': addr,
            'client': client,
            'party_id': None,
            'status': 'online'
        }
        
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
        if player_id in self.online_players:
            del self.online_players[player_id]
        client.close()

    def handle_game_request(self, request, player_id):
        req_type = request.get('type')
        
        if req_type == 'login':
            season = request.get('season', '1')  # Get player's season
            self.player_seasons[player_id] = season
            return {
                'status': 'success',
                'accountId': player_id,
                'displayName': f'Player_{player_id}',
                'inventory': self.get_default_inventory(),
                'friends': self.friend_lists.get(player_id, [])
            }
            
        elif req_type == 'add_friend':
            friend_id = request.get('friend_id')
            if friend_id in self.online_players:
                if player_id not in self.friend_lists:
                    self.friend_lists[player_id] = []
                self.friend_lists[player_id].append(friend_id)
                return {'status': 'success', 'message': 'Friend added!'}
            return {'status': 'error', 'message': 'Player not found'}
            
        elif req_type == 'invite_to_party':
            friend_id = request.get('friend_id')
            if friend_id in self.online_players:
                if self.player_seasons[player_id] == self.player_seasons[friend_id]:
                    self.party_invites[friend_id] = player_id
                    return {'status': 'success', 'message': 'Invite sent!'}
                return {'status': 'error', 'message': 'Players must be in same season'}
            return {'status': 'error', 'message': 'Player not online'}
            
        elif req_type == 'accept_invite':
            if player_id in self.party_invites:
                party_leader = self.party_invites[player_id]
                party_id = f'party_{int(time.time())}'
                self.online_players[player_id]['party_id'] = party_id
                self.online_players[party_leader]['party_id'] = party_id
                self.party_chat[party_id] = []
                return {'status': 'success', 'party_id': party_id}
            return {'status': 'error', 'message': 'No pending invites'}
            
        elif req_type == 'party_chat':
            message = request.get('message')
            party_id = self.online_players[player_id]['party_id']
            if party_id:
                self.party_chat[party_id].append({
                    'player': player_id,
                    'message': message,
                    'timestamp': time.time()
                })
                return {'status': 'success'}
            return {'status': 'error', 'message': 'Not in a party'}
            
        elif req_type == 'matchmaking':
            season = self.player_seasons[player_id]
            party_id = self.online_players[player_id]['party_id']
            
            # Add whole party to queue if in party
            if party_id:
                party_members = [pid for pid, data in self.online_players.items() 
                               if data['party_id'] == party_id]
                for member in party_members:
                    if member not in self.matchmaking_queue:
                        self.matchmaking_queue.append(member)
            else:
                self.matchmaking_queue.append(player_id)
                
            # Try to match players in same season
            same_season_players = [p for p in self.matchmaking_queue 
                                 if self.player_seasons[p] == season]
            
            if len(same_season_players) >= 2:
                lobby_id = f'lobby_{int(time.time())}'
                players = same_season_players[:2]
                self.lobbies[lobby_id] = {
                    'players': players,
                    'season': season,
                    'status': 'starting'
                }
                for p in players:
                    self.matchmaking_queue.remove(p)
                return {
                    'status': 'match_found',
                    'lobby_id': lobby_id,
                    'players': players,
                    'season': season
                }
            return {
                'status': 'queued',
                'position': len(self.matchmaking_queue)
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
            # Season 1 Skins
            {'id': 'CID_001', 'name': 'Renegade Raider', 'rarity': 'Rare'},
            {'id': 'CID_002', 'name': 'Aerial Assault Trooper', 'rarity': 'Rare'},
            # Season 2 Skins
            {'id': 'CID_003', 'name': 'Black Knight', 'rarity': 'Legendary'},
            {'id': 'CID_004', 'name': 'Blue Squire', 'rarity': 'Rare'},
            {'id': 'CID_005', 'name': 'Royale Knight', 'rarity': 'Rare'},
            {'id': 'CID_006', 'name': 'Sparkle Specialist', 'rarity': 'Epic'},
            # Holiday Skins
            {'id': 'CID_007', 'name': 'Skull Trooper', 'rarity': 'Epic'},
            {'id': 'CID_008', 'name': 'Ghoul Trooper', 'rarity': 'Epic'},
            {'id': 'CID_009', 'name': 'Crackshot', 'rarity': 'Legendary'},
            {'id': 'CID_010', 'name': 'Red-Nosed Raider', 'rarity': 'Rare'}
        ]

    def get_available_pickaxes(self):
        return [
            # Season 1 Pickaxes
            {'id': 'PID_001', 'name': 'Raiders Revenge', 'rarity': 'Epic'},
            {'id': 'PID_002', 'name': 'AC/DC', 'rarity': 'Rare'},
            # Season 2 Pickaxes  
            {'id': 'PID_003', 'name': 'Axecalibur', 'rarity': 'Rare'},
            {'id': 'PID_004', 'name': 'Pulse Axe', 'rarity': 'Rare'},
            # Holiday Pickaxes
            {'id': 'PID_005', 'name': 'Reaper Scythe', 'rarity': 'Epic'},
            {'id': 'PID_006', 'name': 'Candy Axe', 'rarity': 'Epic'}
        ]

    def get_available_gliders(self):
        return [
            # Season 1 Gliders
            {'id': 'GID_001', 'name': 'Aerial Assault One', 'rarity': 'Rare'},
            {'id': 'GID_002', 'name': 'Mako', 'rarity': 'Rare'},
            # Season 2 Gliders
            {'id': 'GID_003', 'name': 'Sir Glider the Brave', 'rarity': 'Rare'},
            {'id': 'GID_004', 'name': 'Winter Wing', 'rarity': 'Rare'},
            # Holiday Gliders
            {'id': 'GID_005', 'name': 'Snowflake', 'rarity': 'Epic'},
            {'id': 'GID_006', 'name': 'Cozy Coaster', 'rarity': 'Epic'}
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

import socket
import threading
import json
import time
import requests
import base64
import logging
import random
import webbrowser
from http.server import HTTPServer, BaseHTTPRequestHandler
from datetime import datetime, timezone, timedelta
from pathlib import Path
import subprocess
import os
import jwt
from urllib.parse import unquote, parse_qs, urlparse
from auth import AuthHandler, generate_guest_credentials

print("Starting ZeroFN Server (Chapter 1 Season 2)...")
print("Initializing components...")

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s',
    handlers=[
        logging.FileHandler('server.log'),
        logging.StreamHandler()
    ]
)

print("Logging system initialized...")

# Start mitmproxy in a new window
def start_proxy():
    try:
        proxy_path = os.path.join(os.path.dirname(__file__), 'proxy.py')
        if os.name == 'nt':  # Windows
            subprocess.Popen(['start', 'cmd', '/k', 'mitmdump', '-s', proxy_path], shell=True)
        else:  # Unix/Linux
            subprocess.Popen(['x-terminal-emulator', '-e', f'mitmdump -s {proxy_path}'])
        print("Started mitmproxy with custom script")
        time.sleep(2)  # Wait for proxy to initialize
    except Exception as e:
        print(f"Failed to start proxy: {str(e)}")
        raise

class FortniteServer:
    def __init__(self):
        print("Setting up server configuration...")
        self.logger = logging.getLogger('FortniteServer')
        self.host = '127.0.0.1'  # Listen on localhost
        self.port = 7778
        
        # Start proxy server
        start_proxy()
        
        # Updated Epic Games OAuth credentials
        self.client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
        self.client_secret = "Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA"
        
        # Store expected state parameter
        self.expected_state = None
        
        # Check for auth token
        try:
            with open('auth_token.json', 'r') as f:
                self.auth_token = json.load(f)
                print("Found existing auth token")
                print(f"Logged in as: {self.auth_token.get('displayName', 'Unknown')} {os.environ.get('LOGGED_IN', '')}")
                
                # Validate and refresh token if needed
                if self.should_refresh_token():
                    self.refresh_auth_token()
                    
        except FileNotFoundError:
            print("No auth token found. Please log in first!")
            print("Starting auth server...")
            
            # Create guest credentials if needed
            username, email, password, account_id = generate_guest_credentials()
            
            # Start auth server
            auth_server = HTTPServer(('127.0.0.1', 7777), AuthHandler)
            auth_thread = threading.Thread(target=auth_server.serve_forever)
            auth_thread.daemon = True
            auth_thread.start()
            
            # Wait for auth token to be created
            while not os.path.exists('auth_token.json'):
                time.sleep(1)
                
            with open('auth_token.json', 'r') as f:
                self.auth_token = json.load(f)
                print(f"Successfully logged in as: {self.auth_token.get('displayName', 'Unknown')} {os.environ.get('LOGGED_IN', '')}")
        
        try:
            print("Initializing HTTP server...")
            self.http_server = HTTPServer((self.host, self.port), self.create_request_handler())
            print(f'Successfully bound to {self.host}:{self.port}')
        except Exception as e:
            print(f"ERROR: Could not bind to port {self.port}. Error: {str(e)}")
            try:
                subprocess.run(['netstat', '-ano', '|', 'findstr', str(self.port)], shell=True)
            except:
                pass
            print("Press Enter to exit...")
            input()
            raise e
            
        print("Setting up client tracking system...")
        self.connected_clients = {}  # Store client tokens
        self.clients_lock = threading.Lock()
        
        print("Initializing matchmaking system...")
        self.matchmaking_queue = []
        self.match_lock = threading.Lock()
        
        self.logger.info(f'ZeroFN server listening on {self.host}:{self.port}')

<<<<<<< Updated upstream
    def should_refresh_token(self):
        """Check if token needs refreshing"""
        if not self.auth_token:
            return True
            
        # Handle both string timestamp and ISO format
        expires_at = self.auth_token.get('expires_at', '2000-01-01T00:00:00')
        try:
            # Try parsing as ISO format first
            expires_dt = datetime.fromisoformat(expires_at.replace('Z', '+00:00'))
        except ValueError:
            try:
                # Try parsing as Unix timestamp
                expires_dt = datetime.fromtimestamp(int(expires_at), tz=timezone.utc)
            except:
                # If all parsing fails, force refresh
                return True
                
        return datetime.now(timezone.utc) >= expires_dt

    def refresh_auth_token(self):
        """Refresh the Epic Games auth token"""
        try:
            refresh_token = self.auth_token.get('refresh_token')
            if not refresh_token:
                raise Exception("No refresh token available")
                
            auth_string = base64.b64encode(f"{self.client_id}:{self.client_secret}".encode()).decode()
            
            headers = {
                'Authorization': f'Basic {auth_string}',
                'Content-Type': 'application/x-www-form-urlencoded',
                'User-Agent': 'Fortnite/++Fortnite+Release-1.0-CL-3700114 Windows/6.1.7601.2.1.0.768.101'
            }
            
            data = {
                'grant_type': 'refresh_token',
                'refresh_token': refresh_token
            }
            
            # Use legacy Season 2 endpoint
            response = requests.post(
                'https://account-public-service-prod.ol.epicgames.com/account/api/oauth/token',
                headers=headers,
                data=data,
                verify=False
            )
            
            if response.status_code == 200:
                new_token = response.json()
                new_token['expires_at'] = (datetime.now(timezone.utc) + timedelta(seconds=new_token['expires_in'])).isoformat()
                
                self.auth_token = new_token
                with open('auth_token.json', 'w') as f:
                    json.dump(new_token, f, indent=4)
                    
                self.logger.info("Successfully refreshed auth token")
            else:
                raise Exception(f"Token refresh failed: {response.text}")
                
        except Exception as e:
            self.logger.error(f"Error refreshing token: {str(e)}")
            raise

    def create_request_handler(self):
        outer_instance = self
        
        class RequestHandler(BaseHTTPRequestHandler):
            def log_message(self, format, *args):
                outer_instance.logger.info(f"HTTP Request: {format%args}")
            
            def add_client(self, token=None):
                client_ip = self.client_address[0]
                with outer_instance.clients_lock:
                    if token:
                        outer_instance.connected_clients[client_ip] = token
                        outer_instance.logger.info(f'Authenticated client {client_ip} with token')
                    elif client_ip not in outer_instance.connected_clients:
                        if outer_instance.auth_token:
                            token = outer_instance.auth_token.get('access_token')
                        else:
                            token = base64.b64encode(os.urandom(32)).decode()
                        outer_instance.connected_clients[client_ip] = token
                        outer_instance.logger.info(f'New client {client_ip} assigned token')
                    print(f'Total connected clients: {len(outer_instance.connected_clients)}')

            def do_OPTIONS(self):
                self.send_response(200)
                self.send_header('Access-Control-Allow-Origin', '*')
                self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
                self.send_header('Access-Control-Allow-Headers', 'Authorization, Content-Type')
                self.send_header('Access-Control-Max-Age', '86400')
                self.end_headers()

            def do_GET(self):
                outer_instance.logger.info(f"GET request received for path: {self.path}")  # Logging statement
                print(f"Received GET request for path: {self.path}")
                self.add_client()
                
                # Handle Epic callback
                if self.path.startswith('/epic/callback'):
                    try:
                        # Parse URL parameters properly
                        parsed_url = urlparse(self.path)
                        params = parse_qs(parsed_url.query)
                        
                        # Get state and code from parsed parameters
                        state = params.get('state', [''])[0]
                        code = params.get('code', [''])[0]
                        
                        if not state or not code:
                            raise ValueError("Missing required parameters")
                            
                        # Verify state matches expected state
                        if not outer_instance.expected_state or state != outer_instance.expected_state:
                            raise ValueError("Invalid state parameter")
                        
                        # Process the callback
                        self.send_response(200)
                        self.send_header('Content-Type', 'text/html')
                        self.end_headers()
                        self.wfile.write(b"Authorization successful! You can close this window.")
                        return
                    except Exception as e:
                        self.send_response(400)
                        self.send_header('Content-Type', 'text/html')
                        self.end_headers()
                        self.wfile.write(f"Error processing callback: {str(e)}".encode())
                        return
                
                # Check if auth token exists and refresh if needed
                if not outer_instance.auth_token or outer_instance.should_refresh_token():
                    try:
                        outer_instance.refresh_auth_token()
                    except:
                        # Generate new state parameter before redirecting
                        outer_instance.expected_state = base64.b64encode(os.urandom(32)).decode()
                        # Redirect to auth server if refresh fails
                        self.send_response(302)
                        self.send_header('Location', f'http://127.0.0.1:7777?state={outer_instance.expected_state}')
                        self.end_headers()
                        return

                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.send_header('Access-Control-Allow-Credentials', 'true')
                self.send_header('Access-Control-Allow-Headers', 'Authorization, Content-Type')
                self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
                self.end_headers()

                if self.path.startswith('/fortnite/api/game/v2/profile'):
                    # Season 2 specific profile response
                    response = {
                        "profileRevision": random.randint(1000,9999),
                        "profileId": "athena",
                        "profileChanges": [{
                            "changeType": "fullProfileUpdate", 
                            "profile": {
                                "_id": outer_instance.auth_token.get('account_id'),
                                "accountId": outer_instance.auth_token.get('account_id'),
                                "version": "season_2",
                                "season": 2,
                                "items": {
                                    "SKIN_1": {
                                        "templateId": "AthenaCharacter:CID_017_Athena_Commando_M", # Season 2 default skin
                                        "attributes": {
                                            "favorite": True,
                                            "item_seen": True,
                                            "level": 100,
                                            "xp": 999999,
                                            "variants": []
                                        }
                                    }
                                },
                                "stats": {
                                    "attributes": {
                                        "level": 70, # Season 2 max level
                                        "xp": 999999,
                                        "season_level": 70
                                    }
                                },
                                "commandRevision": random.randint(1000,9999)
                            }
                        }],
                        "serverTime": datetime.now(timezone.utc).isoformat(),
                        "profileCommandRevision": random.randint(1000,9999),
                        "responseVersion": 1
                    }
                elif self.path.startswith('/fortnite/api/game/v2/matchmakingservice/ticket'):
                    response = {
                        "serviceUrl": "ws://127.0.0.1:7778",  # Updated to listen on localhost
                        "ticketType": "mms-player",
                        "payload": "mms-player",
                        "signature": "mms-player-signature"
                    }
                elif self.path.startswith('/account/api/oauth/token'):
                    response = {
                        "access_token": outer_instance.auth_token.get('access_token'),
                        "expires_in": 28800,
                        "expires_at": "9999-12-31T23:59:59.999Z",
                        "token_type": "bearer",
                        "account_id": outer_instance.auth_token.get('account_id'),
                        "client_id": outer_instance.client_id,
                        "internal_client": True,
                        "client_service": "fortnite",
                        "refresh_token": outer_instance.auth_token.get('refresh_token'),
                        "refresh_expires": 115200,
                        "refresh_expires_at": "9999-12-31T23:59:59.999Z",
                        "displayName": outer_instance.auth_token.get('displayName', 'ZeroFN Player'),
                        "app": "fortnite",
                        "in_app_id": outer_instance.auth_token.get('account_id')
                    }
                else:
                    response = {
                        "access_token": outer_instance.auth_token.get('access_token'),
                        "expires_in": outer_instance.auth_token.get('expires_in'),
                        "expires_at": outer_instance.auth_token.get('expires_at'),
                        "token_type": outer_instance.auth_token.get('token_type'),
                        "refresh_token": outer_instance.auth_token.get('refresh_token'),
                        "account_id": outer_instance.auth_token.get('account_id'),
                        "client_id": outer_instance.client_id,
                        "internal_client": True,
                        "client_service": "fortnite",
                        "displayName": outer_instance.auth_token.get('displayName', 'ZeroFN Player'),
                        "app": "fortnite",
                        "serverTime": datetime.now(timezone.utc).isoformat()
                    }

                print(f"Sending response: {json.dumps(response)}")
                self.wfile.write(json.dumps(response).encode())

            def do_POST(self):
                outer_instance.logger.info(f"POST request received for path: {self.path}")  # Logging statement
                print(f"Received POST request for path: {self.path}")
                self.add_client()
                
                # Check if auth token exists and refresh if needed
                if not outer_instance.auth_token or outer_instance.should_refresh_token():
                    try:
                        outer_instance.refresh_auth_token()
                    except:
                        self.send_response(401)
                        self.send_header('Content-Type', 'application/json')
                        self.end_headers()
                        self.wfile.write(json.dumps({"error": "Not authenticated"}).encode())
                        return

                content_length = int(self.headers.get('Content-Length', 0))
                post_data = self.rfile.read(content_length)
                
                try:
                    request_json = json.loads(post_data.decode())
                    outer_instance.logger.info(f'Received POST request: {request_json}')
                except:
                    outer_instance.logger.warning('Could not parse POST data as JSON')
                
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.send_header('Access-Control-Allow-Credentials', 'true')
                self.send_header('Access-Control-Allow-Headers', 'Authorization, Content-Type')
                self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
                self.end_headers()

                if self.path.startswith('/account/api/oauth/token'):
                    response = {
                        "access_token": outer_instance.auth_token.get('access_token'),
                        "expires_in": 28800,
                        "expires_at": "9999-12-31T23:59:59.999Z", 
                        "token_type": "bearer",
                        "account_id": outer_instance.auth_token.get('account_id'),
                        "client_id": outer_instance.client_id,
                        "internal_client": True,
                        "client_service": "fortnite",
                        "refresh_token": outer_instance.auth_token.get('refresh_token'),
                        "refresh_expires": 115200,
                        "refresh_expires_at": "9999-12-31T23:59:59.999Z",
                        "displayName": outer_instance.auth_token.get('displayName', 'ZeroFN Player'),
                        "app": "fortnite",
                        "in_app_id": outer_instance.auth_token.get('account_id')
                    }
                else:
                    response = {
                        "access_token": outer_instance.auth_token.get('access_token'),
                        "expires_in": outer_instance.auth_token.get('expires_in'),
                        "expires_at": outer_instance.auth_token.get('expires_at'),
                        "token_type": outer_instance.auth_token.get('token_type'),
                        "refresh_token": outer_instance.auth_token.get('refresh_token'),
                        "account_id": outer_instance.auth_token.get('account_id'),
                        "client_id": outer_instance.client_id,
                        "internal_client": True,
                        "client_service": "fortnite",
                        "displayName": outer_instance.auth_token.get('displayName', 'ZeroFN Player'),
                        "app": "fortnite",
                        "serverTime": datetime.now(timezone.utc).isoformat()
                    }

                print(f"Sending response: {json.dumps(response)}")
                self.wfile.write(json.dumps(response).encode())
=======
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
>>>>>>> Stashed changes

    def start(self):
        try:
            print("Starting server components...")
            print("Initializing network services...")
            time.sleep(2)
            print("Setting up game systems...")
            time.sleep(2)
            print("Finalizing startup...")
            time.sleep(1)
            print("\n=========================")
            print("SERVER IS READY!")
            print("You can now launch Fortnite")
            print("=========================\n")
            print("Press Ctrl+C to stop the server")
            self.logger.info('Starting ZeroFN server...')
            self.http_server.serve_forever()
        except Exception as e:
<<<<<<< Updated upstream
            self.logger.error(f'Server error: {str(e)}')
            print(f"ERROR: Server failed to start: {str(e)}")
            print("Press Enter to exit...")
            input()
        finally:
            self.logger.info('Shutting down server...')
            print("Shutting down server...")
            print("Press Enter to exit...")
            input()
            self.http_server.shutdown()
=======
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

                # Handle different API endpoints
                response_body = ""
                if "/account/api/oauth/token" in path:
                    response_body = self.generate_auth_token_response()
                elif "/fortnite/api/game/v2/profile" in path:
                    response_body = self.generate_profile_response()
                elif "/content/api/pages/fortnite-game" in path:
                    response_body = self.generate_game_info()
                elif "/lightswitch/api/service/bulk/status" in path:
                    response_body = self.generate_lightswitch_response()
                else:
                    # Default auth response for unknown endpoints
                    response_body = self.generate_auth_token_response()

                # Prepare HTTP response headers
                headers = [
                    'HTTP/1.1 200 OK',
                    'Content-Type: application/json',
                    'Access-Control-Allow-Origin: *',
                    'Access-Control-Allow-Methods: GET, POST, OPTIONS',
                    'Access-Control-Allow-Headers: Content-Type, Authorization',
                    'Connection: keep-alive'
                ]

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
            "profileChanges": [],
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
            "_locale": "en-US"
        })

    def generate_lightswitch_response(self):
        return json.dumps([{
            "serviceInstanceId": "fortnite",
            "status": "UP",
            "message": "Fortnite is online",
            "maintenanceUri": None,
            "allowedActions": ["PLAY", "DOWNLOAD"],
            "banned": False
        }])

    def shutdown(self):
        logger.info('Shutting down ZeroFN server...')
        self.running = False
        try:
            self.server_socket.close()
        except Exception:
            pass
        logger.info('Server shutdown complete')

>>>>>>> Stashed changes

if __name__ == '__main__':
    try:
        server = FortniteServer()
        server.start()
    except KeyboardInterrupt:
        logging.info('Server stopped by user')
        print("\nServer stopped by user")
        print("Press Enter to exit...")
        input()
    except Exception as e:
        logging.error(f'Fatal server error: {str(e)}')
        print(f"\nFatal server error: {str(e)}")
        print("Press Enter to exit...")
        input()

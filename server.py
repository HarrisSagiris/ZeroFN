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
from datetime import datetime
from pathlib import Path
import subprocess
import os
import jwt

print("Starting ZeroFN Server...")
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

class FortniteServer:
    def __init__(self):
        print("Setting up server configuration...")
        self.logger = logging.getLogger('FortniteServer')
        self.host = '127.0.0.1'  # Listen only on localhost
        self.port = 7777
        
        # Epic Games OAuth credentials
        self.client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
        self.client_secret = "Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA"
        self.redirect_uri = f"http://{self.host}:{self.port}/auth/callback"
        
        # JWT secret for token signing
        self.jwt_secret = "zerofn_secret_key"
        
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
            raise e
            
        print("Setting up client tracking system...")
        self.connected_clients = {}  # Store client tokens
        self.clients_lock = threading.Lock()
        
        print("Initializing matchmaking system...")
        self.matchmaking_queue = []
        self.match_lock = threading.Lock()
        
        self.logger.info(f'Fortnite private server listening on {self.host}:{self.port}')

    def generate_token(self, account_id):
        # Generate JWT token with account info
        payload = {
            'sub': account_id,
            'iss': 'ZeroFN',
            'iat': datetime.utcnow(),
            'exp': datetime.utcnow() + datetime.timedelta(hours=8),
            'jti': str(random.randint(1000000, 9999999)),
            'displayName': f'ZeroFN_Player_{account_id[:8]}',
            'scope': 'fortnite:profile:* fortnite:stats:* fortnite:cloudstorage:*'
        }
        token = jwt.encode(payload, self.jwt_secret, algorithm='HS256')
        return token

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
                        token = outer_instance.generate_token(str(random.randint(10000, 99999)))
                        outer_instance.connected_clients[client_ip] = token
                        outer_instance.logger.info(f'New client {client_ip} assigned token')
                    print(f'Total connected clients: {len(outer_instance.connected_clients)}')

            def validate_token(self, auth_header):
                if not auth_header:
                    return False
                try:
                    token = auth_header.split(' ')[1]
                    jwt.decode(token, outer_instance.jwt_secret, algorithms=['HS256'])
                    return True
                except:
                    return False

            def do_GET(self):
                print(f"Received GET request for path: {self.path}")
                
                # Handle Epic authentication flow
                if self.path == '/login':
                    auth_url = f"https://www.epicgames.com/id/api/redirect?clientId={outer_instance.client_id}&responseType=code&redirectUrl={outer_instance.redirect_uri}"
                    self.send_response(302)
                    self.send_header('Location', auth_url)
                    self.end_headers()
                    return
                    
                elif self.path.startswith('/auth/callback'):
                    # Generate token without actual Epic validation
                    account_id = f"zerofn_{random.randint(10000,99999)}"
                    token = outer_instance.generate_token(account_id)
                    self.add_client(token)
                    
                    self.send_response(200)
                    self.send_header('Content-Type', 'application/json')
                    self.end_headers()
                    
                    auth_response = {
                        "access_token": token,
                        "expires_in": 28800,
                        "expires_at": "9999-12-31T23:59:59.999Z", 
                        "token_type": "bearer",
                        "refresh_token": token,
                        "account_id": account_id,
                        "client_id": outer_instance.client_id,
                        "internal_client": True,
                        "client_service": "fortnite",
                        "displayName": f"ZeroFN_Player_{account_id[-4:]}",
                        "app": "fortnite",
                        "in_app_id": account_id,
                        "device_id": f"zerofn_device_{random.randint(1000,9999)}"
                    }
                    
                    self.wfile.write(json.dumps(auth_response).encode())
                    return

                # Validate token for other endpoints
                auth_header = self.headers.get('Authorization')
                if not self.validate_token(auth_header):
                    self.add_client()  # Generate new token if invalid

                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()

                if self.path.startswith('/fortnite/api/game/v2/profile'):
                    # Enhanced cosmetics response
                    response = {
                        "profileRevision": random.randint(1000,9999),
                        "profileId": "athena",
                        "profileChanges": [{
                            "changeType": "fullProfileUpdate",
                            "profile": {
                                "_id": f"profile_{random.randint(10000,99999)}",
                                "accountId": self.connected_clients.get(self.client_address[0], "default_account"),
                                "items": {
                                    # Expanded cosmetics
                                    "SKIN_1": {
                                        "templateId": "AthenaCharacter:CID_001_Athena_Commando_F_Default",
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
                                        "level": 100,
                                        "xp": 999999,
                                        "season_level": 100
                                    }
                                },
                                "commandRevision": random.randint(1000,9999)
                            }
                        }],
                        "serverTime": datetime.now().isoformat(),
                        "profileCommandRevision": random.randint(1000,9999),
                        "responseVersion": 1
                    }

                elif self.path.startswith('/fortnite/api/matchmaking/session'):
                    response = self.handle_matchmaking()
                else:
                    # Default response with token refresh
                    client_token = outer_instance.generate_token(str(random.randint(10000,99999)))
                    self.add_client(client_token)
                    
                    response = {
                        "access_token": client_token,
                        "expires_in": 28800,
                        "expires_at": "9999-12-31T23:59:59.999Z",
                        "token_type": "bearer",
                        "refresh_token": client_token,
                        "refresh_expires": 115200,
                        "refresh_expires_at": "9999-12-31T23:59:59.999Z",
                        "account_id": f"zerofn_{random.randint(10000,99999)}",
                        "client_id": outer_instance.client_id,
                        "internal_client": True,
                        "client_service": "fortnite",
                        "displayName": "ZeroFN Player",
                        "app": "fortnite",
                        "in_app_id": f"zerofn_app_{random.randint(1000,9999)}",
                        "device_auth": {
                            "device_id": f"zerofn_device_{random.randint(1000,9999)}",
                            "account_id": f"zerofn_{random.randint(10000,99999)}",
                            "secret": base64.b64encode(os.urandom(32)).decode(),
                            "user_agent": "ZeroFNClient/1.0.0"
                        },
                        "serverTime": datetime.now().isoformat()
                    }

                print(f"Sending response: {json.dumps(response)}")
                self.wfile.write(json.dumps(response).encode())

            def do_POST(self):
                print(f"Received POST request for path: {self.path}")
                content_length = int(self.headers.get('Content-Length', 0))
                post_data = self.rfile.read(content_length)
                
                try:
                    request_json = json.loads(post_data.decode())
                    outer_instance.logger.info(f'Received POST request: {request_json}')
                except:
                    outer_instance.logger.warning('Could not parse POST data as JSON')
                
                # Generate new token for POST request
                client_token = outer_instance.generate_token(str(random.randint(10000,99999)))
                self.add_client(client_token)
                
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()

                response = {
                    "access_token": client_token,
                    "expires_in": 28800,
                    "expires_at": "9999-12-31T23:59:59.999Z",
                    "token_type": "bearer",
                    "account_id": f"zerofn_{random.randint(10000,99999)}",
                    "client_id": outer_instance.client_id,
                    "internal_client": True,
                    "client_service": "fortnite",
                    "displayName": "ZeroFN Player",
                    "app": "fortnite",
                    "serverTime": datetime.now().isoformat()
                }

                print(f"Sending response: {json.dumps(response)}")
                self.wfile.write(json.dumps(response).encode())

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
            print("You can now launch the game")
            print("=========================\n")
            self.logger.info('Starting Fortnite private server...')
            self.http_server.serve_forever()
        except Exception as e:
            self.logger.error(f'Server error: {str(e)}')
            print(f"ERROR: Server failed to start: {str(e)}")
        finally:
            self.logger.info('Shutting down server...')
            print("Shutting down server...")
            self.http_server.shutdown()

if __name__ == '__main__':
    try:
        server = FortniteServer()
        server.start()
    except KeyboardInterrupt:
        logging.info('Server stopped by user')
        print("\nServer stopped by user")
    except Exception as e:
        logging.error(f'Fatal server error: {str(e)}')
        print(f"\nFatal server error: {str(e)}")

import socket
import threading
import json
import time
import requests
import base64
import logging
import random
from http.server import HTTPServer, BaseHTTPRequestHandler
from datetime import datetime
from pathlib import Path

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
        self.host = '0.0.0.0'  # Listen on all interfaces
        self.port = 7777
        
        print("Initializing HTTP server...")
        # Initialize HTTP server
        self.http_server = HTTPServer((self.host, self.port), self.create_request_handler())
        
        print("Setting up client tracking system...")
        # Track connected clients
        self.connected_clients = set()
        self.clients_lock = threading.Lock()
        
        print("Initializing matchmaking system...")
        # Initialize matchmaking queue
        self.matchmaking_queue = []
        self.match_lock = threading.Lock()
        
        print(f'Server bound to {self.host}:{self.port}')
        self.logger.info(f'Fortnite private server listening on {self.host}:{self.port}')

    def create_request_handler(self):
        outer_instance = self
        
        class RequestHandler(BaseHTTPRequestHandler):
            def log_message(self, format, *args):
                outer_instance.logger.info(f"HTTP Request: {format%args}")
            
            def add_client(self):
                client_ip = self.client_address[0]
                with outer_instance.clients_lock:
                    if client_ip not in outer_instance.connected_clients:
                        outer_instance.connected_clients.add(client_ip)
                        outer_instance.logger.info(f'New client connected from {client_ip}')
                        outer_instance.logger.info(f'Total connected clients: {len(outer_instance.connected_clients)}')
                        print(f'New client connected from {client_ip}')
                        print(f'Total connected clients: {len(outer_instance.connected_clients)}')
                
            def do_GET(self):
                self.add_client()
                self.send_response(200)
                self.send_header('Content-Type', 'application/json')
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()

                if self.path.startswith('/fortnite/api/game/v2/profile'):
                    # Return cosmetics inventory
                    response = {
                        "profileRevision": 1,
                        "profileId": "athena",
                        "profileChanges": [{
                            "changeType": "fullProfileUpdate",
                            "profile": {
                                "_id": "valid_profile",
                                "accountId": "valid_account",
                                "items": {
                                    # Default skins
                                    "SKIN_1": {
                                        "templateId": "AthenaCharacter:CID_001_Athena_Commando_F_Default",
                                        "attributes": {"favorite": False}
                                    },
                                    "SKIN_2": {
                                        "templateId": "AthenaCharacter:CID_002_Athena_Commando_F_Default", 
                                        "attributes": {"favorite": False}
                                    },
                                    # Default pickaxes
                                    "PICKAXE_1": {
                                        "templateId": "AthenaPickaxe:DefaultPickaxe",
                                        "attributes": {"favorite": False}
                                    },
                                    # Default gliders
                                    "GLIDER_1": {
                                        "templateId": "AthenaGlider:DefaultGlider",
                                        "attributes": {"favorite": False}  
                                    }
                                },
                                "stats": {
                                    "attributes": {
                                        "level": 100,
                                        "xp": 999999
                                    }
                                }
                            }
                        }],
                        "serverTime": datetime.now().isoformat(),
                        "responseVersion": 1
                    }
                elif self.path.startswith('/fortnite/api/matchmaking/session'):
                    # Handle matchmaking
                    with outer_instance.match_lock:
                        if len(outer_instance.matchmaking_queue) >= 2:
                            # Create match with players in queue
                            players = outer_instance.matchmaking_queue[:2]
                            outer_instance.matchmaking_queue = outer_instance.matchmaking_queue[2:]
                            
                            response = {
                                "status": "MATCHED",
                                "matchId": str(random.randint(1000, 9999)),
                                "players": players,
                                "serverTime": datetime.now().isoformat()
                            }
                            outer_instance.logger.info(f'Created match {response["matchId"]} with players {players}')
                        else:
                            # Add to queue
                            player_id = str(random.randint(1, 1000))
                            outer_instance.matchmaking_queue.append(player_id)
                            
                            response = {
                                "status": "QUEUED",
                                "queuePosition": len(outer_instance.matchmaking_queue),
                                "estimatedWaitTime": 30,
                                "serverTime": datetime.now().isoformat()
                            }
                            outer_instance.logger.info(f'Added player {player_id} to matchmaking queue')
                else:
                    # Default auth response
                    response = {
                        "access_token": "eg1~valid_token",
                        "expires_in": 28800,
                        "expires_at": "9999-12-31T23:59:59.999Z",
                        "token_type": "bearer",
                        "refresh_token": "eg1~valid_refresh", 
                        "refresh_expires": 115200,
                        "refresh_expires_at": "9999-12-31T23:59:59.999Z",
                        "account_id": "valid_account",
                        "client_id": "valid_client",
                        "internal_client": True,
                        "client_service": "fortnite",
                        "displayName": "ZeroFN Player",
                        "app": "fortnite",
                        "in_app_id": "valid_app_id",
                        "serverTime": datetime.now().isoformat()
                    }
                    outer_instance.logger.info(f'Authenticated client {self.client_address[0]}')

                self.wfile.write(json.dumps(response).encode())

            def do_POST(self):
                self.add_client()
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
                self.end_headers()

                response = {
                    "access_token": "eg1~valid_token",
                    "expires_in": 28800,
                    "expires_at": "9999-12-31T23:59:59.999Z",
                    "token_type": "bearer",
                    "account_id": "valid_account",
                    "client_id": "valid_client", 
                    "internal_client": True,
                    "client_service": "fortnite",
                    "displayName": "ZeroFN Player",
                    "app": "fortnite",
                    "serverTime": datetime.now().isoformat()
                }

                self.wfile.write(json.dumps(response).encode())

    def start(self):
        try:
            print("Starting server components...")
            print("Initializing network services...")
            time.sleep(2)  # Allow time for network initialization
            print("Setting up game systems...")
            time.sleep(2)  # Allow time for game systems
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

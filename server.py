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

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s',
    handlers=[
        logging.FileHandler('server.log'),
        logging.StreamHandler()
    ]
)

class FortniteServer:
    def __init__(self):
        self.logger = logging.getLogger('FortniteServer')
        self.host = '0.0.0.0'  # Listen on all interfaces
        self.port = 7777
        
        # Initialize HTTP server for auth bypass
        self.http_server = HTTPServer((self.host, self.port), self.create_auth_handler())
        
        # Initialize matchmaking queue
        self.matchmaking_queue = []
        self.match_lock = threading.Lock()
        
        self.logger.info(f'Auth bypass server listening on {self.host}:{self.port}')

    def create_auth_handler(self):
        outer_instance = self
        
        class AuthHandler(BaseHTTPRequestHandler):
            def log_message(self, format, *args):
                outer_instance.logger.info(f"HTTP Request: {format%args}")
                
            def do_GET(self):
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
                                }
                            }
                        }]
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
                                "players": players
                            }
                        else:
                            # Add to queue
                            player_id = str(random.randint(1, 1000))
                            outer_instance.matchmaking_queue.append(player_id)
                            
                            response = {
                                "status": "QUEUED",
                                "queuePosition": len(outer_instance.matchmaking_queue),
                                "estimatedWaitTime": 30
                            }
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
                        "in_app_id": "valid_app_id"
                    }

                self.wfile.write(json.dumps(response).encode())

            def do_POST(self):
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
                }

                self.wfile.write(json.dumps(response).encode())

    def start(self):
        try:
            self.logger.info('Starting auth bypass server...')
            self.http_server.serve_forever()
        except Exception as e:
            self.logger.error(f'Server error: {str(e)}')
        finally:
            self.logger.info('Shutting down server...')
            self.http_server.shutdown()

if __name__ == '__main__':
    try:
        server = FortniteServer()
        server.start()
    except KeyboardInterrupt:
        logging.info('Server stopped by user')
    except Exception as e:
        logging.error(f'Fatal server error: {str(e)}')

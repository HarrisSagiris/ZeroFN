import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import subprocess
import os
import sys
import threading
import webbrowser
import shutil
import requests
from pathlib import Path
import logging
import time
import ctypes
import socket
import json
import uuid
import random
from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse
import secrets
import base64
import winreg
from datetime import datetime, timezone, timedelta
import jwt
import mitmproxy.ctx
from mitmproxy import ctx, http

# Configure Fortnite theme styles
class FortniteTheme:
    BG_COLOR = '#121212'
    FG_COLOR = '#ffffff'
    ACCENT_COLOR = '#fccc4d' 
    SECONDARY_COLOR = '#2b2b2b'
    HIGHLIGHT_COLOR = '#ffc700'

    @staticmethod
    def configure_styles():
        style = ttk.Style()
        
        # Configure main styles
        style.configure('Fortnite.TFrame', background=FortniteTheme.BG_COLOR)
        style.configure('Fortnite.TLabel',
                       background=FortniteTheme.BG_COLOR,
                       foreground=FortniteTheme.FG_COLOR,
                       font=("Segoe UI", 10))
        
        # Custom button style
        style.configure('Fortnite.TButton',
                       background=FortniteTheme.ACCENT_COLOR,
                       foreground=FortniteTheme.BG_COLOR,
                       font=("Segoe UI", 10, "bold"),
                       padding=10)
        style.map('Fortnite.TButton',
                 background=[('active', FortniteTheme.HIGHLIGHT_COLOR)],
                 foreground=[('active', FortniteTheme.BG_COLOR)])
                 
        # Frame styles
        style.configure('Fortnite.TLabelframe',
                       background=FortniteTheme.BG_COLOR,
                       foreground=FortniteTheme.FG_COLOR)
        style.configure('Fortnite.TLabelframe.Label',
                       background=FortniteTheme.BG_COLOR,
                       foreground=FortniteTheme.ACCENT_COLOR,
                       font=("Segoe UI", 11, "bold"))

        # Combobox style
        style.configure('Fortnite.TCombobox',
                       background=FortniteTheme.SECONDARY_COLOR,
                       foreground=FortniteTheme.FG_COLOR,
                       fieldbackground=FortniteTheme.SECONDARY_COLOR,
                       arrowcolor=FortniteTheme.ACCENT_COLOR)

# Custom HTTP request handler for Fortnite server
class FortniteServerHandler(BaseHTTPRequestHandler):
    clients = set()
    connection_count = 0
    last_activity = {}
    state = None
    
    def __init__(self, *args, **kwargs):
        self.server_should_close = False
        super().__init__(*args, **kwargs)
        
    def log_connection(self, client_ip, action):
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] {action}: {client_ip}")
        FortniteServerHandler.last_activity[client_ip] = timestamp
        
        with open('logs/server.log', 'a') as f:
            f.write(f"[{timestamp}] {action}: {client_ip}\n")
            f.write(f"Headers: {self.headers}\n")
            f.write(f"Path: {self.path}\n")
            f.write("-" * 80 + "\n")
    
    def send_json_response(self, data, status=200):
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Authorization, Content-Type')
        self.send_header('X-Epic-Correlation-ID', str(uuid.uuid4()))
        self.send_header('X-Epic-Device-ID', str(uuid.uuid4()))
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

    def do_GET(self):
        try:
            client_ip = self.client_address[0]
            if client_ip not in FortniteServerHandler.clients:
                FortniteServerHandler.clients.add(client_ip)
                FortniteServerHandler.connection_count += 1
                self.log_connection(client_ip, "New client connected")
                
            # Handle Epic callback
            if self.path.startswith('/epic/callback'):
                try:
                    parsed_url = urllib.parse.urlparse(self.path)
                    params = urllib.parse.parse_qs(parsed_url.query)
                    
                    state = params.get('state', [''])[0]
                    code = params.get('code', [''])[0]
                    
                    if not state or not code:
                        raise ValueError("Missing required parameters")
                        
                    if not FortniteServerHandler.state or state != FortniteServerHandler.state:
                        raise ValueError("Invalid state parameter")
                    
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

            # Handle verification endpoint
            elif self.path == "/account/api/oauth/verify":
                self.log_connection(client_ip, "Client verification")
                response = {
                    "access_token": str(uuid.uuid4()),
                    "expires_in": 28800,
                    "expires_at": "9999-12-31T23:59:59.999Z",
                    "token_type": "bearer",
                    "account_id": str(uuid.uuid4()),
                    "client_id": "xyza7891TydzdNolyGQJYa9b6n6rLMJl",
                    "internal_client": True,
                    "client_service": "fortnite",
                    "displayName": "ZeroFN Player",
                    "app": "fortnite",
                    "in_app_id": str(uuid.uuid4())
                }
                self.send_json_response(response)

            # Handle cloud storage endpoints
            elif self.path == "/fortnite/api/cloudstorage/system":
                self.log_connection(client_ip, "Requesting cloud storage")
                self.send_json_response([])
                
            elif self.path == "/fortnite/api/game/v2/enabled_features":
                self.log_connection(client_ip, "Checking enabled features")
                self.send_json_response([])
                
            elif self.path.startswith("/fortnite/api/cloudstorage/user/"):
                self.log_connection(client_ip, "User storage access")
                self.send_json_response([])

            # Handle waiting room check
            elif self.path == "/waitingroom/api/waitingroom":
                self.log_connection(client_ip, "Waiting room check")
                self.send_json_response(None)

            # Handle service status check
            elif self.path == "/lightswitch/api/service/bulk/status":
                self.log_connection(client_ip, "Service status check")
                response = [{
                    "serviceInstanceId": "fortnite",
                    "status": "UP",
                    "message": "Fortnite is online",
                    "maintenanceUri": None,
                    "allowedActions": ["PLAY", "DOWNLOAD"],
                    "banned": False
                }]
                self.send_json_response(response)

            # Handle platform validation
            elif self.path.startswith("/fortnite/api/game/v2/tryPlayOnPlatform"):
                self.log_connection(client_ip, "Platform validation")
                response = {
                    "platformValid": True,
                    "clientPlatform": "WIN"
                }
                self.send_json_response(response)

            # Handle privacy settings
            elif self.path.startswith("/fortnite/api/game/v2/privacy/account/"):
                self.log_connection(client_ip, "Privacy settings check")
                response = {
                    "accountId": str(uuid.uuid4()),
                    "optOutOfPublicLeaderboards": False
                }
                self.send_json_response(response)

            # Handle account info
            elif self.path == "/account/api/public/account":
                self.log_connection(client_ip, "Public account info")
                response = {
                    "id": str(uuid.uuid4()),
                    "displayName": "ZeroFN Player",
                    "externalAuths": {}
                }
                self.send_json_response(response)

            # Handle OAuth exchange
            elif self.path == "/account/api/oauth/exchange":
                self.log_connection(client_ip, "OAuth exchange")
                response = {
                    "expiresInSeconds": 28800,
                    "code": str(uuid.uuid4()),
                    "creatingClientId": "ec684b8c687f479fadea3cb2ad83f5c6"  
                }
                self.send_json_response(response)

            else:
                self.log_connection(client_ip, f"Generic request to {self.path}")
                response = {
                    "status": "ok",
                    "message": "ZeroFN server running",
                    "serverTime": datetime.now(timezone.utc).isoformat()
                }
                self.send_json_response(response)

        except Exception as e:
            self.log_connection(client_ip, f"Error: {str(e)}")
            self.send_error(500, str(e))

    def do_POST(self):
        try:
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            
            data = json.loads(post_data.decode())
            client_ip = self.client_address[0]
            
            if client_ip not in FortniteServerHandler.clients:
                FortniteServerHandler.clients.add(client_ip)
                FortniteServerHandler.connection_count += 1
                self.log_connection(client_ip, "New client connected via POST")
            
            # Handle authentication
            if self.path == "/account/api/oauth/token":
                self.log_connection(client_ip, "Client authentication")
                response = {
                    "access_token": str(uuid.uuid4()),
                    "expires_in": 28800,
                    "expires_at": "9999-12-31T23:59:59.999Z",
                    "token_type": "bearer",
                    "refresh_token": str(uuid.uuid4()),
                    "refresh_expires": 115200,
                    "refresh_expires_at": "9999-12-31T23:59:59.999Z",
                    "account_id": str(uuid.uuid4()),
                    "client_id": "xyza7891TydzdNolyGQJYa9b6n6rLMJl",
                    "internal_client": True,
                    "client_service": "fortnite",
                    "displayName": "ZeroFN Player",
                    "app": "fortnite"
                }
                self.send_json_response(response)
                
            # Handle profile query
            elif self.path == "/fortnite/api/game/v2/profile/client/QueryProfile":
                self.log_connection(client_ip, "Profile query")
                response = {
                    "profileId": data.get("profileId", "athena"),
                    "profileChanges": [{
                        "_type": "fullProfileUpdate",
                        "profile": {
                            "_id": str(uuid.uuid4()),
                            "accountId": str(uuid.uuid4()),
                            "profileId": data.get("profileId", "athena"),
                            "version": "no_version",
                            "items": {},
                            "stats": {
                                "attributes": {
                                    "season_num": 2,
                                    "book_level": 70,
                                    "book_xp": 999999,
                                    "battlestars": 999999,
                                    "battlepass_tier": 70
                                }
                            },
                            "commandRevision": 0
                        }
                    }],
                    "serverTime": datetime.now(timezone.utc).isoformat(),
                    "profileCommandRevision": 0,
                    "responseVersion": 1
                }
                self.send_json_response(response)
                
            # Handle matchmaking
            elif self.path == "/fortnite/api/game/v2/matchmaking/account":
                self.log_connection(client_ip, "Matchmaking request")
                response = {
                    "accountId": str(uuid.uuid4()),
                    "matches": [],
                    "startTime": datetime.now(timezone.utc).isoformat(),
                    "endTime": datetime.now(timezone.utc).isoformat()
                }
                self.send_json_response(response)
                
            else:
                self.log_connection(client_ip, f"Generic POST to {self.path}")
                response = {"status": "ok"}
                self.send_json_response(response)
            
        except Exception as e:
            self.log_connection(client_ip, f"Error: {str(e)}")
            self.send_error(500, str(e))
            
    def log_message(self, format, *args):
        pass

class ZeroFNServer:
    def __init__(self, host="127.0.0.1", port=7777):
        self.host = host
        self.port = port
        self.server = None
        self.running = False
        self.logger = logging.getLogger('ZeroFNServer')
        
        # Epic Games OAuth credentials
        self.client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
        self.client_secret = "Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA"
        
    def start(self):
        try:
            self.server = HTTPServer((self.host, self.port), FortniteServerHandler)
            self.running = True
            
            print("\n=========================")
            print("SERVER IS READY!")
            print("You can now launch Fortnite")
            print("=========================\n")
            
            while self.running:
                self.server.handle_request()
                
        except Exception as e:
            self.logger.error(f'Server error: {str(e)}')
            
    def stop(self):
        self.running = False
        if self.server:
            self.server.server_close()

class ZeroFNApp:
    def __init__(self, root):
        if not self.is_admin():
            self.restart_as_admin()
            
        self.root = root
        self.root.title("ZeroFN Launcher")
        self.root.geometry("1000x800")
        self.root.configure(bg=FortniteTheme.BG_COLOR)
        
        FortniteTheme.configure_styles()
        
        self.fortnite_path = tk.StringVar()
        self.selected_season = tk.StringVar(value="Chapter 1 Season 2")
        
        self.season_urls = {
            "Chapter 1 Season 2": "https://cdn.fnbuilds.services/1.11.zip"
        }
        
        self.server = ZeroFNServer()
        
        if not os.path.exists('logs'):
            os.makedirs('logs')
            
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('logs/zerofn.log'),
                logging.StreamHandler()
            ]
        )
        self.logger = logging.getLogger('ZeroFN')
        
        self.create_gui()
        
    def is_admin(self):
        try:
            return ctypes.windll.shell32.IsUserAnAdmin()
        except:
            return False
            
    def restart_as_admin(self):
        ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)
        sys.exit()
        
    def create_gui(self):
        main_frame = ttk.Frame(self.root, style='Fortnite.TFrame', padding=30)
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Header
        header_frame = ttk.Frame(main_frame, style='Fortnite.TFrame')
        header_frame.pack(fill=tk.X, pady=(0,30))
        
        title_label = ttk.Label(
            header_frame,
            text="ZeroFN",
            font=("Segoe UI", 48, "bold"),
            foreground=FortniteTheme.ACCENT_COLOR,
            style='Fortnite.TLabel'
        )
        title_label.pack()
        
        subtitle = ttk.Label(
            header_frame,
            text="Experience Chapter 1 Season 2 like never before",
            font=("Segoe UI", 16),
            foreground=FortniteTheme.FG_COLOR,
            style='Fortnite.TLabel'
        )
        subtitle.pack(pady=5)
        
        # Path selection
        path_frame = ttk.LabelFrame(
            main_frame,
            text="FORTNITE LOCATION",
            style='Fortnite.TLabelframe',
            padding=20
        )
        path_frame.pack(fill=tk.X, pady=20)
        
        path_entry = ttk.Entry(
            path_frame,
            textvariable=self.fortnite_path,
            font=("Segoe UI", 11),
            width=50
        )
        path_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0,15))
        
        browse_btn = ttk.Button(
            path_frame,
            text="BROWSE",
            command=self.browse_fortnite,
            style='Fortnite.TButton'
        )
        browse_btn.pack(side=tk.RIGHT)
        
        # Action buttons
        btn_frame = ttk.Frame(main_frame, style='Fortnite.TFrame')
        btn_frame.pack(pady=30)
        
        for btn_text, btn_cmd in [
            ("INSTALL FORTNITE", self.install_fortnite),
            ("START GAME", self.start_game),
            ("JOIN DISCORD", lambda: webbrowser.open('https://discord.gg/yCY4FTMPdK'))
        ]:
            btn = ttk.Button(
                btn_frame,
                text=btn_text,
                command=btn_cmd,
                style='Fortnite.TButton',
                width=25
            )
            btn.pack(pady=10)
        
        # Console output
        console_frame = ttk.LabelFrame(
            main_frame,
            text="CONSOLE",
            style='Fortnite.TLabelframe',
            padding=15
        )
        console_frame.pack(fill=tk.BOTH, expand=True, pady=(20,0))
        
        scrollbar = ttk.Scrollbar(console_frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        self.console = tk.Text(
            console_frame,
            height=15,
            bg=FortniteTheme.SECONDARY_COLOR,
            fg=FortniteTheme.FG_COLOR,
            font=("Consolas", 10),
            yscrollcommand=scrollbar.set
        )
        self.console.pack(fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.console.yview)
        
    def log(self, message):
        timestamp = time.strftime("%H:%M:%S")
        self.console.insert(tk.END, f"[{timestamp}] {message}\n")
        self.console.see(tk.END)
        self.logger.info(message)
        
    def browse_fortnite(self):
        path = filedialog.askdirectory()
        if path:
            self.fortnite_path.set(path)
            self.log(f"Selected path: {path}")
            
    def install_fortnite(self):
        if not self.fortnite_path.get():
            messagebox.showerror("Error", "Please select installation path first!")
            return
            
        url = self.season_urls["Chapter 1 Season 2"]
        path = self.fortnite_path.get()
        
        def download():
            try:
                self.log("Starting download...")
                response = requests.get(url, stream=True)
                total = int(response.headers.get('content-length', 0))
                
                with open("fortnite.zip", 'wb') as f:
                    downloaded = 0
                    for data in response.iter_content(chunk_size=4096):
                        downloaded += len(data)
                        f.write(data)
                        done = int(50 * downloaded / total)
                        self.log(f"Download progress: [{'=' * done}{' ' * (50-done)}] {done*2}%")
                        
                self.log("Extracting files...")
                shutil.unpack_archive("fortnite.zip", path)
                os.remove("fortnite.zip")
                
                self.log("Installation complete!")
                
            except Exception as e:
                self.log(f"Error during installation: {str(e)}")
                messagebox.showerror("Installation Error", str(e))
                
        threading.Thread(target=download, daemon=True).start()
        
    def start_game(self):
        if not self.fortnite_path.get():
            messagebox.showerror("Error", "Please select Fortnite path first!")
            return
            
        try:
            # Start server
            server_thread = threading.Thread(target=self.server.start, daemon=True)
            server_thread.start()
            self.log("Server started on 127.0.0.1:7777")
            
            # Kill existing processes
            processes = ["FortniteClient-Win64-Shipping.exe", "EasyAntiCheat.exe", "BEService.exe"]
            for proc in processes:
                subprocess.run(["taskkill", "/f", "/im", proc], 
                             stdout=subprocess.DEVNULL,
                             stderr=subprocess.DEVNULL)
            
            # Launch game
            game_path = os.path.join(self.fortnite_path.get(), 
                                   "FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe")
            
            if not os.path.exists(game_path):
                raise FileNotFoundError("Fortnite executable not found!")
                
            args = [
                game_path,
                "-NOSSLPINNING",
                "-AUTH_TYPE=epic",
                "-AUTH_LOGIN=unused",
                "-AUTH_PASSWORD=zerofn",
                "-epicapp=Fortnite",
                "-epicenv=Prod",
                "-epiclocale=en-us",
                "-epicportal",
                "-noeac",
                "-nobe",
                "-fltoken=fn",
                "-skippatchcheck",
                "-notexturestreaming",
                "-HTTP=127.0.0.1:7777",
                "-AUTH_HOST=127.0.0.1:7777",
                "-AUTH_SSL=0",
                "-AUTH_VERIFY_SSL=0",
                "-AUTH_EPIC=0",
                "-AUTH_EPIC_ONLY=0",
                "-FORCECLIENT=127.0.0.1:7777",
                "-NOEPICWEB",
                "-NOEPICFRIENDS",
                "-NOEAC",
                "-NOBE",
                "-FORCECLIENT_HOST=127.0.0.1:7777",
                "-DISABLEFORTNITELOGIN",
                "-DISABLEEPICLOGIN",
                "-DISABLEEPICGAMESLOGIN",
                "-DISABLEEPICGAMESPORTAL",
                "-DISABLEEPICGAMESVERIFY"
            ]
            
            subprocess.Popen(args)
            self.log("Game launched successfully!")
            
        except Exception as e:
            self.log(f"Error starting game: {str(e)}")
            messagebox.showerror("Launch Error", str(e))
            self.server.stop()
            
    def __del__(self):
        if hasattr(self, 'server'):
            self.server.stop()

if __name__ == "__main__":
    root = tk.Tk()
    app = ZeroFNApp(root)
    root.mainloop()

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
    
    def log_connection(self, client_ip, action):
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] {action}: {client_ip}")
        FortniteServerHandler.last_activity[client_ip] = timestamp
    
    def send_json_response(self, data, status=200):
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

    def do_GET(self):
        try:
            client_ip = self.client_address[0]
            if client_ip not in FortniteServerHandler.clients:
                FortniteServerHandler.clients.add(client_ip)
                FortniteServerHandler.connection_count += 1
                self.log_connection(client_ip, "New client connected")
                print(f"Total connections: {FortniteServerHandler.connection_count}")
            
            # Handle verification endpoint
            if self.path == "/account/api/oauth/verify":
                self.log_connection(client_ip, "Client verification")
                response = {
                    "access_token": str(uuid.uuid4()),
                    "expires_in": 28800,
                    "expires_at": "9999-12-31T23:59:59.999Z",
                    "token_type": "bearer",
                    "account_id": "ZeroFN",
                    "client_id": "ZeroFN",
                    "internal_client": True,
                    "client_service": "fortnite",
                    "displayName": "ZeroFN",
                    "app": "fortnite",
                    "in_app_id": "ZeroFN"
                }
                self.send_json_response(response)
            elif self.path == "/fortnite/api/cloudstorage/system":
                self.log_connection(client_ip, "Requesting cloud storage")
                self.send_json_response([])
            elif self.path == "/fortnite/api/game/v2/enabled_features":
                self.log_connection(client_ip, "Checking enabled features")
                self.send_json_response([])
            elif self.path.startswith("/fortnite/api/cloudstorage/user/"):
                self.log_connection(client_ip, "User storage access")
                self.send_json_response([])
            elif self.path == "/waitingroom/api/waitingroom":
                self.log_connection(client_ip, "Waiting room check")
                self.send_json_response(None)
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
            elif self.path == "/fortnite/api/game/v2/tryPlayOnPlatform/account/ZeroFN":
                self.log_connection(client_ip, "Platform validation")
                response = {"platformValid": True}
                self.send_json_response(response)
            elif self.path == "/fortnite/api/game/v2/privacy/account/ZeroFN":
                self.log_connection(client_ip, "Privacy settings check")
                response = {"accountId": "ZeroFN", "optOutOfPublicLeaderboards": False}
                self.send_json_response(response)
            else:
                self.log_connection(client_ip, f"Generic request to {self.path}")
                response = {
                    "status": "ok",
                    "message": "ZeroFN server running",
                    "client_ip": client_ip,
                    "total_connections": FortniteServerHandler.connection_count,
                    "active_clients": len(FortniteServerHandler.clients)
                }
                self.send_json_response(response)
            
        except ConnectionAbortedError:
            FortniteServerHandler.clients.remove(client_ip)
            FortniteServerHandler.connection_count -= 1
            self.log_connection(client_ip, "Client disconnected")
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
            
            # Handle different endpoints
            if self.path == "/account/api/oauth/token":
                self.log_connection(client_ip, "Client authentication")
                response = {
                    "access_token": "zerofn_access_token",
                    "expires_in": 28800,
                    "expires_at": "9999-12-31T23:59:59.999Z", 
                    "token_type": "bearer",
                    "refresh_token": "zerofn_refresh",
                    "refresh_expires": 28800,
                    "refresh_expires_at": "9999-12-31T23:59:59.999Z",
                    "account_id": "ZeroFN",
                    "client_id": "ZeroFN",
                    "internal_client": True,
                    "client_service": "fortnite",
                    "displayName": "ZeroFN",
                    "app": "fortnite",
                    "in_app_id": "ZeroFN",
                    "device_id": "zerofn_device"
                }
                self.send_json_response(response)
                
            elif self.path == "/fortnite/api/game/v2/profile/client/QueryProfile":
                self.log_connection(client_ip, "Profile query")
                response = {
                    "profileId": data.get("profileId", "athena"),
                    "profileChanges": [{
                        "_type": "fullProfileUpdate",
                        "profile": {
                            "_id": "ZeroFN",
                            "accountId": "ZeroFN", 
                            "profileId": data.get("profileId", "athena"),
                            "version": "no_version",
                            "items": {},
                            "stats": {
                                "attributes": {
                                    "past_seasons": [],
                                    "season_match_boost": 999999,
                                    "loadouts": ["ZeroFN"],
                                    "mfa_reward_claimed": True,
                                    "rested_xp_overflow": 999999,
                                    "quest_manager": {},
                                    "book_level": 999,
                                    "season_num": 1,
                                    "season_update": 0,
                                    "book_xp": 999999,
                                    "permissions": []
                                }
                            },
                            "commandRevision": 0
                        }
                    }],
                    "profileCommandRevision": 0,
                    "serverTime": "2023-01-01T00:00:00.000Z",
                    "responseVersion": 1
                }
                self.send_json_response(response)
                
            elif self.path == "/fortnite/api/game/v2/matchmaking/account":
                self.log_connection(client_ip, "Matchmaking request")
                response = {
                    "accountId": "ZeroFN",
                    "matches": [],
                    "startTime": time.strftime("%Y-%m-%dT%H:%M:%S.000Z"),
                    "endTime": time.strftime("%Y-%m-%dT%H:%M:%S.000Z")
                }
                self.send_json_response(response)
                
            elif self.path == "/fortnite/api/game/v2/profile/ZeroFN/client/SetCosmeticLockerSlot":
                self.log_connection(client_ip, "Cosmetic slot update")
                response = {"profileRevision": 1, "profileId": "athena", "profileChangesBaseRevision": 1, "profileChanges": [], "profileCommandRevision": 1, "serverTime": time.strftime("%Y-%m-%dT%H:%M:%S.000Z"), "responseVersion": 1}
                self.send_json_response(response)
                
            elif self.path == "/fortnite/api/game/v2/profile/ZeroFN/client/SetCosmeticLockerBanner":
                self.log_connection(client_ip, "Banner update")
                response = {"profileRevision": 1, "profileId": "athena", "profileChangesBaseRevision": 1, "profileChanges": [], "profileCommandRevision": 1, "serverTime": time.strftime("%Y-%m-%dT%H:%M:%S.000Z"), "responseVersion": 1}
                self.send_json_response(response)
                
            elif self.path == "/fortnite/api/game/v2/profile/ZeroFN/client/EquipBattleRoyaleCustomization":
                self.log_connection(client_ip, "BR customization update")
                response = {"profileRevision": 1, "profileId": "athena", "profileChangesBaseRevision": 1, "profileChanges": [], "profileCommandRevision": 1, "serverTime": time.strftime("%Y-%m-%dT%H:%M:%S.000Z"), "responseVersion": 1}
                self.send_json_response(response)
                
            else:
                self.log_connection(client_ip, f"Generic POST to {self.path}")
                response = {
                    "status": "ok",
                    "path": self.path,
                    "client_info": {
                        "ip": client_ip,
                        "last_activity": FortniteServerHandler.last_activity.get(client_ip)
                    }
                }
                self.send_json_response(response)
            
        except ConnectionAbortedError:
            FortniteServerHandler.clients.remove(client_ip)
            FortniteServerHandler.connection_count -= 1
            self.log_connection(client_ip, "Client disconnected")
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
        
    def start(self):
        try:
            test_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            test_sock.settimeout(1)
            result = test_sock.connect_ex((self.host, self.port))
            test_sock.close()
            
            if result == 0:
                subprocess.run(f"netstat -ano | findstr :{self.port}", shell=True)
                subprocess.run(f"taskkill /F /PID $(netstat -ano | findstr :{self.port} | awk '{{print $5}}')", shell=True)
                time.sleep(1)
            
            self.server = HTTPServer((self.host, self.port), FortniteServerHandler)
            self.running = True
            
            try:
                requests.get(f"http://{self.host}:{self.port}/", timeout=1)
            except:
                pass
                
            while self.running:
                try:
                    self.server.handle_request()
                except:
                    pass
                
        except Exception as e:
            print(f"[ERROR] Server error: {str(e)}")
            
    def stop(self):
        self.running = False
        if self.server:
            self.server.server_close()
            
    def wait_for_client(self, timeout=30):
        start_time = time.time()
        while time.time() - start_time < timeout:
            if FortniteServerHandler.clients:
                return True
            time.sleep(0.5)
        return False

class ZeroFNApp:
    def __init__(self, root):
        # Check for admin rights
        if not self.is_admin():
            self.restart_as_admin()
            
        self.root = root
        self.root.title("ProjectZERO Launcher")
        self.root.geometry("1000x800")
        self.root.configure(bg=FortniteTheme.BG_COLOR)
        
        # Configure Fortnite theme
        FortniteTheme.configure_styles()
        
        # Variables
        self.fortnite_path = tk.StringVar()
        self.selected_season = tk.StringVar(value="Season 1 (Chapter 1)")
        self.server_process = None
        self.game_process = None
        self.server_window = None
        
        # Season download URLs
        self.season_urls = {
            "Season 1 (Chapter 1)": "https://public.simplyblk.xyz/1.11.zip",
            "Season 4 (Chapter 1)": "https://public.simplyblk.xyz/4.0.zip",
            "Season 7 (Chapter 1)": "https://public.simplyblk.xyz/7.30.zip"
        }
        
        # Initialize server
        self.server = ZeroFNServer()
        
        # Configure logging
        self.setup_logging()
        
        self.create_gui()
        
    def is_admin(self):
        try:
            return ctypes.windll.shell32.IsUserAnAdmin()
        except:
            return False
            
    def restart_as_admin(self):
        ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)
        sys.exit()
        
    def setup_logging(self):
        if not os.path.exists('logs'):
            os.makedirs('logs')
            
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('logs/zerofn_launcher.log'),
                logging.StreamHandler()
            ]
        )
        self.logger = logging.getLogger('ZeroFNLauncher')
        
    def create_gui(self):
        # Main container
        main_frame = ttk.Frame(self.root, style='Fortnite.TFrame', padding=30)
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Header section with logo
        header_frame = ttk.Frame(main_frame, style='Fortnite.TFrame')
        header_frame.pack(fill=tk.X, pady=(0,30))
        
        title_label = ttk.Label(
            header_frame,
            text="ProjectZERO",
            font=("Segoe UI", 48, "bold"),
            foreground=FortniteTheme.ACCENT_COLOR,
            style='Fortnite.TLabel'
        )
        title_label.pack()
        
        subtitle = ttk.Label(
            header_frame,
            text="Experience OG Fortnite like never before",
            font=("Segoe UI", 16),
            foreground=FortniteTheme.FG_COLOR,
            style='Fortnite.TLabel'
        )
        subtitle.pack(pady=5)
        
        # Season selection dropdown
        season_frame = ttk.LabelFrame(
            main_frame,
            text="SELECT SEASON",
            style='Fortnite.TLabelframe',
            padding=20
        )
        season_frame.pack(fill=tk.X, pady=20)
        
        season_combo = ttk.Combobox(
            season_frame,
            textvariable=self.selected_season,
            values=list(self.season_urls.keys()),
            state="readonly",
            style='Fortnite.TCombobox',
            width=40
        )
        season_combo.pack()
        
        # Path selection with modern styling
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
        
        # Action buttons with Fortnite styling
        btn_frame = ttk.Frame(main_frame, style='Fortnite.TFrame')
        btn_frame.pack(pady=30)
        
        for btn_text, btn_cmd, btn_width in [
            ("INSTALL SELECTED SEASON", self.install_fortnite_og, 25),
            ("START HYBRID MODE", self.start_hybrid_mode, 25),
            ("JOIN DISCORD", lambda: webbrowser.open('https://discord.gg/yCY4FTMPdK'), 25)
        ]:
            btn = ttk.Button(
                btn_frame,
                text=btn_text,
                command=btn_cmd,
                style='Fortnite.TButton',
                width=btn_width
            )
            btn.pack(pady=10)
        
        # Status console with custom styling
        status_frame = ttk.LabelFrame(
            main_frame,
            text="CONSOLE OUTPUT",
            style='Fortnite.TLabelframe',
            padding=15
        )
        status_frame.pack(fill=tk.BOTH, expand=True, pady=(20,0))
        
        # Scrollbar styling
        scrollbar = ttk.Scrollbar(status_frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        self.status_text = tk.Text(
            status_frame,
            height=15,
            bg=FortniteTheme.SECONDARY_COLOR,
            fg=FortniteTheme.FG_COLOR,
            font=("Consolas", 10),
            yscrollcommand=scrollbar.set,
            padx=10,
            pady=10
        )
        self.status_text.pack(fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.status_text.yview)
        
        # Footer
        footer = ttk.Label(
            main_frame,
            text="Created by root404 and the ZeroFN team",
            font=("Segoe UI", 9),
            foreground=FortniteTheme.FG_COLOR,
            style='Fortnite.TLabel'
        )
        footer.pack(pady=(20,0))
        
    def log_status(self, message):
        timestamp = time.strftime("%H:%M:%S")
        self.status_text.insert(tk.END, f"[{timestamp}] {message}\n")
        self.status_text.see(tk.END)
        self.logger.info(message)
        
    def browse_fortnite(self):
        path = filedialog.askdirectory(title="Select Fortnite Directory")
        if path:
            self.fortnite_path.set(path)
            self.log_status(f"Selected Fortnite path: {path}")
            
    def install_fortnite_og(self):
        season = self.selected_season.get()
        season_dir = f"FortniteOG_{season.split()[1]}"
        install_dir = Path(season_dir)
        download_url = self.season_urls[season]
        
        def download():
            try:
                if install_dir.exists():
                    self.log_status(f"{season} directory already exists. Verifying files...")
                    self.fortnite_path.set(str(install_dir.absolute()))
                    return
                    
                self.log_status(f"Starting {season} download...")
                response = requests.get(download_url, stream=True)
                total_size = int(response.headers.get('content-length', 0))
                block_size = 8192
                downloaded = 0
                
                zip_file = f"{season.split()[1]}.zip"
                with open(zip_file, 'wb') as f:
                    for chunk in response.iter_content(chunk_size=block_size):
                        if chunk:
                            f.write(chunk)
                            downloaded += len(chunk)
                            progress = int((downloaded / total_size) * 100)
                            if progress % 10 == 0:
                                self.log_status(f"Download progress: {progress}%")
                                
                self.log_status("Download complete! Extracting files...")
                shutil.unpack_archive(zip_file, season_dir)
                os.remove(zip_file)
                
                self.log_status("Installation completed successfully!")
                self.fortnite_path.set(str(install_dir.absolute()))
                
            except Exception as e:
                self.log_status(f"Error during installation: {str(e)}")
                messagebox.showerror("Installation Error", str(e))
                
        threading.Thread(target=download, daemon=True).start()
        
    def start_hybrid_mode(self):
        if not self.fortnite_path.get():
            messagebox.showerror("Error", "Please select Fortnite path first!")
            return

        # Start server in a separate thread
        server_thread = threading.Thread(target=self.server.start, daemon=True)
        server_thread.start()
        self.log_status("ZeroFN server started on 127.0.0.1:7777")

        # Wait briefly for server to start and establish connection
        time.sleep(1)
        try:
            requests.get("http://127.0.0.1:7777/", timeout=2)
            self.log_status("Server connection verified!")
        except:
            self.log_status("Warning: Could not verify server connection")

        # Clean up existing processes
        self.log_status("Cleaning up existing processes...")
        processes = [
            "FortniteClient-Win64-Shipping.exe",
            "EasyAntiCheat.exe",
            "BEService.exe",
            "FortniteLauncher.exe",
            "EpicGamesLauncher.exe"
        ]
        
        for proc in processes:
            try:
                subprocess.run(["taskkill", "/f", "/im", proc], 
                             stdout=subprocess.DEVNULL,
                             stderr=subprocess.DEVNULL,
                             check=False)
            except:
                pass
        
        try:
            # Set compatibility flags with admin rights
            game_exe = Path(self.fortnite_path.get()) / "FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe"
            
            if not game_exe.exists():
                raise FileNotFoundError("Fortnite executable not found!")
            
            self.log_status("Setting compatibility flags...")
            try:
                subprocess.run([
                    "reg", "add",
                    "HKCU\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers",
                    "/v", str(game_exe),
                    "/t", "REG_SZ",
                    "/d", "~ RUNASADMIN DISABLEDXMAXIMIZEDWINDOWEDMODE DISABLETHEMES",
                    "/f"
                ], check=True, capture_output=True)
            except subprocess.CalledProcessError:
                self.log_status("Warning: Could not set compatibility flags")
            
            # Launch game with admin rights
            self.log_status("Launching Fortnite...")
            
            # Change working directory to game executable location
            os.chdir(str(game_exe.parent))
            
            launch_args = [
                str(game_exe),
                "-NOSPLASH",
                "-USEALLAVAILABLECORES",
                "-dx11",
                "-AUTH_TYPE=epic",
                "-AUTH_LOGIN=ZeroFN@zerofn.com",
                "-AUTH_PASSWORD=zerofn",
                "-epicapp=Fortnite",
                "-epicenv=Prod",
                "-epiclocale=en-us",
                "-epicportal",
                "-noeac",
                "-nobe",
                "-fromfl=be",
                "-fltoken=",
                "-nolog",
                "-NOSSLPINNING",
                "-preferredregion=NAE",
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
            
            # Launch game with proper argument formatting
            launch_cmd = " ".join(f'"{arg}"' if " " in arg else arg for arg in launch_args)
            self.game_process = subprocess.Popen(
                launch_cmd,
                shell=True,
                creationflags=subprocess.CREATE_NEW_CONSOLE
            )
            
            self.log_status("Game launched successfully!")
            
            # Wait for client connection
            if self.server.wait_for_client(timeout=30):
                self.log_status("Client connected to server successfully!")
            else:
                self.log_status("Warning: Client connection timeout")
            
        except Exception as e:
            self.log_status(f"Error starting hybrid mode: {str(e)}")
            messagebox.showerror("Launch Error", str(e))
            self.server.stop()
        
    def __del__(self):
        # Cleanup on exit
        if hasattr(self, 'server'):
            self.server.stop()
            
        if hasattr(self, 'game_process') and self.game_process:
            try:
                self.game_process.terminate()
            except:
                pass

if __name__ == "__main__":
    root = tk.Tk()
    app = ZeroFNApp(root)
    root.mainloop()

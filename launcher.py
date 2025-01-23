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
import zipfile
from PIL import Image, ImageTk

# Modern Epic Games theme with glassmorphism effects
class EpicTheme:
    BG_COLOR = '#0b0d17' 
    FG_COLOR = '#ffffff'
    ACCENT_COLOR = '#000000'  # Changed to black
    SECONDARY_COLOR = '#151823'
    HIGHLIGHT_COLOR = '#333333'  # Darker highlight for black buttons
    BORDER_RADIUS = 25  # Increased border radius
    GLASS_ALPHA = 0.95

    @staticmethod
    def configure_styles():
        style = ttk.Style()
        
        # Configure main styles with glassmorphism
        style.configure('Epic.TFrame',
                       background=EpicTheme.BG_COLOR,
                       relief='flat')
        
        style.configure('Epic.TLabel',
                       background=EpicTheme.BG_COLOR,
                       foreground=EpicTheme.FG_COLOR,
                       font=("Segoe UI Semibold", 10))
        
        # Modern gradient button style with more padding
        style.configure('Epic.TButton',
                       background=EpicTheme.ACCENT_COLOR,
                       foreground=EpicTheme.FG_COLOR,
                       font=("Segoe UI", 10, "bold"),
                       padding=20,  # Increased padding
                       relief="flat",
                       borderwidth=0)
        
        style.map('Epic.TButton',
                 background=[('active', EpicTheme.HIGHLIGHT_COLOR)],
                 foreground=[('active', EpicTheme.FG_COLOR)])

        # Glass panel frame styles with more padding
        style.configure('Epic.TLabelframe',
                       background=EpicTheme.SECONDARY_COLOR,
                       foreground=EpicTheme.FG_COLOR,
                       relief="solid",
                       borderwidth=1,
                       padding=25,  # Added padding
                       bordercolor=EpicTheme.ACCENT_COLOR)
        
        style.configure('Epic.TLabelframe.Label',
                       background=EpicTheme.SECONDARY_COLOR,
                       foreground=EpicTheme.ACCENT_COLOR,
                       font=("Segoe UI", 12, "bold"),
                       padding=10)  # Added padding

        # Modern floating combobox style
        style.configure('Epic.TCombobox',
                       background=EpicTheme.SECONDARY_COLOR,
                       foreground=EpicTheme.FG_COLOR,
                       fieldbackground=EpicTheme.SECONDARY_COLOR,
                       arrowcolor=EpicTheme.ACCENT_COLOR,
                       relief="flat",
                       borderwidth=0,
                       padding=15)  # Added padding

        # Custom styles for different button types
        style.configure('Primary.Epic.TButton',
                       background=EpicTheme.ACCENT_COLOR,
                       foreground=EpicTheme.FG_COLOR,
                       padding=20)  # Added padding

        style.configure('Secondary.Epic.TButton',
                       background=EpicTheme.ACCENT_COLOR,  # Changed to black
                       foreground=EpicTheme.FG_COLOR,
                       padding=20)  # Added padding

        style.configure('Success.Epic.TButton',
                       background=EpicTheme.ACCENT_COLOR,  # Changed to black
                       foreground=EpicTheme.FG_COLOR,
                       padding=20)  # Added padding

class FortniteServerHandler(BaseHTTPRequestHandler):
    clients = set()
    connection_count = 0
    last_activity = {}
    epic_auth_code = None
    auth_tokens = None
    
    def log_connection(self, client_ip, action):
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] {action}: {client_ip}")
        FortniteServerHandler.last_activity[client_ip] = timestamp
    
    def send_json_response(self, data, status=200):
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())

    def do_GET(self):
        try:
            client_ip = self.client_address[0]
            
            if client_ip not in FortniteServerHandler.clients:
                FortniteServerHandler.clients.add(client_ip)
                FortniteServerHandler.connection_count += 1
                self.log_connection(client_ip, "New client connected")
            
            # Handle Epic Games OAuth callback
            if self.path.startswith("/epic/callback"):
                query = urllib.parse.parse_qs(urllib.parse.urlparse(self.path).query)
                FortniteServerHandler.epic_auth_code = query.get('code', [None])[0]
                if FortniteServerHandler.epic_auth_code:
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
                                <div class="success-icon">✓</div>
                                <h2>Login Successful!</h2>
                                <p>You can close this window and return to the launcher.</p>
                            </div>
                        </body>
                    </html>
                    """
                    self.wfile.write(success_html.encode())
                return
            
            # Handle verification endpoint
            if self.path == "/account/api/oauth/verify":
                if not FortniteServerHandler.auth_tokens:
                    self.send_error(401, "Not authenticated")
                    return
                    
                response = {
                    "access_token": FortniteServerHandler.auth_tokens["access_token"],
                    "expires_in": 28800,
                    "expires_at": "9999-12-31T23:59:59.999Z",
                    "token_type": "bearer",
                    "account_id": FortniteServerHandler.auth_tokens["account_id"],
                    "client_id": "xyza7891TydzdNolyGQJYa9b6n6rLMJI",
                    "internal_client": True,
                    "client_service": "fortnite",
                    "displayName": FortniteServerHandler.auth_tokens["display_name"],
                    "app": "fortnite",
                    "in_app_id": FortniteServerHandler.auth_tokens["account_id"]
                }
                self.send_json_response(response)
            elif self.path == "/fortnite/api/game/v2/enabled_features":
                self.send_json_response([])
            elif self.path == "/fortnite/api/cloudstorage/system":
                self.send_json_response([])
            elif self.path.startswith("/fortnite/api/cloudstorage/user/"):
                self.send_json_response([])
            elif self.path == "/waitingroom/api/waitingroom":
                self.send_json_response(None)
            elif self.path == "/lightswitch/api/service/bulk/status":
                self.send_json_response([{
                    "serviceInstanceId": "fortnite",
                    "status": "UP",
                    "message": "Fortnite is online",
                    "maintenanceUri": None,
                    "allowedActions": ["PLAY", "DOWNLOAD"],
                    "banned": False
                }])
            else:
                self.send_json_response({"status": "ok"})

        except Exception as e:
            self.log_connection(client_ip, f"Error: {str(e)}")
            self.send_error(500, str(e))

    def do_POST(self):
        try:
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            
            self.send_json_response({
                "status": "success",
                "authenticated": True
            })
            
        except Exception as e:
            self.send_error(500, str(e))

class PrivateServer:
    def __init__(self, host="127.0.0.1", port=7777):
        self.host = host
        self.port = port
        self.server = None
        self.running = False
        self.client_connected = threading.Event()

    def start(self):
        try:
            self.server = HTTPServer((self.host, self.port), FortniteServerHandler)
            self.running = True
            while self.running:
                self.server.handle_request()
                if FortniteServerHandler.clients:
                    self.client_connected.set()
        except Exception as e:
            print(f"Server error: {str(e)}")

    def stop(self):
        self.running = False
        if self.server:
            self.server.server_close()

    def wait_for_client(self, timeout=30):
        return self.client_connected.wait(timeout)

class LauncherApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Epic Games Launcher")
        self.root.geometry("1280x800")
        self.root.configure(bg=EpicTheme.BG_COLOR)
        self.root.minsize(1024, 768)
        
        # Set window icon
        if os.path.exists("assets/epic_icon.ico"):
            self.root.iconbitmap("assets/epic_icon.ico")

        # Load and apply background image
        if os.path.exists("assets/bg.png"):
            bg_image = Image.open("assets/bg.png")
            bg_image = bg_image.resize((1280, 800), Image.Resampling.LANCZOS)
            self.bg_photo = ImageTk.PhotoImage(bg_image)
            bg_label = tk.Label(root, image=self.bg_photo)
            bg_label.place(x=0, y=0, relwidth=1, relheight=1)

        EpicTheme.configure_styles()

        self.fortnite_path = tk.StringVar()
        self.selected_season = tk.StringVar(value="Chapter 1 Season 1")
        self.server = PrivateServer()
        self.game_process = None

        # Season download URLs with thumbnails
        self.season_urls = {
            "Chapter 1 Season 1": {
                "url": "https://cdn.fnbuilds.services/1.7.2.zip",
                "thumb": "assets/seasons/s1.jpg"
            },
            "Chapter 1 Season 4": {
                "url": "https://cdn.fnbuilds.services/4.5.zip",
                "thumb": "assets/seasons/s4.jpg"
            },
            "Chapter 1 Season 7": {
                "url": "https://cdn.fnbuilds.services/7.40.zip",
                "thumb": "assets/seasons/s7.jpg"
            }
        }

        self.create_gui()
        self.setup_logging()

        # Add window drag functionality
        self.root.bind("<Button-1>", self.start_move)
        self.root.bind("<ButtonRelease-1>", self.stop_move)
        self.root.bind("<B1-Motion>", self.do_move)

    def create_gui(self):
        # Create main container with glass effect
        main_frame = ttk.Frame(self.root, style='Epic.TFrame')
        main_frame.pack(fill=tk.BOTH, expand=True, padx=40, pady=40)

        # Modern header with animated logo
        header_frame = ttk.Frame(main_frame, style='Epic.TFrame')
        header_frame.pack(fill=tk.X, pady=(0, 30))
        
        if os.path.exists("assets/epic_logo_animated.gif"):
            # Add animated logo support here
            pass
        elif os.path.exists("assets/epic_logo.png"):
            logo_img = Image.open("assets/epic_logo.png")
            logo_img = logo_img.resize((48, 48), Image.Resampling.LANCZOS)
            logo_photo = ImageTk.PhotoImage(logo_img)
            logo_label = ttk.Label(header_frame, image=logo_photo, style='Epic.TLabel')
            logo_label.image = logo_photo
            logo_label.pack(side=tk.LEFT)

        title = ttk.Label(header_frame, 
                         text="Fortnite Private Server",
                         font=("Segoe UI", 28, "bold"),
                         style='Epic.TLabel')
        title.pack(side=tk.LEFT, padx=20)

        # Modern path selection with icon
        path_frame = ttk.LabelFrame(main_frame,
                                  text="INSTALLATION DIRECTORY",
                                  style='Epic.TLabelframe',
                                  padding=20)
        path_frame.pack(fill=tk.X, pady=(0, 30))

        path_btn = ttk.Button(path_frame,
                            text="SELECT FORTNITE PATH",
                            command=self.browse_fortnite,
                            style='Primary.Epic.TButton')
        path_btn.pack(side=tk.LEFT, padx=(0, 15))

        path_label = ttk.Label(path_frame,
                             textvariable=self.fortnite_path,
                             style='Epic.TLabel')
        path_label.pack(side=tk.LEFT, fill=tk.X, expand=True)

        # Season selection with thumbnails
        season_frame = ttk.LabelFrame(main_frame,
                                    text="SELECT SEASON",
                                    style='Epic.TLabelframe',
                                    padding=20)
        season_frame.pack(fill=tk.X, pady=(0, 30))

        season_combo = ttk.Combobox(season_frame,
                                  textvariable=self.selected_season,
                                  values=list(self.season_urls.keys()),
                                  state="readonly",
                                  style='Epic.TCombobox',
                                  width=30)
        season_combo.pack(side=tk.LEFT, padx=(0, 15))

        install_btn = ttk.Button(season_frame,
                               text="INSTALL SELECTED SEASON",
                               command=self.install_season,
                               style='Success.Epic.TButton')
        install_btn.pack(side=tk.LEFT)

        # Season preview (thumbnail)
        self.preview_label = ttk.Label(season_frame, style='Epic.TLabel')
        self.preview_label.pack(side=tk.RIGHT, padx=15)
        season_combo.bind('<<ComboboxSelected>>', self.update_preview)

        # Launch options with modern icons
        launch_frame = ttk.LabelFrame(main_frame,
                                    text="LAUNCH OPTIONS",
                                    style='Epic.TLabelframe',
                                    padding=20)
        launch_frame.pack(fill=tk.X, pady=(0, 30))

        hybrid_btn = ttk.Button(launch_frame,
                              text="START HYBRID MODE",
                              command=self.start_hybrid_mode,
                              style='Primary.Epic.TButton')
        hybrid_btn.pack(side=tk.LEFT, padx=(0, 15))

        login_btn = ttk.Button(launch_frame,
                             text="LOGIN TO ACCOUNT",
                             command=lambda: webbrowser.open('https://www.epicgames.com/id/login?redirectUrl=https%3A%2F%2Fwww.epicgames.com%2Fid%2Fapi%2Fredirect%3FclientId%3D3446cd72694c4a4485d81b77adbb2141%26responseType%3Dcode'),
                             style='Primary.Epic.TButton')
        login_btn.pack(side=tk.LEFT, padx=(0, 15))

        discord_btn = ttk.Button(launch_frame,
                               text="JOIN DISCORD",
                               command=lambda: webbrowser.open('https://discord.gg/zerofn'),
                               style='Secondary.Epic.TButton')
        discord_btn.pack(side=tk.LEFT)

        # Modern console with syntax highlighting
        console_frame = ttk.LabelFrame(main_frame,
                                     text="CONSOLE OUTPUT",
                                     style='Epic.TLabelframe',
                                     padding=20)
        console_frame.pack(fill=tk.BOTH, expand=True)

        # Add console toolbar
        console_toolbar = ttk.Frame(console_frame, style='Epic.TFrame')
        console_toolbar.pack(fill=tk.X, pady=(0, 10))

        clear_btn = ttk.Button(console_toolbar,
                             text="Clear Console",
                             command=lambda: self.console.delete(1.0, tk.END),
                             style='Secondary.Epic.TButton')
        clear_btn.pack(side=tk.RIGHT)

        # Console with custom font and colors
        self.console = tk.Text(console_frame,
                             height=15,
                             bg=EpicTheme.SECONDARY_COLOR,
                             fg=EpicTheme.FG_COLOR,
                             font=("JetBrains Mono", 10),
                             relief="flat",
                             padx=15,
                             pady=15,
                             wrap=tk.WORD)
        self.console.pack(fill=tk.BOTH, expand=True)

        # Add scrollbar
        scrollbar = ttk.Scrollbar(console_frame, orient="vertical", command=self.console.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.console.configure(yscrollcommand=scrollbar.set)

        # Modern footer with social links
        footer_frame = ttk.Frame(main_frame, style='Epic.TFrame')
        footer_frame.pack(fill=tk.X, pady=(20, 0))

        credits = ttk.Label(footer_frame,
                          text="Created by root404 and the ZeroFN team",
                          font=("Segoe UI", 9),
                          style='Epic.TLabel')
        credits.pack(side=tk.LEFT)

        # Social media icons/links
        social_frame = ttk.Frame(footer_frame, style='Epic.TFrame')
        social_frame.pack(side=tk.RIGHT)

        for platform in ['twitter', 'github', 'discord']:
            if os.path.exists(f"assets/{platform}_icon.png"):
                icon = Image.open(f"assets/{platform}_icon.png")
                icon = icon.resize((24, 24), Image.Resampling.LANCZOS)
                icon_photo = ImageTk.PhotoImage(icon)
                icon_btn = ttk.Label(social_frame, image=icon_photo, style='Epic.TLabel')
                icon_btn.image = icon_photo
                icon_btn.pack(side=tk.LEFT, padx=5)

    def update_preview(self, event=None):
        season = self.selected_season.get()
        thumb_path = self.season_urls[season]["thumb"]
        if os.path.exists(thumb_path):
            thumb_img = Image.open(thumb_path)
            thumb_img = thumb_img.resize((160, 90), Image.Resampling.LANCZOS)
            thumb_photo = ImageTk.PhotoImage(thumb_img)
            self.preview_label.configure(image=thumb_photo)
            self.preview_label.image = thumb_photo

    def start_move(self, event):
        self.x = event.x
        self.y = event.y

    def stop_move(self, event):
        self.x = None
        self.y = None

    def do_move(self, event):
        deltax = event.x - self.x
        deltay = event.y - self.y
        x = self.root.winfo_x() + deltax
        y = self.root.winfo_y() + deltay
        self.root.geometry(f"+{x}+{y}")

    def setup_logging(self):
        if not os.path.exists('logs'):
            os.makedirs('logs')
            
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('logs/launcher.log'),
                logging.StreamHandler()
            ]
        )
        self.logger = logging.getLogger('Launcher')

    def browse_fortnite(self):
        path = filedialog.askdirectory(title="Select Fortnite Directory")
        if path:
            self.fortnite_path.set(path)
            self.log_message(f"Selected Fortnite path: {path}")

    def install_season(self):
        season = self.selected_season.get()
        url = self.season_urls[season]["url"]
        install_path = self.fortnite_path.get()
        
        if not install_path:
            messagebox.showerror("Error", "Please select installation directory first!")
            return
            
        def download():
            try:
                self.log_message(f"Starting download of {season}...")
                
                temp_dir = Path("temp")
                temp_dir.mkdir(exist_ok=True)
                
                zip_path = temp_dir / f"{season.replace(' ', '_')}.zip"
                response = requests.get(url, stream=True)
                total_size = int(response.headers.get('content-length', 0))
                
                with open(zip_path, 'wb') as f:
                    downloaded = 0
                    for chunk in response.iter_content(chunk_size=8192):
                        if chunk:
                            f.write(chunk)
                            downloaded += len(chunk)
                            progress = int((downloaded / total_size) * 100)
                            self.log_message(f"Download progress: {progress}%")
                            
                self.log_message("Download complete, extracting files...")
                
                with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                    zip_ref.extractall(install_path)
                    
                server_files = Path("server_files") / season.replace(" ", "_")
                if server_files.exists():
                    shutil.copytree(server_files, install_path / "ServerFiles", dirs_exist_ok=True)
                
                shutil.rmtree(temp_dir)
                
                self.log_message(f"{season} installed successfully!")
                
            except Exception as e:
                self.log_message(f"Installation error: {str(e)}")
                messagebox.showerror("Error", str(e))
                
        threading.Thread(target=download, daemon=True).start()

    def start_hybrid_mode(self):
        if not self.fortnite_path.get():
            messagebox.showerror("Error", "Please select Fortnite path first!")
            return
            
        try:
            # Start private server
            self.log_message("Starting private server...")
            server_thread = threading.Thread(target=self.server.start, daemon=True)
            server_thread.start()
            
            # Launch game
            game_exe = Path(self.fortnite_path.get()) / "FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe"
            
            if not game_exe.exists():
                raise FileNotFoundError("Fortnite executable not found!")

            launch_args = [
                str(game_exe),
                "-NOSPLASH",
                "-USEALLAVAILABLECORES",
                "-dx11",
                "-AUTH_TYPE=epic",
                "-AUTH_LOGIN=unused",
                "-AUTH_PASSWORD=unused", 
                "-epicapp=Fortnite",
                "-epicenv=Prod",
                "-epiclocale=en-us",
                "-epicportal",
                "-noeac",
                "-fromfl=be",
                "-fltoken=none",
                "-skippatchcheck",
                "-HTTP=127.0.0.1:7777",
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

            self.game_process = subprocess.Popen(launch_args)
            self.log_message("Game launched successfully!")
            
            # Wait for client connection
            if self.server.wait_for_client(timeout=30):
                self.log_message("Client connected to server successfully!")
            else:
                self.log_message("Warning: Client connection timeout")
            
        except Exception as e:
            self.log_message(f"Error starting hybrid mode: {str(e)}")
            messagebox.showerror("Error", str(e))
            self.server.stop()

    def log_message(self, message):
        timestamp = time.strftime("%H:%M:%S")
        self.console.insert(tk.END, f"[{timestamp}] {message}\n")
        self.console.see(tk.END)
        self.logger.info(message)

    def __del__(self):
        if hasattr(self, 'server'):
            self.server.stop()
            
        if hasattr(self, 'game_process') and self.game_process:
            try:
                self.game_process.terminate()
            except:
                pass

if __name__ == "__main__":
    root = tk.Tk()
    app = LauncherApp(root)
    root.mainloop()

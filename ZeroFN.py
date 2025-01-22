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

# Custom HTTP request handler for Fortnite server
class FortniteServerHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        
        response = {
            "status": "ok",
            "message": "ZeroFN server running"
        }
        
        self.wfile.write(json.dumps(response).encode())
        
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        
        try:
            data = json.loads(post_data.decode())
            
            # Handle different endpoints
            if self.path == "/account/api/oauth/token":
                response = {
                    "access_token": str(uuid.uuid4()),
                    "expires_in": 28800,
                    "token_type": "bearer",
                    "account_id": str(uuid.uuid4()),
                    "client_id": "ZeroFN",
                    "internal_client": True,
                    "client_service": "fortnite"
                }
                
            elif self.path == "/fortnite/api/game/v2/profile/client/QueryProfile":
                response = {
                    "profileId": "athena",
                    "profileChanges": [],
                    "profileCommandRevision": 1,
                    "serverTime": time.strftime("%Y-%m-%dT%H:%M:%S.000Z"),
                    "responseVersion": 1
                }
                
            else:
                response = {
                    "status": "ok",
                    "path": self.path
                }
                
            self.send_response(200)
            self.send_header('Content-Type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps(response).encode())
            
        except Exception as e:
            self.send_error(500, str(e))
            
    def log_message(self, format, *args):
        # Suppress default logging
        pass

class ZeroFNServer:
    def __init__(self, host="127.0.0.1", port=7777):
        self.host = host
        self.port = port
        self.server = None
        self.running = False
        
    def start(self):
        try:
            self.server = HTTPServer((self.host, self.port), FortniteServerHandler)
            self.running = True
            print(f"[INFO] ZeroFN server started on {self.host}:{self.port}")
            while self.running:
                self.server.handle_request()
        except Exception as e:
            print(f"[ERROR] Server error: {str(e)}")
            
    def stop(self):
        self.running = False
        if self.server:
            self.server.server_close()
            print("[INFO] ZeroFN server stopped")

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
        self.server_process = None
        self.game_process = None
        self.server_window = None
        
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
            ("INSTALL FORTNITE OG", self.install_fortnite_og, 25),
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
        install_dir = Path("FortniteOG")
        download_url = "https://public.simplyblk.xyz/1.11.zip"
        
        def download():
            try:
                if install_dir.exists():
                    self.log_status("FortniteOG directory already exists. Verifying files...")
                    self.fortnite_path.set(str(install_dir.absolute()))
                    return
                    
                self.log_status("Starting Fortnite OG download...")
                response = requests.get(download_url, stream=True)
                total_size = int(response.headers.get('content-length', 0))
                block_size = 8192
                downloaded = 0
                
                with open("1.11.zip", 'wb') as f:
                    for chunk in response.iter_content(chunk_size=block_size):
                        if chunk:
                            f.write(chunk)
                            downloaded += len(chunk)
                            progress = int((downloaded / total_size) * 100)
                            if progress % 10 == 0:
                                self.log_status(f"Download progress: {progress}%")
                                
                self.log_status("Download complete! Extracting files...")
                shutil.unpack_archive("1.11.zip", "FortniteOG")
                os.remove("1.11.zip")
                
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

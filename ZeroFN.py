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
from datetime import datetime, timezone
from server import FortniteServer
from auth import AuthServer

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
        
        # Initialize servers
        self.fortnite_server = FortniteServer()
        self.auth_server = AuthServer()
        
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
            # Start servers
            fortnite_thread = threading.Thread(target=self.fortnite_server.start, daemon=True)
            auth_thread = threading.Thread(target=self.auth_server.start, daemon=True)
            
            fortnite_thread.start()
            auth_thread.start()
            
            self.log("Fortnite server started on 127.0.0.1:7777")
            self.log("Auth server started on 127.0.0.1:7878") 
            
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
                "-AUTH_HOST=127.0.0.1:7878",
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
            self.fortnite_server.stop()
            self.auth_server.stop()
            
    def __del__(self):
        if hasattr(self, 'fortnite_server'):
            self.fortnite_server.stop()
        if hasattr(self, 'auth_server'):
            self.auth_server.stop()

if __name__ == "__main__":
    root = tk.Tk()
    app = ZeroFNApp(root)
    root.mainloop()

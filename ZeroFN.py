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

# Configure dark theme styles
class DarkTheme:
    BG_COLOR = '#1a1a1a'
    FG_COLOR = '#ffffff' 
    ACCENT_COLOR = '#404040'
    HIGHLIGHT_COLOR = '#0066cc'

    @staticmethod
    def configure_styles():
        style = ttk.Style()
        style.configure('TFrame', background=DarkTheme.BG_COLOR)
        style.configure('TLabel', 
                       background=DarkTheme.BG_COLOR,
                       foreground=DarkTheme.FG_COLOR)
        style.configure('TButton',
                       background=DarkTheme.ACCENT_COLOR,
                       foreground=DarkTheme.FG_COLOR,
                       padding=5)
        style.configure('TLabelframe', 
                       background=DarkTheme.BG_COLOR,
                       foreground=DarkTheme.FG_COLOR)
        style.configure('TLabelframe.Label',
                       background=DarkTheme.BG_COLOR,
                       foreground=DarkTheme.FG_COLOR)
        style.configure('TEntry',
                       fieldbackground=DarkTheme.ACCENT_COLOR,
                       foreground=DarkTheme.FG_COLOR)

class ZeroFNApp:
    def __init__(self, root):
        self.root = root
        self.root.title("ZeroFN Launcher")
        self.root.geometry("900x700")
        self.root.configure(bg=DarkTheme.BG_COLOR)
        
        # Configure dark theme
        DarkTheme.configure_styles()
        
        # Variables
        self.fortnite_path = tk.StringVar()
        self.server_process = None
        self.game_process = None
        self.server_window = None
        
        # Configure logging
        self.setup_logging()
        
        self.create_gui()
        
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
        # Main frame with padding
        main_frame = ttk.Frame(self.root, padding="20")
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Title with custom font and color
        title_label = ttk.Label(
            main_frame, 
            text="ProjectZERO",
            font=("Segoe UI", 32, "bold"),
            foreground=DarkTheme.HIGHLIGHT_COLOR
        )
        title_label.pack(pady=20)
        
        # Subtitle
        subtitle_label = ttk.Label(
            main_frame,
            text="Created by root404 and the ZeroFN team",
            font=("Segoe UI", 14)
        )
        subtitle_label.pack(pady=5)
        
        # Path selection frame
        path_frame = ttk.LabelFrame(main_frame, text="Fortnite Path", padding=15)
        path_frame.pack(fill=tk.X, pady=15)
        
        path_entry = ttk.Entry(
            path_frame, 
            textvariable=self.fortnite_path,
            font=("Segoe UI", 10)
        )
        path_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0,10))
        
        browse_btn = ttk.Button(
            path_frame,
            text="Browse",
            command=self.browse_fortnite,
            style='Accent.TButton'
        )
        browse_btn.pack(side=tk.RIGHT)
        
        # Action buttons frame
        btn_frame = ttk.Frame(main_frame)
        btn_frame.pack(pady=20)
        
        # Custom button styles
        for btn_text, btn_cmd in [
            ("Install Fortnite OG", self.install_fortnite_og),
            ("Start Hybrid Mode", self.start_hybrid_mode),
            ("Join Discord", lambda: webbrowser.open('https://discord.gg/yCY4FTMPdK'))
        ]:
            btn = ttk.Button(
                btn_frame,
                text=btn_text,
                command=btn_cmd,
                style='Accent.TButton'
            )
            btn.pack(pady=8, ipadx=20, ipady=5)
        
        # Status frame with scrollbar
        self.status_frame = ttk.LabelFrame(main_frame, text="Status", padding=15)
        self.status_frame.pack(fill=tk.BOTH, expand=True, pady=15)
        
        # Add scrollbar
        scrollbar = ttk.Scrollbar(self.status_frame)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        self.status_text = tk.Text(
            self.status_frame, 
            height=12,
            bg=DarkTheme.ACCENT_COLOR,
            fg=DarkTheme.FG_COLOR,
            font=("Consolas", 10),
            yscrollcommand=scrollbar.set
        )
        self.status_text.pack(fill=tk.BOTH, expand=True)
        scrollbar.config(command=self.status_text.yview)
        
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
            
        def start_server():
            try:
                # Create server window
                self.server_window = subprocess.Popen(
                    ['cmd', '/c', 'start', 'cmd', '/k', 
                     f'title ZeroFN Server && python server.py'],
                    creationflags=subprocess.CREATE_NEW_CONSOLE
                )
                
                self.log_status("ZeroFN server started in new window")
                time.sleep(2)  # Wait for server to initialize
                
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
                    subprocess.run(["taskkill", "/f", "/im", proc], 
                                 stdout=subprocess.DEVNULL,
                                 stderr=subprocess.DEVNULL)
                
                # Set compatibility flags
                game_exe = Path(self.fortnite_path.get()) / "FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe"
                
                if not game_exe.exists():
                    raise FileNotFoundError("Fortnite executable not found!")
                
                self.log_status("Setting compatibility flags...")
                subprocess.run([
                    "reg", "add",
                    "HKCU\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers",
                    "/v", str(game_exe),
                    "/t", "REG_SZ",
                    "/d", "~ RUNASADMIN DISABLEDXMAXIMIZEDWINDOWEDMODE DISABLETHEMES",
                    "/f"
                ], stdout=subprocess.DEVNULL)
                
                # Launch game
                self.log_status("Launching Fortnite...")
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
                    "-noeac", "-nobe", "-fromfl=be", "-fltoken=",
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
                    "-FORCECLIENT_HOST=127.0.0.1:7777"
                ]
                
                self.game_process = subprocess.Popen(launch_args)
                self.log_status("Game launched successfully!")
                self.log_status("Server logs available in separate window")
                
            except Exception as e:
                self.log_status(f"Error starting hybrid mode: {str(e)}")
                messagebox.showerror("Launch Error", str(e))
                
        threading.Thread(target=start_server, daemon=True).start()
        
    def __del__(self):
        # Cleanup on exit
        if self.server_process:
            self.server_process.terminate()
        if self.game_process:
            self.game_process.terminate()
        if self.server_window:
            subprocess.run(["taskkill", "/F", "/T", "/PID", str(self.server_window.pid)],
                         stdout=subprocess.DEVNULL,
                         stderr=subprocess.DEVNULL)

if __name__ == "__main__":
    root = tk.Tk()
    app = ZeroFNApp(root)
    root.mainloop()

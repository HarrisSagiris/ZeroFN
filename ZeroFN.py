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
            
        def start_server():
            try:
                # Create server window with admin rights
                self.server_window = subprocess.Popen(
                    ['cmd', '/c', 'start', 'cmd', '/k',
                     f'title ZeroFN Server && python server.py'],
                    creationflags=subprocess.CREATE_NEW_CONSOLE | subprocess.DETACHED_PROCESS
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
                
                # Set compatibility flags with admin rights
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
                
                # Launch game with admin rights
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

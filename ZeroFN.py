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

class ZeroFNApp:
    def __init__(self, root):
        self.root = root
        self.root.title("ZeroFN Launcher")
        self.root.geometry("800x600")
        self.root.configure(bg='#1a1a1a')
        
        # Variables
        self.fortnite_path = tk.StringVar()
        self.server_process = None
        self.game_process = None
        
        self.create_gui()
        
    def create_gui(self):
        # Main frame
        main_frame = ttk.Frame(self.root, padding="20")
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Title
        title_label = ttk.Label(
            main_frame, 
            text="ProjectZERO",
            font=("Arial", 24, "bold")
        )
        title_label.pack(pady=20)
        
        # Subtitle
        subtitle_label = ttk.Label(
            main_frame,
            text="Created by root404 and the ZeroFN team",
            font=("Arial", 12)
        )
        subtitle_label.pack(pady=5)
        
        # Path selection frame
        path_frame = ttk.LabelFrame(main_frame, text="Fortnite Path", padding=10)
        path_frame.pack(fill=tk.X, pady=10)
        
        path_entry = ttk.Entry(path_frame, textvariable=self.fortnite_path)
        path_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0,5))
        
        browse_btn = ttk.Button(
            path_frame,
            text="Browse",
            command=self.browse_fortnite
        )
        browse_btn.pack(side=tk.RIGHT)
        
        # Buttons frame
        btn_frame = ttk.Frame(main_frame)
        btn_frame.pack(pady=20)
        
        install_btn = ttk.Button(
            btn_frame,
            text="Install Fortnite OG",
            command=self.install_fortnite_og
        )
        install_btn.pack(pady=5)
        
        start_btn = ttk.Button(
            btn_frame, 
            text="Start Hybrid Mode",
            command=self.start_hybrid_mode
        )
        start_btn.pack(pady=5)
        
        discord_btn = ttk.Button(
            btn_frame,
            text="Join Discord",
            command=lambda: webbrowser.open('https://discord.gg/yCY4FTMPdK')
        )
        discord_btn.pack(pady=5)
        
        # Status frame
        self.status_frame = ttk.LabelFrame(main_frame, text="Status", padding=10)
        self.status_frame.pack(fill=tk.BOTH, expand=True, pady=10)
        
        self.status_text = tk.Text(self.status_frame, height=10)
        self.status_text.pack(fill=tk.BOTH, expand=True)
        
    def log_status(self, message):
        self.status_text.insert(tk.END, f"{message}\n")
        self.status_text.see(tk.END)
        
    def browse_fortnite(self):
        path = filedialog.askdirectory(title="Select Fortnite Directory")
        if path:
            self.fortnite_path.set(path)
            
    def install_fortnite_og(self):
        install_dir = Path("FortniteOG")
        download_url = "https://public.simplyblk.xyz/1.11.zip"
        
        def download():
            try:
                if install_dir.exists():
                    self.log_status("FortniteOG directory already exists. Verifying files...")
                    return
                    
                self.log_status("Downloading Fortnite OG...")
                response = requests.get(download_url, stream=True)
                total_size = int(response.headers.get('content-length', 0))
                
                with open("1.11.zip", 'wb') as f:
                    for chunk in response.iter_content(chunk_size=8192):
                        if chunk:
                            f.write(chunk)
                            
                self.log_status("Download complete. Extracting files...")
                shutil.unpack_archive("1.11.zip", "FortniteOG")
                os.remove("1.11.zip")
                
                self.log_status("Installation complete!")
                self.fortnite_path.set(str(install_dir.absolute()))
                
            except Exception as e:
                self.log_status(f"Error during installation: {str(e)}")
                
        threading.Thread(target=download, daemon=True).start()
        
    def start_hybrid_mode(self):
        if not self.fortnite_path.get():
            messagebox.showerror("Error", "Please select Fortnite path first!")
            return
            
        def start_server():
            try:
                self.log_status("Starting ZeroFN server...")
                self.server_process = subprocess.Popen(
                    [sys.executable, "server.py"],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE
                )
                
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
                
                # Launch game
                game_exe = Path(self.fortnite_path.get()) / "FortniteGame/Binaries/Win64/FortniteClient-Win64-Shipping.exe"
                
                if not game_exe.exists():
                    raise FileNotFoundError("Fortnite executable not found!")
                    
                self.log_status("Launching game...")
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
                
            except Exception as e:
                self.log_status(f"Error starting hybrid mode: {str(e)}")
                
        threading.Thread(target=start_server, daemon=True).start()
        
    def __del__(self):
        if self.server_process:
            self.server_process.terminate()
        if self.game_process:
            self.game_process.terminate()

if __name__ == "__main__":
    root = tk.Tk()
    app = ZeroFNApp(root)
    root.mainloop()

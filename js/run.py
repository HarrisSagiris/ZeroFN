import os
import sys
import subprocess
import time
import webbrowser
import json
import requests
from pathlib import Path
import tkinter as tk
from tkinter import ttk, filedialog, scrolledtext, messagebox
import threading

class LauncherGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("ZeroFN Launcher")
        self.root.geometry("800x600")
        self.root.configure(bg='#2C2F33')
        self.root.resizable(False, False)
        
        # Configure styles
        style = ttk.Style()
        style.configure('TFrame', background='#2C2F33')
        style.configure('TLabel', background='#2C2F33', foreground='white')
        style.configure('TButton', padding=10, background='#7289DA')
        
        # Create main frame
        main_frame = ttk.Frame(root, padding="20")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # Title
        title_label = ttk.Label(main_frame, text="ZeroFN Launcher", font=('Helvetica', 24, 'bold'))
        title_label.grid(row=0, column=0, columnspan=2, pady=(0,20))
        
        # Path selection frame
        path_frame = ttk.Frame(main_frame)
        path_frame.grid(row=1, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0,10))
        
        ttk.Label(path_frame, text="Fortnite Installation Path:", font=('Helvetica', 12)).grid(row=0, column=0, sticky=tk.W)
        self.path_var = tk.StringVar()
        self.path_entry = ttk.Entry(path_frame, textvariable=self.path_var, width=60, font=('Helvetica', 10))
        self.path_entry.grid(row=1, column=0, padx=(0,10))
        
        browse_btn = ttk.Button(path_frame, text="Browse", command=self.browse_path, style='Accent.TButton')
        browse_btn.grid(row=1, column=1)
        
        # Status frame
        status_frame = ttk.Frame(main_frame)
        status_frame.grid(row=2, column=0, columnspan=2, pady=20)
        
        self.backend_status = ttk.Label(status_frame, text="⭕ Backend: Not Running", font=('Helvetica', 10))
        self.backend_status.grid(row=0, column=0, padx=10)
        
        self.proxy_status = ttk.Label(status_frame, text="⭕ Proxy: Not Running", font=('Helvetica', 10))
        self.proxy_status.grid(row=0, column=1, padx=10)
        
        # Log area with custom styling
        log_frame = ttk.Frame(main_frame)
        log_frame.grid(row=3, column=0, columnspan=2, pady=(0,20))
        
        ttk.Label(log_frame, text="Launcher Log:", font=('Helvetica', 12)).grid(row=0, column=0, sticky=tk.W)
        
        self.log_area = scrolledtext.ScrolledText(log_frame, height=15, width=80, font=('Consolas', 10))
        self.log_area.grid(row=1, column=0, pady=(5,0))
        self.log_area.configure(bg='#23272A', fg='#FFFFFF')
        
        # Launch button with custom styling
        self.launch_btn = ttk.Button(main_frame, text="Launch Game", command=self.start_launch, style='Accent.TButton')
        self.launch_btn.grid(row=4, column=0, columnspan=2, pady=(0,10))
        
        # Status bar
        self.status_bar = ttk.Label(main_frame, text="Ready", font=('Helvetica', 10))
        self.status_bar.grid(row=5, column=0, columnspan=2, sticky=tk.W)
        
        # Load saved path and start backend
        self.load_saved_path()
        self.start_backend()
        
        # Configure window close handler
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        
    def log(self, message):
        self.log_area.insert(tk.END, f"[{time.strftime('%H:%M:%S')}] {message}\n")
        self.log_area.see(tk.END)
        self.status_bar.config(text=message)
        
    def update_status(self, backend_running=False, proxy_running=False):
        self.backend_status.config(text=f"{'🟢' if backend_running else '⭕'} Backend: {'Running' if backend_running else 'Not Running'}")
        self.proxy_status.config(text=f"{'🟢' if proxy_running else '⭕'} Proxy: {'Running' if proxy_running else 'Not Running'}")
        
    def browse_path(self):
        path = filedialog.askdirectory(title="Select Fortnite Installation Directory")
        if path:
            self.path_var.set(path)
            self.save_path(path)
            self.log(f"Set Fortnite path to: {path}")
            
    def load_saved_path(self):
        settings_file = Path("settings.json")
        if settings_file.exists():
            try:
                with open(settings_file) as f:
                    settings = json.load(f)
                    if "fortnite_path" in settings:
                        self.path_var.set(settings["fortnite_path"])
                        self.log("Loaded saved Fortnite path")
            except:
                self.log("Error loading settings file")
                    
    def save_path(self, path):
        try:
            settings = {"fortnite_path": path}
            with open("settings.json", "w") as f:
                json.dump(settings, f)
            self.log("Saved Fortnite path")
        except:
            self.log("Error saving settings")
            
    def start_backend(self):
        self.log("Starting backend server...")
        node_path = "node.exe" if sys.platform == "win32" else "node"
            
        try:
            self.backend_process = subprocess.Popen(
                [node_path, "app.js"], 
                cwd=os.path.join(os.path.dirname(__file__), "js"),
                creationflags=subprocess.CREATE_NEW_CONSOLE if sys.platform == "win32" else 0
            )
            time.sleep(2)
            if self.check_backend():
                self.log("Backend server started successfully")
                self.update_status(backend_running=True)
            else:
                self.log("Backend server failed to start")
        except Exception as e:
            self.log(f"Error starting backend: {str(e)}")
            
    def check_backend(self):
        try:
            response = requests.get("http://127.0.0.1:7777/lightswitch/api/service/bulk/status", timeout=2)
            return response.status_code == 200
        except:
            return False
            
    def start_proxy(self):
        self.log("Starting proxy server...")
        proxy_path = Path("proxy.py")
        
        try:
            if sys.platform == "win32":
                self.proxy_process = subprocess.Popen(
                    ["mitmdump", "-s", str(proxy_path), "--listen-port", "7777"],
                    creationflags=subprocess.CREATE_NEW_CONSOLE
                )
            else:
                self.proxy_process = subprocess.Popen(
                    ["mitmdump", "-s", str(proxy_path), "--listen-port", "7777"]
                )
            time.sleep(2)
            self.update_status(proxy_running=True)
            self.log("Proxy server started successfully")
        except Exception as e:
            self.log(f"Error starting proxy: {str(e)}")
        
    def launch_fortnite(self, path):
        self.log("Launching Fortnite...")
        
        os.environ["HTTPS_PROXY"] = "http://127.0.0.1:7777"
        
        exe = Path(path) / "FortniteGame" / "Binaries" / "Win64" / "FortniteClient-Win64-Shipping.exe"
        
        if not exe.exists():
            self.log("Error: Fortnite executable not found")
            messagebox.showerror("Error", "Fortnite executable not found at specified path")
            return False
            
        args = [
            str(exe),
            "-epicapp=Fortnite",
            "-epicenv=Prod", 
            "-epiclocale=en-us",
            "-epicportal",
            "-nobe",
            "-fromfl=be",
            "-fltoken=h1cdhchd10150221h130d1",
            "-skippatchcheck"
        ]
        
        try:
            subprocess.Popen(args)
            return True
        except Exception as e:
            self.log(f"Error launching Fortnite: {str(e)}")
            messagebox.showerror("Error", f"Failed to launch Fortnite: {str(e)}")
            return False
        
    def start_launch(self):
        path = self.path_var.get()
        
        if not path or not Path(path).exists():
            self.log("Error: Invalid Fortnite path")
            messagebox.showerror("Error", "Please select a valid Fortnite installation path")
            return
            
        self.launch_btn.config(state="disabled")
        
        def launch_thread():
            self.log("Starting ZeroFN Launcher...")
            
            if not self.check_backend():
                self.log("Backend server not detected - starting it now...")
                self.start_backend()
                time.sleep(2)
                
                if not self.check_backend():
                    self.log("Error: Failed to start backend server")
                    messagebox.showerror("Error", "Failed to start backend server")
                    self.launch_btn.config(state="normal")
                    return
            
            self.start_proxy()
            
            if self.launch_fortnite(path):
                self.log("Launch successful!")
                messagebox.showinfo("Success", "Fortnite launched successfully!")
            else:
                self.log("Launch failed!")
                
            self.launch_btn.config(state="normal")
            
        threading.Thread(target=launch_thread, daemon=True).start()

    def on_closing(self):
        if hasattr(self, 'backend_process'):
            self.backend_process.terminate()
        if hasattr(self, 'proxy_process'):
            self.proxy_process.terminate()
        self.root.destroy()

def main():
    root = tk.Tk()
    app = LauncherGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()

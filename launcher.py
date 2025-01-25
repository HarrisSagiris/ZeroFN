import os
import sys
import json
import subprocess
import webbrowser
import requests
import time
from mitmproxy import ctx, http
import asyncio
import websockets
import threading
import tkinter as tk
from tkinter import ttk, messagebox
from PIL import Image, ImageTk
import customtkinter as ctk

class ZeroFNLauncher:
    def __init__(self):
        self.backend_process = None
        self.proxy_process = None
        self.fortnite_path = None
        self.backend_port = 5595
        self.proxy_port = 7777

        # Setup UI
        self.root = ctk.CTk()
        self.root.title("ZeroFN Launcher")
        self.root.geometry("800x600")
        self.root.resizable(False, False)
        
        # Set dark theme
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")

        # Create main frame
        self.main_frame = ctk.CTkFrame(self.root)
        self.main_frame.pack(fill="both", expand=True, padx=20, pady=20)

        # Header
        self.header_label = ctk.CTkLabel(
            self.main_frame,
            text="ZeroFN Private Server",
            font=("Segoe UI", 24, "bold")
        )
        self.header_label.pack(pady=20)

        # Status frame
        self.status_frame = ctk.CTkFrame(self.main_frame)
        self.status_frame.pack(fill="x", padx=20, pady=10)

        # Status indicators
        self.backend_status = ctk.CTkLabel(
            self.status_frame,
            text="Backend: Stopped",
            font=("Segoe UI", 12)
        )
        self.backend_status.pack(pady=5)

        self.proxy_status = ctk.CTkLabel(
            self.status_frame,
            text="Proxy: Stopped", 
            font=("Segoe UI", 12)
        )
        self.proxy_status.pack(pady=5)

        self.fortnite_status = ctk.CTkLabel(
            self.status_frame,
            text="Fortnite: Not Found",
            font=("Segoe UI", 12)
        )
        self.fortnite_status.pack(pady=5)

        # Launch button
        self.launch_btn = ctk.CTkButton(
            self.main_frame,
            text="Launch ZeroFN",
            command=self.start_server,
            font=("Segoe UI", 14, "bold"),
            height=40
        )
        self.launch_btn.pack(pady=20)

        # Console output
        self.console = ctk.CTkTextbox(
            self.main_frame,
            height=200,
            font=("Consolas", 11)
        )
        self.console.pack(fill="x", padx=20, pady=10)
        
        # Redirect stdout to console
        sys.stdout = self
        
    def write(self, text):
        self.console.insert("end", text)
        self.console.see("end")
        
    def flush(self):
        pass

    def start_backend(self):
        """Start the Node.js backend server"""
        try:
            backend_path = os.path.join(os.path.dirname(__file__), 'js')
            self.backend_process = subprocess.Popen(['node', 'app.js'], cwd=backend_path)
            print("[+] Backend server started on port", self.backend_port)
            self.backend_status.configure(text="Backend: Running")
            time.sleep(2)
        except Exception as e:
            print("[-] Failed to start backend server:", str(e))
            messagebox.showerror("Error", f"Failed to start backend server: {str(e)}")
            return False
        return True

    def start_proxy(self):
        """Start the mitmproxy server"""
        try:
            from mitmproxy.tools.main import mitmdump
            self.proxy_process = subprocess.Popen([
                'mitmdump',
                '-p', str(self.proxy_port),
                '--ssl-insecure',
                '--set', 'block_global=false',
                '--scripts', 'proxy.py'
            ])
            print("[+] Proxy server started on port", self.proxy_port)
            self.proxy_status.configure(text="Proxy: Running")
            time.sleep(2)
        except Exception as e:
            print("[-] Failed to start proxy server:", str(e))
            messagebox.showerror("Error", f"Failed to start proxy server: {str(e)}")
            return False
        return True

    def setup_fortnite(self):
        """Locate and configure Fortnite"""
        possible_paths = [
            "C:\\Program Files\\Epic Games\\Fortnite\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe",
            "D:\\Program Files\\Epic Games\\Fortnite\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe"
        ]

        for path in possible_paths:
            if os.path.exists(path):
                self.fortnite_path = path
                self.fortnite_status.configure(text="Fortnite: Found")
                break

        if not self.fortnite_path:
            print("[-] Fortnite installation not found")
            messagebox.showerror("Error", "Fortnite installation not found")
            return False
        return True

    def launch_fortnite(self):
        """Launch Fortnite with required parameters"""
        try:
            args = [
                self.fortnite_path,
                "-epicapp=Fortnite",
                "-epicenv=Prod",
                "-epiclocale=en-us",
                "-epicportal",
                "-nobe",
                "-fromfl=none",
                "-fltoken=none",
                f"-AUTH_LOGIN=unused",
                f"-AUTH_PASSWORD=unused",
                f"-AUTH_TYPE=epic",
                "-skippatchcheck",
                "-caldera=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJhY2NvdW50X2lkIjoiYmU5ZGE1YzJmYmVhNDQwN2IyZjQwZWJhYWQ4NTlhZDQiLCJnZW5lcmF0ZWQiOjE2Mzg3MTcyNzgsImNhbGRlcmFHdWlkIjoiMzgxMGI4NjMtMmE2NS00NDU3LTliNTgtNGRhYjNiNDgyYTg2IiwiYWNQcm92aWRlciI6IkVhc3lBbnRpQ2hlYXQiLCJub3RlcyI6IiIsImZhbGxiYWNrIjpmYWxzZX0.VAWQB67RTxhiWOxx7DBjnzDnXyyEnX7OljJm-j2d88G_WgwQ9wrE6lwMEHZHjBd1ISJdUO1UVUqkfLdU5nofBQ"
            ]
            subprocess.Popen(args)
            print("[+] Fortnite launched successfully")
            return True
        except Exception as e:
            print("[-] Failed to launch Fortnite:", str(e))
            messagebox.showerror("Error", f"Failed to launch Fortnite: {str(e)}")
            return False

    def start_server(self):
        """Start all services"""
        self.launch_btn.configure(state="disabled")
        print("[*] Starting ZeroFN Private Server...")
        
        if not self.start_backend():
            self.launch_btn.configure(state="normal")
            return
            
        if not self.start_proxy():
            self.launch_btn.configure(state="normal")
            return
            
        if not self.setup_fortnite():
            self.launch_btn.configure(state="normal")
            return
            
        if not self.launch_fortnite():
            self.launch_btn.configure(state="normal")
            return
            
        print("[+] ZeroFN Private Server is running!")
        
    def run(self):
        """Main method to run the launcher"""
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.mainloop()
        
    def on_closing(self):
        """Handle window closing"""
        if messagebox.askokcancel("Quit", "Do you want to quit?"):
            print("\n[*] Shutting down...")
            if self.backend_process:
                self.backend_process.terminate()
            if self.proxy_process:
                self.proxy_process.terminate()
            self.root.destroy()

if __name__ == "__main__":
    launcher = ZeroFNLauncher()
    launcher.run()

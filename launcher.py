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

class ZeroFNLauncher:
    def __init__(self):
        self.backend_process = None
        self.proxy_process = None
        self.fortnite_path = None
        self.backend_port = 5595
        self.proxy_port = 7777

    def start_backend(self):
        """Start the Node.js backend server"""
        try:
            # Assuming the backend is in a 'js' subdirectory
            backend_path = os.path.join(os.path.dirname(__file__), 'js')
            self.backend_process = subprocess.Popen(['node', 'app.js'], cwd=backend_path)
            print("[+] Backend server started on port", self.backend_port)
            time.sleep(2)  # Give the server time to start
        except Exception as e:
            print("[-] Failed to start backend server:", str(e))
            sys.exit(1)

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
            time.sleep(2)  # Give proxy time to start
        except Exception as e:
            print("[-] Failed to start proxy server:", str(e))
            sys.exit(1)

    def setup_fortnite(self):
        """Locate and configure Fortnite"""
        # Default Fortnite paths
        possible_paths = [
            "C:\\Program Files\\Epic Games\\Fortnite\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe",
            "D:\\Program Files\\Epic Games\\Fortnite\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe"
        ]

        for path in possible_paths:
            if os.path.exists(path):
                self.fortnite_path = path
                break

        if not self.fortnite_path:
            print("[-] Fortnite installation not found")
            sys.exit(1)

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
        except Exception as e:
            print("[-] Failed to launch Fortnite:", str(e))
            sys.exit(1)

    def run(self):
        """Main method to run the launcher"""
        print("[*] Starting ZeroFN Private Server...")
        
        # Start backend and proxy
        self.start_backend()
        self.start_proxy()
        
        # Setup and launch Fortnite
        self.setup_fortnite()
        self.launch_fortnite()
        
        print("[+] ZeroFN Private Server is running!")
        print("[*] Press Ctrl+C to exit...")
        
        try:
            # Keep the script running
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            print("\n[*] Shutting down...")
            if self.backend_process:
                self.backend_process.terminate()
            if self.proxy_process:
                self.proxy_process.terminate()

if __name__ == "__main__":
    launcher = ZeroFNLauncher()
    launcher.run()


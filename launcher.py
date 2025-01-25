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
from tkinter import ttk, messagebox, filedialog
from PIL import Image, ImageTk
import customtkinter as ctk

class ZeroFNLauncher:
    def __init__(self):
        self.backend_process = None
        self.proxy_process = None
        self.fortnite_path = None
        self.backend_port = 5595
        self.proxy_port = 7777
        self.xmpp_process = None
        self.matchmaker_process = None
        self.mcp_process = None

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

        self.xmpp_status = ctk.CTkLabel(
            self.status_frame,
            text="XMPP: Stopped",
            font=("Segoe UI", 12)
        )
        self.xmpp_status.pack(pady=5)

        self.matchmaker_status = ctk.CTkLabel(
            self.status_frame,
            text="Matchmaker: Stopped",
            font=("Segoe UI", 12)
        )
        self.matchmaker_status.pack(pady=5)

        self.mcp_status = ctk.CTkLabel(
            self.status_frame,
            text="MCP: Stopped",
            font=("Segoe UI", 12)
        )
        self.mcp_status.pack(pady=5)

        # Fortnite path selection
        self.path_frame = ctk.CTkFrame(self.main_frame)
        self.path_frame.pack(fill="x", padx=20, pady=10)
        
        self.path_label = ctk.CTkLabel(
            self.path_frame,
            text="Fortnite Path:",
            font=("Segoe UI", 12)
        )
        self.path_label.pack(side="left", padx=5)
        
        self.path_entry = ctk.CTkEntry(
            self.path_frame,
            width=400,
            placeholder_text="Enter Fortnite installation path"
        )
        self.path_entry.pack(side="left", padx=5)
        
        self.path_button = ctk.CTkButton(
            self.path_frame,
            text="Browse",
            command=self.browse_fortnite,
            width=100
        )
        self.path_button.pack(side="right", padx=5)

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

    def browse_fortnite(self):
        """Open file dialog to select Fortnite installation"""
        initial_dir = "C:\\Program Files\\Epic Games"
        if not os.path.exists(initial_dir):
            initial_dir = "C:\\"
            
        path = filedialog.askopenfilename(
            title="Select FortniteClient-Win64-Shipping.exe",
            initialdir=initial_dir,
            filetypes=[("Executable files", "FortniteClient-Win64-Shipping.exe")]
        )
        
        if path:
            self.fortnite_path = path
            self.path_entry.delete(0, tk.END)
            self.path_entry.insert(0, path)
            print(f"[+] Selected Fortnite path: {path}")

    def start_backend(self):
        """Start all Node.js backend services"""
        try:
            js_path = os.path.join(os.path.dirname(__file__), 'js')
            
            # Start main backend
            self.backend_process = subprocess.Popen(['node', 'app.js'], cwd=js_path)
            print("[+] Main backend server started on port", self.backend_port)
            self.backend_status.configure(text="Backend: Running")

            # Start XMPP server
            self.xmpp_process = subprocess.Popen(['node', 'xmpp/index.js'], cwd=js_path)
            print("[+] XMPP server started")
            self.xmpp_status.configure(text="XMPP: Running")

            # Start matchmaker
            self.matchmaker_process = subprocess.Popen(['node', 'matchmaker/index.js'], cwd=js_path)
            print("[+] Matchmaker server started")
            self.matchmaker_status.configure(text="Matchmaker: Running")

            # Start MCP (Monthly Crew Pack) server
            self.mcp_process = subprocess.Popen(['node', 'mcp/index.js'], cwd=js_path)
            print("[+] MCP server started")
            self.mcp_status.configure(text="MCP: Running")

            time.sleep(2)
            return True

        except Exception as e:
            print("[-] Failed to start backend services:", str(e))
            messagebox.showerror("Error", f"Failed to start backend services: {str(e)}")
            return False

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
        """Verify Fortnite path"""
        self.fortnite_path = self.path_entry.get().strip()
        
        if not self.fortnite_path:
            print("[-] Please enter or select Fortnite installation path")
            messagebox.showerror("Error", "Please enter or select Fortnite installation path")
            return False
            
        if not os.path.exists(self.fortnite_path):
            print("[-] Specified Fortnite path does not exist")
            messagebox.showerror("Error", "Specified Fortnite path does not exist")
            return False
            
        if not self.fortnite_path.endswith("FortniteClient-Win64-Shipping.exe"):
            print("[-] Invalid Fortnite executable selected")
            messagebox.showerror("Error", "Please select FortniteClient-Win64-Shipping.exe")
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
            if self.xmpp_process:
                self.xmpp_process.terminate()
            if self.matchmaker_process:
                self.matchmaker_process.terminate()
            if self.mcp_process:
                self.mcp_process.terminate()
            self.root.destroy()

if __name__ == "__main__":
    launcher = ZeroFNLauncher()
    launcher.run()

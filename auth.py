from http.server import HTTPServer, BaseHTTPRequestHandler
import webbrowser
import json
import requests
import urllib.parse
import os
import threading
import time
import base64
import random
import ssl

# Disable SSL warnings
requests.packages.urllib3.disable_warnings()

class AuthHandler(BaseHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        self.server_should_close = False
        super().__init__(*args, **kwargs)

    def do_GET(self):
        if self.path == '/':
            # Serve login page
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            
            html = """
            <html>
            <head>
                <title>ZeroFN Login</title>
                <style>
                    body { 
                        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        height: 100vh;
                        margin: 0;
                        background: linear-gradient(135deg, #1e1e2f 0%, #2d2d44 100%);
                        color: white;
                    }
                    .login-container {
                        background: rgba(30, 30, 47, 0.95);
                        padding: 60px;
                        border-radius: 20px;
                        text-align: center;
                        box-shadow: 0 12px 40px 0 rgba(0,0,0,0.4);
                        backdrop-filter: blur(15px);
                        border: 2px solid rgba(255,255,255,0.1);
                        max-width: 500px;
                        width: 90%;
                    }
                    h1 {
                        background: linear-gradient(45deg, #00c3ff, #0095f6);
                        -webkit-background-clip: text;
                        -webkit-text-fill-color: transparent;
                        margin-bottom: 30px;
                        font-size: 3em;
                        text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
                    }
                    p {
                        font-size: 1.3em;
                        margin-bottom: 40px;
                        color: #e0e0e0;
                        line-height: 1.6;
                    }
                    .login-btn {
                        background: linear-gradient(45deg, #0095f6, #00c3ff);
                        color: white;
                        padding: 18px 45px;
                        border: none;
                        border-radius: 30px;
                        cursor: pointer;
                        font-size: 20px;
                        font-weight: bold;
                        transition: all 0.3s ease;
                        text-transform: uppercase;
                        letter-spacing: 1.5px;
                        margin-bottom: 20px;
                        display: block;
                        width: 100%;
                        box-shadow: 0 5px 15px rgba(0,149,246,0.3);
                    }
                    .guest-btn {
                        background: linear-gradient(45deg, #555566, #666677);
                        color: white;
                        padding: 18px 45px;
                        border: none;
                        border-radius: 30px;
                        cursor: pointer;
                        font-size: 20px;
                        font-weight: bold;
                        transition: all 0.3s ease;
                        text-transform: uppercase;
                        letter-spacing: 1.5px;
                        width: 100%;
                        box-shadow: 0 5px 15px rgba(85,85,102,0.3);
                    }
                    .login-btn:hover {
                        transform: translateY(-3px);
                        box-shadow: 0 8px 25px rgba(0,149,246,0.5);
                    }
                    .guest-btn:hover {
                        transform: translateY(-3px);
                        box-shadow: 0 8px 25px rgba(85,85,102,0.5);
                    }
                    .logo {
                        width: 120px;
                        height: 120px;
                        margin-bottom: 20px;
                    }
                </style>
            </head>
            <body>
                <div class="login-container">
                    <img src="https://i.imgur.com/XYZ123.png" alt="ZeroFN Logo" class="logo">
                    <h1>Welcome to ZeroFN</h1>
                    <p>Experience Fortnite like never before.<br>Login with your Epic Games account or continue as guest.</p>
                    <a href="/login">
                        <button class="login-btn">Login with Epic Games</button>
                    </a>
                    <a href="/guest">
                        <button class="guest-btn">Continue as Guest</button>
                    </a>
                </div>
            </body>
            </html>
            """
            self.wfile.write(html.encode())

        elif self.path == '/guest':
            # Create guest token with more realistic values
            guest_token = {
                "access_token": f"eg1~guest~{base64.b64encode(os.urandom(32)).decode('utf-8')}",
                "expires_in": 28800,
                "expires_at": "2099-12-31T23:59:59.999Z",
                "token_type": "bearer",
                "account_id": f"guest_{int(time.time())}",
                "client_id": "3446cd72694c4a4485d81b77adbb2141",
                "displayName": f"Guest-{random.randint(1000,9999)}",
                "internal_client": True,
                "client_service": "fortnite",
                "app": "fortnite"
            }

            with open('auth_token.json', 'w') as f:
                json.dump(guest_token, f)

            os.environ['LOGGED_IN'] = "(Guest)"

            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()

            success_html = """
            <html>
            <head>
                <meta http-equiv="refresh" content="3;url=http://127.0.0.1:7777/close">
                <style>
                    body {
                        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                        background: linear-gradient(135deg, #1e1e2f 0%, #2d2d44 100%);
                        color: white;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        height: 100vh;
                        margin: 0;
                    }
                    .success-container {
                        text-align: center;
                        padding: 40px;
                        background: rgba(30, 30, 47, 0.95);
                        border-radius: 20px;
                        box-shadow: 0 12px 40px 0 rgba(0,0,0,0.4);
                    }
                    h2 {
                        color: #00ff00;
                        font-size: 2em;
                        margin-bottom: 20px;
                    }
                </style>
            </head>
            <body>
                <div class="success-container">
                    <h2>Successfully logged in as Guest</h2>
                    <p>This window will close automatically...</p>
                </div>
            </body>
            </html>
            """
            self.wfile.write(success_html.encode())

            def shutdown():
                time.sleep(3)
                self.server.shutdown()
            threading.Thread(target=shutdown).start()

        elif self.path == '/login':
            # Enhanced Epic Games OAuth flow with all permissions
            client_id = "3446cd72694c4a4485d81b77adbb2141"
            redirect_uri = "http://127.0.0.1:7777/epic/callback"
            state = base64.b64encode(os.urandom(32)).decode('utf-8')
            
            auth_params = {
                'client_id': client_id,
                'response_type': 'code',
                'redirect_uri': redirect_uri,
                'scope': 'basic_profile friends presence openid offline_access email accounts public_profile',
                'state': state,
                'prompt': 'login'
            }
            
            auth_url = f"https://www.epicgames.com/id/authorize?{urllib.parse.urlencode(auth_params)}"
            
            self.send_response(302)
            self.send_header('Location', auth_url)
            self.end_headers()

        elif self.path.startswith('/epic/callback'):
            query = urllib.parse.parse_qs(urllib.parse.urlparse(self.path).query)
            auth_code = query.get('code', [None])[0]

            if auth_code:
                token_url = "https://account-public-service-prod.ol.epicgames.com/account/api/oauth/token"
                client_id = "3446cd72694c4a4485d81b77adbb2141"
                client_secret = "e7a553f4-f96f-4d4a-b949-99cbfea4d584"
                redirect_uri = "http://127.0.0.1:7777/epic/callback"
                
                headers = {
                    'Content-Type': 'application/x-www-form-urlencoded',
                    'Authorization': f'Basic {base64.b64encode(f"{client_id}:{client_secret}".encode()).decode()}',
                    'User-Agent': 'EpicGamesLauncher/13.3.0-17155645+++Portal+Release-Live Windows/10.0.22621.1.256.64bit'
                }
                
                data = {
                    'grant_type': 'authorization_code',
                    'code': auth_code,
                    'token_type': 'eg1'
                }

                try:
                    # Disable SSL verification for token request
                    response = requests.post(token_url, headers=headers, data=data, verify=False)
                    response.raise_for_status()
                    
                    token_data = response.json()
                    
                    # Get account details with retry mechanism
                    max_retries = 3
                    for attempt in range(max_retries):
                        try:
                            account_url = f"https://account-public-service-prod.ol.epicgames.com/account/api/public/account/{token_data['account_id']}"
                            account_headers = {
                                'Authorization': f'Bearer {token_data["access_token"]}',
                                'User-Agent': 'EpicGamesLauncher/13.3.0-17155645+++Portal+Release-Live Windows/10.0.22621.1.256.64bit'
                            }
                            account_response = requests.get(account_url, headers=account_headers, verify=False)
                            account_response.raise_for_status()
                            
                            account_data = account_response.json()
                            token_data['displayName'] = account_data.get('displayName', 'ZeroFN Player')
                            break
                        except:
                            if attempt == max_retries - 1:
                                raise
                            time.sleep(1)
                    
                    # Save enhanced token data
                    with open('auth_token.json', 'w') as f:
                        json.dump(token_data, f, indent=4)
                    
                    os.environ['LOGGED_IN'] = "(Logged In)"
                    
                    self.send_response(200)
                    self.send_header('Content-type', 'text/html')
                    self.end_headers()
                    
                    success_html = """
                    <html>
                    <head>
                        <meta http-equiv="refresh" content="3;url=http://127.0.0.1:7777/close">
                        <style>
                            body {
                                font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                                background: linear-gradient(135deg, #1e1e2f 0%, #2d2d44 100%);
                                color: white;
                                display: flex;
                                justify-content: center;
                                align-items: center;
                                height: 100vh;
                                margin: 0;
                            }
                            .success-container {
                                text-align: center;
                                padding: 50px;
                                background: rgba(30, 30, 47, 0.95);
                                border-radius: 20px;
                                box-shadow: 0 12px 40px 0 rgba(0,0,0,0.4);
                                max-width: 600px;
                                width: 90%;
                            }
                            h1 {
                                color: #00ff00;
                                font-size: 2.5em;
                                margin-bottom: 20px;
                            }
                            p {
                                font-size: 1.2em;
                                margin: 10px 0;
                            }
                            .welcome {
                                color: #0095f6;
                                font-size: 1.5em;
                                margin-top: 20px;
                            }
                        </style>
                    </head>
                    <body>
                        <div class="success-container">
                            <h1>Login Successful!</h1>
                            <p class="welcome">Welcome, """ + token_data['displayName'] + """!</p>
                            <p>You can now close this window and return to ZeroFN.</p>
                            <p>Window will close automatically in 3 seconds...</p>
                        </div>
                    </body>
                    </html>
                    """
                    self.wfile.write(success_html.encode())
                    
                    def shutdown():
                        time.sleep(3)
                        self.server.shutdown()
                    threading.Thread(target=shutdown).start()
                    return
                    
                except requests.exceptions.RequestException as e:
                    print(f"Error during authentication: {str(e)}")
                    self.send_error_page("Authentication failed. Please try again.")
                except Exception as e:
                    print(f"Unexpected error: {str(e)}")
                    self.send_error_page("An unexpected error occurred. Please try again.")

            else:
                self.send_error_page("Invalid authentication code received.")

    def send_error_page(self, message):
        self.send_response(400)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        error_html = f"""
        <html>
        <head>
            <style>
                body {{
                    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                    background: linear-gradient(135deg, #1e1e2f 0%, #2d2d44 100%);
                    color: white;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    height: 100vh;
                    margin: 0;
                }}
                .error-container {{
                    text-align: center;
                    padding: 50px;
                    background: rgba(30, 30, 47, 0.95);
                    border-radius: 20px;
                    box-shadow: 0 12px 40px 0 rgba(0,0,0,0.4);
                }}
                h1 {{
                    color: #ff0000;
                    margin-bottom: 20px;
                }}
                .retry-btn {{
                    background: linear-gradient(45deg, #666666, #888888);
                    color: white;
                    padding: 15px 40px;
                    border: none;
                    border-radius: 25px;
                    cursor: pointer;
                    font-size: 18px;
                    margin-top: 20px;
                    text-decoration: none;
                    display: inline-block;
                }}
            </style>
        </head>
        <body>
            <div class="error-container">
                <h1>Authentication Failed</h1>
                <p>{message}</p>
                <a href="/" class="retry-btn">Try Again</a>
            </div>
        </body>
        </html>
        """
        self.wfile.write(error_html.encode())

def start_auth_server():
    server = HTTPServer(('127.0.0.1', 7777), AuthHandler)
    print("Authentication server started at http://127.0.0.1:7777")
    webbrowser.open('http://127.0.0.1:7777')
    server.serve_forever()

if __name__ == '__main__':
    start_auth_server()

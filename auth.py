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
import string
from datetime import datetime, timezone

# Disable SSL warnings
requests.packages.urllib3.disable_warnings()

def generate_guest_credentials():
    # Generate random username
    adjectives = ['Happy', 'Lucky', 'Brave', 'Swift', 'Clever', 'Wild']
    nouns = ['Ninja', 'Warrior', 'Knight', 'Hunter', 'Scout', 'Hero']
    numbers = ''.join(random.choices(string.digits, k=4))
    username = f"{random.choice(adjectives)}{random.choice(nouns)}{numbers}"
    
    # Generate random email and password
    email = f"guest_{username.lower()}@zerofn.com"
    password = ''.join(random.choices(string.ascii_letters + string.digits, k=12))
    
    # Generate random account ID
    account_id = ''.join(random.choices(string.hexdigits, k=32))
    
    return username, email, password, account_id

class AuthHandler(BaseHTTPRequestHandler):
    # Make state a class variable instead of instance variable
    state = None
    
    def __init__(self, *args, **kwargs):
        self.server_should_close = False
        super().__init__(*args, **kwargs)

    def do_GET(self):
        # Handle favicon.ico request
        if self.path == '/favicon.ico':
            self.send_response(404)
            self.end_headers()
            return

        # Handle zerofn.jpg request 
        if self.path == '/zerofn.jpg':
            self.send_response(200)
            self.send_header('Content-type', 'image/jpeg')
            self.end_headers()
            try:
                with open('zerofn.jpg', 'rb') as f:
                    self.wfile.write(f.read())
            except:
                # If image not found, return empty response
                pass
            return

        if self.path == '/':
            # Serve login page with guest option
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            
            html = """
            <html>
            <head>
                <title>ZeroFN Login</title>
                <link href="https://fonts.googleapis.com/css2?family=Bebas+Neue&family=Roboto:wght@400;700&display=swap" rel="stylesheet">
                <link rel="icon" type="image/jpg" href="zerofn.jpg">
                <style>
                    * {
                        margin: 0;
                        padding: 0;
                        box-sizing: border-box;
                    }
                    body { 
                        font-family: 'Roboto', sans-serif;
                        background: #0c0c0d;
                        color: #ffffff;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        min-height: 100vh;
                        margin: 0;
                        overflow-x: hidden;
                    }
                    .login-container {
                        background: rgba(35, 39, 42, 0.5);
                        padding: 60px;
                        border-radius: 20px;
                        text-align: center;
                        box-shadow: 0 12px 40px rgba(0, 0, 0, 0.6);
                        backdrop-filter: blur(15px);
                        border: 2px solid #2b2b2b;
                        max-width: 600px;
                        width: 90%;
                        animation: fadeInUp 1s ease;
                    }
                    .logo {
                        width: 120px;
                        height: 120px;
                        margin-bottom: 30px;
                        transition: transform 0.3s ease;
                    }
                    .logo:hover {
                        transform: scale(1.05);
                    }
                    h1 {
                        font-family: 'Bebas Neue', sans-serif;
                        font-size: 3.5rem;
                        margin-bottom: 20px;
                        text-shadow: 0 8px 16px rgba(0, 0, 0, 0.8);
                    }
                    p {
                        font-size: 1.2rem;
                        margin-bottom: 40px;
                        color: #cccccc;
                        line-height: 1.6;
                    }
                    .btn {
                        background: #fccc4d;
                        color: #121212;
                        padding: 18px 45px;
                        border: none;
                        border-radius: 5px;
                        cursor: pointer;
                        font-size: 1.3rem;
                        font-weight: bold;
                        text-transform: uppercase;
                        transition: all 0.3s ease;
                        display: block;
                        width: 100%;
                        margin-bottom: 20px;
                        box-shadow: 0 4px 15px rgba(252, 204, 77, 0.3);
                    }
                    .btn:hover {
                        background: #ffc700;
                        transform: scale(1.05) translateY(-2px);
                        box-shadow: 0 6px 20px rgba(252, 204, 77, 0.4);
                    }
                    .guest-btn {
                        background: #4d8bfc;
                    }
                    .guest-btn:hover {
                        background: #3b7af0;
                    }
                    @keyframes fadeInUp {
                        from {
                            opacity: 0;
                            transform: translateY(30px);
                        }
                        to {
                            opacity: 1;
                            transform: translateY(0);
                        }
                    }
                </style>
            </head>
            <body>
                <div class="login-container">
                    <img src="zerofn.jpg" alt="ZeroFN Logo" class="logo">
                    <h1>Welcome to ZeroFN</h1>
                    <p>Experience OG Fortnite like never before.<br>Login with your Epic Games account or continue as guest.</p>
                    <a href="/login">
                        <button class="btn">Login with Epic Games</button>
                    </a>
                    <a href="/guest-login">
                        <button class="btn guest-btn">Continue as Guest</button>
                    </a>
                </div>
            </body>
            </html>
            """
            self.wfile.write(html.encode())

        elif self.path == '/guest-login':
            # Generate guest credentials
            username, email, password, account_id = generate_guest_credentials()
            
            # Create guest token data for OG Fortnite private server
            expires_at = int(time.time()) + 28800
            token_data = {
                'access_token': base64.b64encode(os.urandom(32)).decode('utf-8'),
                'refresh_token': base64.b64encode(os.urandom(32)).decode('utf-8'),
                'expires_in': 28800,
                'expires_at': datetime.fromtimestamp(expires_at, tz=timezone.utc).isoformat(),
                'token_type': 'bearer',
                'account_id': account_id,
                'client_id': 'zerofn_client',
                'internal_client': True,
                'client_service': 'fortnite',
                'displayName': username,
                'app': 'fortnite',
                'in_app_id': account_id,
                'device_id': 'zerofn_device',
                'email': email,
                'password': password,
                'is_guest': True,
                'perms': [
                    'fortnite:profile:*:commands',
                    'account:public:account',
                    'account:oauth:ext_auth:persist:claim',
                    'basic:profile:*:public',
                    'friends:list',
                    'presence:*:*'
                ],
                'membership': {
                    'active': True,
                    'level': 'premium',
                    'expires_at': datetime.fromtimestamp(expires_at + 2592000, tz=timezone.utc).isoformat()
                },
                'profile': {
                    'character': 'CID_001_Athena_Commando_F_Default',
                    'backpack': 'BID_001_BlackKnight',
                    'pickaxe': 'Pickaxe_Lockjaw',
                    'glider': 'Glider_Warthog',
                    'trail': 'Trails_ID_012_Lightning',
                    'dance_moves': ['EID_DanceMoves', 'EID_Fresh', 'EID_Floss'],
                    'banner': {
                        'icon': 'BRS01',
                        'color': 'defaultcolor1',
                        'season_level': 100
                    }
                }
            }

            # Save token data
            with open('auth_token.json', 'w') as f:
                json.dump(token_data, f, indent=4)
            
            os.environ['LOGGED_IN'] = "(Guest)"
            
            # Show success page
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            
            success_html = f"""
            <html>
            <head>
                <meta http-equiv="refresh" content="10;url=http://127.0.0.1:7777/close">
                <link href="https://fonts.googleapis.com/css2?family=Bebas+Neue&family=Roboto:wght@400;700&display=swap" rel="stylesheet">
                <link rel="icon" type="image/jpg" href="zerofn.jpg">
                <style>
                    * {{
                        margin: 0;
                        padding: 0;
                        box-sizing: border-box;
                    }}
                    body {{
                        font-family: 'Roboto', sans-serif;
                        background: #0c0c0d;
                        color: #ffffff;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        min-height: 100vh;
                        margin: 0;
                    }}
                    .success-container {{
                        text-align: center;
                        padding: 60px;
                        background: rgba(35, 39, 42, 0.5);
                        border-radius: 20px;
                        box-shadow: 0 12px 40px rgba(0, 0, 0, 0.6);
                        backdrop-filter: blur(15px);
                        border: 2px solid #2b2b2b;
                        animation: fadeInUp 1s ease;
                    }}
                    h1 {{
                        font-family: 'Bebas Neue', sans-serif;
                        color: #4d8bfc;
                        font-size: 3em;
                        margin-bottom: 20px;
                        text-shadow: 0 8px 16px rgba(0, 0, 0, 0.8);
                    }}
                    p {{
                        font-size: 1.2rem;
                        color: #cccccc;
                        margin: 10px 0;
                    }}
                    .credentials {{
                        background: rgba(0, 0, 0, 0.3);
                        padding: 20px;
                        border-radius: 10px;
                        margin: 20px 0;
                        text-align: left;
                    }}
                    .welcome {{
                        color: #4d8bfc;
                        font-size: 1.5em;
                        margin-top: 20px;
                    }}
                    @keyframes fadeInUp {{
                        from {{
                            opacity: 0;
                            transform: translateY(30px);
                        }}
                        to {{
                            opacity: 1;
                            transform: translateY(0);
                        }}
                    }}
                </style>
            </head>
            <body>
                <div class="success-container">
                    <h1>Guest Login Successful!</h1>
                    <p class="welcome">Welcome, {username}!</p>
                    <div class="credentials">
                        <p><strong>Username:</strong> {username}</p>
                        <p><strong>Email:</strong> {email}</p>
                        <p><strong>Password:</strong> {password}</p>
                    </div>
                    <p>Please save these credentials if you want to use them again later.</p>
                    <p>Window will close automatically in 10 seconds...</p>
                </div>
            </body>
            </html>
            """
            self.wfile.write(success_html.encode())
            
            def shutdown():
                time.sleep(10)
                self.server.shutdown()
            threading.Thread(target=shutdown).start()
            return

        elif self.path == '/login':
            # Enhanced Epic Games OAuth flow with all permissions
            client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
            redirect_uri = "http://127.0.0.1:7777/epic/auth/callback/zerofn"
            AuthHandler.state = base64.b64encode(os.urandom(32)).decode('utf-8') # Store state as class variable
            
            auth_params = {
                'client_id': client_id,
                'response_type': 'code',
                'redirect_uri': redirect_uri,
                'scope': 'basic_profile friends_list presence openid offline_access email accounts public_profile',
                'state': AuthHandler.state,
                'prompt': 'login'
            }
            
            auth_url = f"https://www.epicgames.com/id/authorize?{urllib.parse.urlencode(auth_params)}"
            
            self.send_response(302)
            self.send_header('Location', auth_url)
            self.end_headers()

        elif self.path.startswith('/epic/auth/callback/zerofn'):
            query = urllib.parse.parse_qs(urllib.parse.urlparse(self.path).query)
            auth_code = query.get('code', [None])[0]
            received_state = query.get('state', [None])[0]

            # Verify state parameter using class variable
            if not AuthHandler.state or received_state != AuthHandler.state:
                self.send_error_page("Invalid state parameter. Please try logging in again.")
                return

            if auth_code:
                token_url = "https://account-public-service-prod.ol.epicgames.com/account/api/oauth/token"
                client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
                client_secret = "Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA"
                redirect_uri = "http://127.0.0.1:7777/epic/auth/callback/zerofn"
                
                auth_str = f"{client_id}:{client_secret}"
                auth_bytes = auth_str.encode('ascii')
                auth_b64 = base64.b64encode(auth_bytes).decode('ascii')
                
                headers = {
                    'Authorization': f'Basic {auth_b64}',
                    'Content-Type': 'application/x-www-form-urlencoded',
                    'User-Agent': 'EpicGamesLauncher/13.3.0-17155645+++Portal+Release-Live Windows/10.0.22621.1.256.64bit'
                }
                
                data = {
                    'grant_type': 'authorization_code',
                    'code': auth_code,
                    'redirect_uri': redirect_uri
                }

                try:
                    # Add retry mechanism
                    max_retries = 3
                    retry_count = 0
                    
                    while retry_count < max_retries:
                        try:
                            response = requests.post(token_url, headers=headers, data=data, verify=False, timeout=10)
                            response.raise_for_status()
                            break
                        except (requests.exceptions.RequestException, requests.exceptions.Timeout) as e:
                            retry_count += 1
                            if retry_count == max_retries:
                                raise
                            time.sleep(1)  # Wait before retrying
                    
                    token_data = response.json()
                    
                    # Convert expires_at to ISO format
                    expires_at = int(time.time()) + token_data['expires_in']
                    token_data['expires_at'] = datetime.fromtimestamp(expires_at, tz=timezone.utc).isoformat()
                    
                    # Get account details with retry
                    retry_count = 0
                    account_url = "https://account-public-service-prod.ol.epicgames.com/account/api/oauth/verify"
                    account_headers = {
                        'Authorization': f'Bearer {token_data["access_token"]}',
                        'User-Agent': 'EpicGamesLauncher/13.3.0-17155645+++Portal+Release-Live Windows/10.0.22621.1.256.64bit'
                    }
                    
                    while retry_count < max_retries:
                        try:
                            account_response = requests.get(account_url, headers=account_headers, verify=False, timeout=10)
                            account_response.raise_for_status()
                            break
                        except (requests.exceptions.RequestException, requests.exceptions.Timeout) as e:
                            retry_count += 1
                            if retry_count == max_retries:
                                raise
                            time.sleep(1)
                    
                    account_data = account_response.json()
                    token_data['displayName'] = account_data.get('displayName', 'ZeroFN Player')
                    token_data['account_id'] = account_data.get('account_id')

                    # Add OG Fortnite specific data
                    token_data['perms'] = [
                        'fortnite:profile:*:commands',
                        'account:public:account',
                        'account:oauth:ext_auth:persist:claim',
                        'basic:profile:*:public',
                        'friends:list',
                        'presence:*:*'
                    ]
                    token_data['membership'] = {
                        'active': True,
                        'level': 'premium',
                        'expires_at': datetime.fromtimestamp(expires_at + 2592000, tz=timezone.utc).isoformat()
                    }
                    token_data['profile'] = {
                        'character': 'CID_001_Athena_Commando_F_Default',
                        'backpack': 'BID_001_BlackKnight',
                        'pickaxe': 'Pickaxe_Lockjaw',
                        'glider': 'Glider_Warthog',
                        'trail': 'Trails_ID_012_Lightning',
                        'dance_moves': ['EID_DanceMoves', 'EID_Fresh', 'EID_Floss'],
                        'banner': {
                            'icon': 'BRS01',
                            'color': 'defaultcolor1',
                            'season_level': 100
                        }
                    }

                    # Get friends list with retry
                    retry_count = 0
                    friends_url = f"https://friends-public-service-prod.ol.epicgames.com/friends/api/v1/{token_data['account_id']}/summary"
                    
                    while retry_count < max_retries:
                        try:
                            friends_response = requests.get(friends_url, headers=account_headers, verify=False, timeout=10)
                            if friends_response.status_code == 200:
                                token_data['friends'] = friends_response.json()
                            break
                        except (requests.exceptions.RequestException, requests.exceptions.Timeout) as e:
                            retry_count += 1
                            if retry_count == max_retries:
                                break  # Skip friends list if it fails
                            time.sleep(1)
                    
                    # Save token data
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
                        <link href="https://fonts.googleapis.com/css2?family=Bebas+Neue&family=Roboto:wght@400;700&display=swap" rel="stylesheet">
                        <link rel="icon" type="image/jpg" href="zerofn.jpg">
                        <style>
                            * {
                                margin: 0;
                                padding: 0;
                                box-sizing: border-box;
                            }
                            body {
                                font-family: 'Roboto', sans-serif;
                                background: #0c0c0d;
                                color: #ffffff;
                                display: flex;
                                justify-content: center;
                                align-items: center;
                                min-height: 100vh;
                                margin: 0;
                            }
                            .success-container {
                                text-align: center;
                                padding: 60px;
                                background: rgba(35, 39, 42, 0.5);
                                border-radius: 20px;
                                box-shadow: 0 12px 40px rgba(0, 0, 0, 0.6);
                                backdrop-filter: blur(15px);
                                border: 2px solid #2b2b2b;
                                animation: fadeInUp 1s ease;
                            }
                            h1 {
                                font-family: 'Bebas Neue', sans-serif;
                                color: #fccc4d;
                                font-size: 3em;
                                margin-bottom: 20px;
                                text-shadow: 0 8px 16px rgba(0, 0, 0, 0.8);
                            }
                            p {
                                font-size: 1.2rem;
                                color: #cccccc;
                                margin: 10px 0;
                            }
                            .welcome {
                                color: #fccc4d;
                                font-size: 1.5em;
                                margin-top: 20px;
                            }
                            @keyframes fadeInUp {
                                from {
                                    opacity: 0;
                                    transform: translateY(30px);
                                }
                                to {
                                    opacity: 1;
                                    transform: translateY(0);
                                }
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
                    self.send_error_page("Authentication failed. Please check your internet connection and try again.")
                except Exception as e:
                    print(f"Unexpected error: {str(e)}")
                    self.send_error_page("An unexpected error occurred. Please try logging in again.")

            else:
                self.send_error_page("Invalid authentication code received. Please try logging in again.")

        elif self.path == '/close':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(b"<html><body><script>window.close()</script></body></html>")
            
            def shutdown():
                time.sleep(1)
                self.server.shutdown()
            threading.Thread(target=shutdown).start()

    def send_error_page(self, message):
        self.send_response(400)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        error_html = f"""
        <html>
        <head>
            <link href="https://fonts.googleapis.com/css2?family=Bebas+Neue&family=Roboto:wght@400;700&display=swap" rel="stylesheet">
            <link rel="icon" type="image/jpg" href="zerofn.jpg">
            <style>
                * {{
                    margin: 0;
                    padding: 0;
                    box-sizing: border-box;
                }}
                body {{
                    font-family: 'Roboto', sans-serif;
                    background: #0c0c0d;
                    color: #ffffff;
                    display: flex;
                    justify-content: center;
                    align-items: center;
                    min-height: 100vh;
                    margin: 0;
                }}
                .error-container {{
                    text-align: center;
                    padding: 60px;
                    background: rgba(35, 39, 42, 0.5);
                    border-radius: 20px;
                    box-shadow: 0 12px 40px rgba(0, 0, 0, 0.6);
                    backdrop-filter: blur(15px);
                    border: 2px solid #2b2b2b;
                    animation: fadeInUp 1s ease;
                }}
                h1 {{
                    font-family: 'Bebas Neue', sans-serif;
                    color: #ff6b6b;
                    font-size: 3em;
                    margin-bottom: 20px;
                    text-shadow: 0 8px 16px rgba(0, 0, 0, 0.8);
                }}
                p {{
                    font-size: 1.2rem;
                    color: #cccccc;
                    margin-bottom: 30px;
                }}
                .retry-btn {{
                    background: #fccc4d;
                    color: #121212;
                    padding: 15px 40px;
                    border: none;
                    border-radius: 5px;
                    cursor: pointer;
                    font-size: 1.2rem;
                    font-weight: bold;
                    text-transform: uppercase;
                    text-decoration: none;
                    display: inline-block;
                    transition: all 0.3s ease;
                    box-shadow: 0 4px 15px rgba(252, 204, 77, 0.3);
                }}
                .retry-btn:hover {{
                    background: #ffc700;
                    transform: scale(1.05) translateY(-2px);
                    box-shadow: 0 6px 20px rgba(252, 204, 77, 0.4);
                }}
                @keyframes fadeInUp {{
                    from {{
                        opacity: 0;
                        transform: translateY(30px);
                    }}
                    to {{
                        opacity: 1;
                        transform: translateY(0);
                    }}
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

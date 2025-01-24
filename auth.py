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
                    .login-btn {
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
                    .guest-btn {
                        background: rgba(35, 39, 42, 0.5);
                        color: #ffffff;
                        padding: 18px 45px;
                        border: 2px solid #2b2b2b;
                        border-radius: 5px;
                        cursor: pointer;
                        font-size: 1.3rem;
                        font-weight: bold;
                        text-transform: uppercase;
                        transition: all 0.3s ease;
                        display: block;
                        width: 100%;
                    }
                    .login-btn:hover {
                        background: #ffc700;
                        transform: scale(1.05) translateY(-2px);
                        box-shadow: 0 6px 20px rgba(252, 204, 77, 0.4);
                    }
                    .guest-btn:hover {
                        background: rgba(35, 39, 42, 0.8);
                        transform: scale(1.05) translateY(-2px);
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
                "client_id": "xyza7891TydzdNolyGQJYa9b6n6rLMJl",
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
                    h2 {
                        font-family: 'Bebas Neue', sans-serif;
                        color: #fccc4d;
                        font-size: 2.5em;
                        margin-bottom: 20px;
                        text-shadow: 0 8px 16px rgba(0, 0, 0, 0.8);
                    }
                    p {
                        font-size: 1.2rem;
                        color: #cccccc;
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
            client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
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
                client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
                client_secret = "Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA"
                redirect_uri = "http://127.0.0.1:7777/epic/callback"
                
                headers = {
                    'Content-Type': 'application/x-www-form-urlencoded',
                    'User-Agent': 'EpicGamesLauncher/13.3.0-17155645+++Portal+Release-Live Windows/10.0.22621.1.256.64bit'
                }
                
                data = {
                    'grant_type': 'authorization_code',
                    'code': auth_code,
                    'client_id': client_id,
                    'client_secret': client_secret,
                    'redirect_uri': redirect_uri
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
                            account_url = "https://account-public-service-prod.ol.epicgames.com/account/api/public/account"
                            account_headers = {
                                'Authorization': f'Bearer {token_data["access_token"]}',
                                'User-Agent': 'EpicGamesLauncher/13.3.0-17155645+++Portal+Release-Live Windows/10.0.22621.1.256.64bit'
                            }
                            account_response = requests.get(account_url, headers=account_headers, verify=False)
                            account_response.raise_for_status()
                            
                            account_data = account_response.json()
                            token_data['displayName'] = account_data.get('displayName', 'ZeroFN Player')
                            token_data['account_id'] = account_data.get('id')
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

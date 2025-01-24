from http.server import HTTPServer, BaseHTTPRequestHandler
import webbrowser
import json
import requests
import urllib.parse
import os

class AuthHandler(BaseHTTPRequestHandler):
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
                        background: linear-gradient(135deg, #1a1a1a 0%, #2d2d2d 100%);
                        color: white;
                    }
                    .login-container {
                        background: rgba(45, 45, 45, 0.9);
                        padding: 50px;
                        border-radius: 15px;
                        text-align: center;
                        box-shadow: 0 8px 32px 0 rgba(0,0,0,0.3);
                        backdrop-filter: blur(10px);
                        border: 1px solid rgba(255,255,255,0.1);
                    }
                    h1 {
                        color: #0095f6;
                        margin-bottom: 30px;
                        font-size: 2.5em;
                        text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
                    }
                    p {
                        font-size: 1.2em;
                        margin-bottom: 30px;
                        color: #cccccc;
                    }
                    .login-btn {
                        background: linear-gradient(45deg, #0095f6, #00c3ff);
                        color: white;
                        padding: 15px 40px;
                        border: none;
                        border-radius: 25px;
                        cursor: pointer;
                        font-size: 18px;
                        font-weight: bold;
                        transition: transform 0.2s, box-shadow 0.2s;
                        text-transform: uppercase;
                        letter-spacing: 1px;
                        margin-bottom: 15px;
                        display: block;
                        width: 100%;
                    }
                    .guest-btn {
                        background: linear-gradient(45deg, #666666, #888888);
                        color: white;
                        padding: 15px 40px;
                        border: none;
                        border-radius: 25px;
                        cursor: pointer;
                        font-size: 18px;
                        font-weight: bold;
                        transition: transform 0.2s, box-shadow 0.2s;
                        text-transform: uppercase;
                        letter-spacing: 1px;
                        width: 100%;
                    }
                    .login-btn:hover, .guest-btn:hover {
                        transform: translateY(-2px);
                        box-shadow: 0 5px 15px rgba(0,149,246,0.4);
                    }
                </style>
            </head>
            <body>
                <div class="login-container">
                    <h1>Welcome to ZeroFN</h1>
                    <p>Please login with your Epic Games account or continue as guest</p>
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
            # Handle guest login
            # Create a basic guest token
            guest_token = {
                "access_token": "guest_token",
                "expires_in": 3600,
                "expires_at": "2099-12-31T23:59:59.999Z",
                "token_type": "bearer",
                "account_id": "guest_account",
                "client_id": "guest",
                "displayName": "Guest Player"
            }
            
            with open('auth_token.json', 'w') as f:
                json.dump(guest_token, f)
            
            os.environ['LOGGED_IN'] = "(Guest)"
            
            # Show success page for guest login
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            
            success_html = """
            <html>
            <head>
                <style>
                    body {
                        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                        background: linear-gradient(135deg, #1a1a1a 0%, #2d2d2d 100%);
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
                        background: rgba(45, 45, 45, 0.9);
                        border-radius: 15px;
                        box-shadow: 0 8px 32px 0 rgba(0,0,0,0.3);
                    }
                    h1 {
                        color: #00ff00;
                        margin-bottom: 20px;
                    }
                </style>
            </head>
            <body>
                <div class="success-container">
                    <h1>Continuing as Guest</h1>
                    <p>You can now close this window and return to ZeroFN.</p>
                    <p>Window will close automatically in 3 seconds...</p>
                </div>
                <script>
                    setTimeout(function() {
                        window.close();
                    }, 3000);
                </script>
            </body>
            </html>
            """
            self.wfile.write(success_html.encode())

        elif self.path == '/login':
            # Redirect to Epic Games OAuth
            client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
            redirect_uri = "http://localhost:8000/callback"
            auth_url = f"https://www.epicgames.com/id/authorize?client_id={client_id}&response_type=code&redirect_uri={redirect_uri}"
            self.send_response(302)
            self.send_header('Location', auth_url)
            self.end_headers()

        elif self.path.startswith('/callback'):
            # Handle OAuth callback
            query_components = urllib.parse.parse_qs(urllib.parse.urlparse(self.path).query)
            auth_code = query_components.get('code', [None])[0]
            
            if auth_code:
                # Exchange auth code for access token
                token_url = "https://account-public-service-prod.ol.epicgames.com/account/api/oauth/token"
                client_id = "xyza7891TydzdNolyGQJYa9b6n6rLMJl"
                client_secret = "Eh+FLGJ5GrvCNwmTEp9Hrqdwn2gGnra645eWrp09zVA"
                
                headers = {
                    'Content-Type': 'application/x-www-form-urlencoded',
                    'Authorization': f'Basic {client_id}:{client_secret}'
                }
                
                data = {
                    'grant_type': 'authorization_code',
                    'code': auth_code,
                    'token_type': 'eg1',
                    'redirect_uri': "http://localhost:8000/callback"
                }
                
                response = requests.post(token_url, headers=headers, data=data)
                
                if response.status_code == 200:
                    # Save auth token and redirect back to ZeroFN
                    token_data = response.json()
                    with open('auth_token.json', 'w') as f:
                        json.dump(token_data, f)
                    
                    # Set environment variable for batch file
                    os.environ['LOGGED_IN'] = "(Logged In)"
                    
                    # Show success page and auto-close
                    self.send_response(200)
                    self.send_header('Content-type', 'text/html')
                    self.end_headers()
                    
                    success_html = """
                    <html>
                    <head>
                        <style>
                            body {
                                font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                                background: linear-gradient(135deg, #1a1a1a 0%, #2d2d2d 100%);
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
                                background: rgba(45, 45, 45, 0.9);
                                border-radius: 15px;
                                box-shadow: 0 8px 32px 0 rgba(0,0,0,0.3);
                            }
                            h1 {
                                color: #00ff00;
                                margin-bottom: 20px;
                            }
                        </style>
                    </head>
                    <body>
                        <div class="success-container">
                            <h1>Login Successful!</h1>
                            <p>You can now close this window and return to ZeroFN.</p>
                            <p>Window will close automatically in 3 seconds...</p>
                        </div>
                        <script>
                            setTimeout(function() {
                                window.close();
                            }, 3000);
                        </script>
                    </body>
                    </html>
                    """
                    self.wfile.write(success_html.encode())
                    return

            # If we get here, something went wrong
            self.send_response(400)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            error_html = """
            <html>
            <head>
                <style>
                    body {
                        font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                        background: linear-gradient(135deg, #1a1a1a 0%, #2d2d2d 100%);
                        color: white;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        height: 100vh;
                        margin: 0;
                    }
                    .error-container {
                        text-align: center;
                        padding: 40px;
                        background: rgba(45, 45, 45, 0.9);
                        border-radius: 15px;
                        box-shadow: 0 8px 32px 0 rgba(0,0,0,0.3);
                    }
                    h1 {
                        color: #ff0000;
                    }
                </style>
            </head>
            <body>
                <div class="error-container">
                    <h1>Authentication Failed</h1>
                    <p>Please try again or contact support if the issue persists.</p>
                </div>
            </body>
            </html>
            """
            self.wfile.write(error_html.encode())

def start_auth_server():
    server = HTTPServer(('localhost', 8000), AuthHandler)
    print("Authentication server started at http://localhost:8000")
    webbrowser.open('http://localhost:8000')
    server.serve_forever()

if __name__ == '__main__':
    start_auth_server()

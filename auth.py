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
                        font-family: Arial, sans-serif;
                        display: flex;
                        justify-content: center;
                        align-items: center;
                        height: 100vh;
                        margin: 0;
                        background-color: #1a1a1a;
                        color: white;
                    }
                    .login-container {
                        background-color: #2d2d2d;
                        padding: 40px;
                        border-radius: 10px;
                        text-align: center;
                    }
                    .login-btn {
                        background-color: #0095f6;
                        color: white;
                        padding: 12px 24px;
                        border: none;
                        border-radius: 5px;
                        cursor: pointer;
                        font-size: 16px;
                        margin-top: 20px;
                    }
                    .login-btn:hover {
                        background-color: #0074cc;
                    }
                </style>
            </head>
            <body>
                <div class="login-container">
                    <h1>Welcome to ZeroFN</h1>
                    <p>Click below to login with your Epic Games account</p>
                    <a href="/login">
                        <button class="login-btn">Login with Epic Games</button>
                    </a>
                </div>
            </body>
            </html>
            """
            self.wfile.write(html.encode())

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
                
                data = {
                    'grant_type': 'authorization_code',
                    'code': auth_code,
                    'client_id': client_id,
                    'client_secret': client_secret,
                    'redirect_uri': "http://localhost:8000/callback"
                }
                
                response = requests.post(token_url, data=data)
                
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
                    <body>
                        <h1>Login Successful!</h1>
                        <p>You can now close this window and return to ZeroFN.</p>
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
            self.wfile.write(b"Authentication failed. Please try again.")

def start_auth_server():
    server = HTTPServer(('localhost', 8000), AuthHandler)
    print("Authentication server started at http://localhost:8000")
    webbrowser.open('http://localhost:8000')
    server.serve_forever()

if __name__ == '__main__':
    start_auth_server()

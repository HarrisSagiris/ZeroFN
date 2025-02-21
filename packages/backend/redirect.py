from mitmproxy import ctx
from mitmproxy import http

def request(flow: http.HTTPFlow) -> None:
    # Only redirect specific Fortnite/Epic API endpoints that need to be handled locally
    target_endpoints = [
        "fortnite-game",
        "fortnite-content", 
        "account-public",
        "launcher-public",
        "epic-games-api",
        "lightswitch-public",
        "account/api/oauth/token",  # Auth endpoint
        "account/api/oauth/verify", # Auth verification endpoint
        "account/api/public/account" # Account info endpoint
    ]
    
    should_redirect = False
    for endpoint in target_endpoints:
        if endpoint in flow.request.pretty_url.lower():
            should_redirect = True
            break
            
    if should_redirect:
        # Get the original path
        original_path = flow.request.path
        
        # Redirect to local server
        flow.request.host = "0.0.0.0"
        flow.request.port = 7777
        flow.request.scheme = "http"
        
        # Keep the original path
        flow.request.path = original_path
        
        # Add headers needed for auth
        flow.request.headers["Authorization"] = "bearer test"  # Default test token
        
        ctx.log.info(f"Redirecting {flow.request.pretty_url} to 127.0.0.1:5595")
    else:
        # Let all other traffic pass through normally
        pass

addons = [
    request
]

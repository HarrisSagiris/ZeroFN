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
        "lightswitch-public"
    ]
    
    should_redirect = False
    for endpoint in target_endpoints:
        if endpoint in flow.request.pretty_url.lower():
            should_redirect = True
            break
            
    if should_redirect:
        # Get the original path
        original_path = flow.request.path
        
        # Redirect to localhost:3000
        flow.request.host = "localhost"
        flow.request.port = 3000
        flow.request.scheme = "http"
        
        # Keep the original path 
        flow.request.path = original_path
        
        ctx.log.info(f"Redirecting {flow.request.pretty_url} to localhost:3000")
    else:
        # Let all other traffic pass through normally
        pass

addons = [
    request
]

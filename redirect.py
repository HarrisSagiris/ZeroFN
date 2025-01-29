from mitmproxy import ctx
from mitmproxy import http

def request(flow: http.HTTPFlow) -> None:
    # Check if request is going to Fortnite/Epic Games API endpoints
    if "fortnite" in flow.request.pretty_url.lower() or "epicgames" in flow.request.pretty_url.lower():
        
        # Get the original path
        original_path = flow.request.path
        
        # Redirect to localhost:3000
        flow.request.host = "localhost" 
        flow.request.port = 3000
        flow.request.scheme = "http"
        
        # Keep the original path
        flow.request.path = original_path
        
        ctx.log.info(f"Redirecting {flow.request.pretty_url} to localhost:3000")

addons = [
    # Register the script
]

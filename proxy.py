from mitmproxy import ctx, http
import json
import base64
import secrets
from datetime import datetime, timezone

def request(flow: http.HTTPFlow) -> None:
    # Intercept all Fortnite authentication requests
    if 'epicgames.com' in flow.request.pretty_url or 'fortnite.com' in flow.request.pretty_url:
        ctx.log.info(f"Intercepted request to: {flow.request.pretty_url}")
        
        # Generate fake authentication response for token requests
        if '/account/api/oauth/token' in flow.request.pretty_url:
            fake_token = {
                "access_token": base64.b64encode(secrets.token_bytes(32)).decode(),
                "expires_in": 28800,
                "expires_at": (datetime.now(timezone.utc)).isoformat(),
                "token_type": "bearer",
                "refresh_token": base64.b64encode(secrets.token_bytes(16)).decode(),
                "account_id": "zerofn_" + secrets.token_hex(8),
                "client_id": "xyza7891TydzdNolyGQJYa9b6n6rLMJl",
                "internal_client": True,
                "client_service": "fortnite",
                "displayName": "ZeroFN Player",
                "app": "fortnite"
            }
            
            flow.response = http.Response.make(
                200,
                json.dumps(fake_token).encode(),
                {"Content-Type": "application/json"}
            )
        
        # Block other Epic endpoints
        elif any(x in flow.request.pretty_url for x in ['friends', 'presence', 'account/api/public']):
            flow.response = http.Response.make(404)

    # Redirect all Fortnite requests to local server
    if 'fortnite' in flow.request.pretty_url.lower():
        original_url = flow.request.pretty_url
        local_url = original_url.replace(
            'epicgames.com', '127.0.0.1:7778'
        ).replace(
            'fortnite.com', '127.0.0.1:7778'
        )
        flow.request.url = local_url
        ctx.log.info(f"Redirecting {original_url} to {local_url}")

def response(flow: http.HTTPFlow) -> None:
    # Add CORS headers to allow local connections
    if flow.response and flow.response.headers:
        flow.response.headers["Access-Control-Allow-Origin"] = "*"
        flow.response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS"
        flow.response.headers["Access-Control-Allow-Headers"] = "*"

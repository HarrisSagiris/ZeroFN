from mitmproxy import ctx, http
import json
import base64
import secrets
from datetime import datetime, timezone

def request(flow: http.HTTPFlow) -> None:
    # Load account info from auth_token.json
    try:
        with open('auth_token.json', 'r') as f:
            auth_data = json.load(f)
            account_id = auth_data.get('account_id', 'zerofn_' + secrets.token_hex(8))
            display_name = auth_data.get('display_name', 'ZeroFN Player')
            email = auth_data.get('email', 'zerofn@example.com')
    except:
        # Fallback values if file can't be read
        account_id = 'zerofn_' + secrets.token_hex(8)
        display_name = 'ZeroFN Player'
        email = 'zerofn@example.com'

    # Intercept all Fortnite authentication requests
    if 'epicgames.com' in flow.request.pretty_url or 'fortnite.com' in flow.request.pretty_url:
        ctx.log.info(f"[REQUEST] Intercepted request to: {flow.request.pretty_url}")
        ctx.log.info(f"[REQUEST] Headers: {dict(flow.request.headers)}")
        if flow.request.content:
            ctx.log.info(f"[REQUEST] Body: {flow.request.content.decode()}")
        
        # Generate fake authentication response for token requests
        if '/account/api/oauth/token' in flow.request.pretty_url:
            ctx.log.info("[AUTH] Generating fake OAuth token")
            fake_token = {
                "access_token": base64.b64encode(secrets.token_bytes(32)).decode(),
                "expires_in": 28800,
                "expires_at": (datetime.now(timezone.utc)).isoformat(),
                "token_type": "bearer",
                "refresh_token": base64.b64encode(secrets.token_bytes(16)).decode(),
                "account_id": account_id,
                "client_id": "xyza7891TydzdNolyGQJYa9b6n6rLMJl",
                "internal_client": True,
                "client_service": "fortnite",
                "displayName": display_name,
                "app": "fortnite",
                "in_app_id": account_id,
                "device_id": "zerofn_device_" + secrets.token_hex(8),
                "scope": "basic_profile friends_list presence"
            }
            flow.response = http.Response.make(
                200,
                json.dumps(fake_token).encode(),
                {"Content-Type": "application/json"}
            )

        # Handle OAuth verify endpoint
        elif '/account/api/oauth/verify' in flow.request.pretty_url:
            ctx.log.info("[AUTH] Handling OAuth verify request")
            flow.response = http.Response.make(
                200, 
                json.dumps({"token": "valid"}).encode()
            )

        # Handle account endpoints
        elif '/account/api/accounts' in flow.request.pretty_url:
            ctx.log.info("[ACCOUNT] Handling accounts request")
            flow.response = http.Response.make(
                200,
                json.dumps({
                    "id": account_id,
                    "displayName": display_name,
                    "email": email,
                    "failedLoginAttempts": 0,
                    "lastLogin": datetime.now(timezone.utc).isoformat(),
                    "numberOfDisplayNameChanges": 0,
                    "dateOfBirth": "1990-01-01",
                    "ageGroup": "ADULT",
                    "headless": False,
                    "country": "US",
                    "lastName": "Player",
                    "preferredLanguage": "en",
                    "canUpdateDisplayName": True,
                    "tfaEnabled": False,
                    "emailVerified": True,
                    "minorVerified": False,
                    "minorExpected": False,
                    "minorStatus": "UNKNOWN"
                }).encode()
            )

        # Handle profile endpoints
        elif '/fortnite/api/game/v2/profile' in flow.request.pretty_url:
            ctx.log.info("[PROFILE] Handling profile request")
            flow.response = http.Response.make(
                200,
                json.dumps({
                    "profileRevision": 1,
                    "profileId": "athena",
                    "profileChangesBaseRevision": 1,
                    "profileChanges": [],
                    "serverTime": datetime.now(timezone.utc).isoformat(),
                    "responseVersion": 1
                }).encode()
            )

        # Handle matchmaking endpoints
        elif '/fortnite/api/game/v2/matchmakingservice' in flow.request.pretty_url:
            ctx.log.info("[MATCHMAKING] Handling matchmaking request")
            flow.response = http.Response.make(200, json.dumps({"status": "MATCHING", "estimated_wait": 1}).encode())
        
        # Handle cloud storage endpoints
        elif '/fortnite/api/cloudstorage' in flow.request.pretty_url:
            ctx.log.info("[STORAGE] Handling cloud storage request")
            flow.response = http.Response.make(200, json.dumps([]).encode())
            
        # Handle party endpoints
        elif '/party/api' in flow.request.pretty_url:
            ctx.log.info("[PARTY] Handling party request")
            flow.response = http.Response.make(200, json.dumps({
                "current": [],
                "pending": [],
                "invites": [],
                "pings": []
            }).encode())

        # Handle catalog endpoints
        elif '/fortnite/api/storefront/v2/catalog' in flow.request.pretty_url:
            ctx.log.info("[CATALOG] Handling catalog request")
            flow.response = http.Response.make(200, json.dumps({"catalog": []}).encode())

        # Handle stats endpoints
        elif '/fortnite/api/stats/accountId' in flow.request.pretty_url:
            ctx.log.info("[STATS] Handling stats request")
            flow.response = http.Response.make(200, json.dumps({}).encode())

        # Handle other Epic endpoints with success responses
        elif 'friends' in flow.request.pretty_url:
            ctx.log.info("[FRIENDS] Handling friends request")
            flow.response = http.Response.make(200, json.dumps({"friends": []}).encode())
        elif 'presence' in flow.request.pretty_url:
            ctx.log.info("[PRESENCE] Handling presence request")
            flow.response = http.Response.make(200, json.dumps({"presence": {}}).encode())
        elif 'account/api/public' in flow.request.pretty_url:
            ctx.log.info("[PUBLIC] Handling public account request")
            flow.response = http.Response.make(200, json.dumps({
                "status": "ok",
                "response": {
                    "allowed": True,
                    "banned": False
                }
            }).encode())
        elif 'lightswitch/api/service/bulk/status' in flow.request.pretty_url:
            ctx.log.info("[STATUS] Handling service status request")
            flow.response = http.Response.make(200, json.dumps([{
                "serviceInstanceId": "fortnite",
                "status": "UP",
                "message": "Fortnite is online",
                "maintenanceUri": None,
                "allowedActions": ["PLAY", "DOWNLOAD"]
            }]).encode())
        elif 'waitingroom/api/waitingroom' in flow.request.pretty_url:
            ctx.log.info("[WAITROOM] Handling waiting room request")
            flow.response = http.Response.make(200, json.dumps({"hasWaitingRoom": False}).encode())
        else:
            # Handle any other Epic/Fortnite endpoints with a default success response
            ctx.log.info(f"[OTHER] Handling other request: {flow.request.pretty_url}")
            flow.response = http.Response.make(200, json.dumps({"status": "ok"}).encode())

        # Log the response we're sending back
        ctx.log.info(f"[RESPONSE] Status: {flow.response.status_code}")
        ctx.log.info(f"[RESPONSE] Headers: {dict(flow.response.headers)}")
        ctx.log.info(f"[RESPONSE] Body: {flow.response.content.decode()}")

    # Redirect all Fortnite requests to local server
    if 'fortnite' in flow.request.pretty_url.lower() or 'epicgames' in flow.request.pretty_url.lower():
        original_url = flow.request.pretty_url
        local_url = original_url.replace(
            'epicgames.com', '127.0.0.1:7777'
        ).replace(
            'fortnite.com', '127.0.0.1:7777'
        )
        flow.request.url = local_url
        ctx.log.info(f"[REDIRECT] {original_url} -> {local_url}")

def response(flow: http.HTTPFlow) -> None:
    # Add CORS headers to allow local connections
    if flow.response and flow.response.headers:
        flow.response.headers["Access-Control-Allow-Origin"] = "*"
        flow.response.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS"
        flow.response.headers["Access-Control-Allow-Headers"] = "*"

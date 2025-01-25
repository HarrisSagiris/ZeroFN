const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const crypto = require('crypto');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
const port = 7777;

// Configure logging with more detail
const logRequest = (req, res, next) => {
    console.log(`[${new Date().toISOString()}] ${req.method} ${req.url}`);
    console.log('[REQUEST] Headers:', req.headers);
    console.log('[REQUEST] Body:', req.body);
    next();
};

// Enhanced middleware
app.use(logRequest);
app.use(bodyParser.json());
app.use(cors({
    origin: '*',
    methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
    allowedHeaders: ['Content-Type', 'Authorization']
}));

// Store active sessions and profiles with TTL
const sessions = new Map();
const profiles = new Map();

// Enhanced ID generator with prefix
const generateId = (prefix = '') => prefix + crypto.randomBytes(16).toString('hex');

// Expanded profile template with more cosmetics
const defaultProfile = {
    accountId: '',
    displayName: '',
    avatar: 'cid_001_athena_commando_f_default',
    level: 100,
    battlePass: {
        level: 100,
        xp: 999999,
        selfBoostXp: 0,
        friendBoostXp: 0
    },
    vbucks: 13500,
    items: [
        'cid_001_athena_commando_f_default',
        'bid_001_blackshield', 
        'pickaxe_lockjaw',
        'glider_founder_pack_1',
        'eid_floss',
        'wrap_001'
    ],
    stats: {
        wins: 500,
        kills: 10000,
        matches: 2000,
        kd: 5.0
    }
};

// Enhanced responses with more realistic data
const defaultResponses = {
    account: (accountId, displayName) => ({
        status: "ok",
        response: {
            accountId: accountId,
            displayName: displayName,
            email: "placeholder@email.com",
            failedLoginAttempts: 0,
            lastLogin: new Date().toISOString(),
            numberOfDisplayNameChanges: 0,
            ageGroup: "UNKNOWN",
            headless: false,
            country: "US",
            lastName: "User",
            preferredLanguage: "en",
            canUpdateDisplayName: true,
            tfaEnabled: false,
            allowedActions: ["PLAY", "DOWNLOAD"],
            allowed: true,
            banned: false,
            externalAuths: {}
        }
    }),
    catalog: {
        catalog: [
            {
                catalogId: "BRWeeklyStorefront",
                displayName: "Battle Royale Store",
                items: [
                    {
                        id: "renegade_raider",
                        name: "Renegade Raider",
                        price: 1200,
                        type: "outfit"
                    },
                    {
                        id: "skull_trooper", 
                        name: "Skull Trooper",
                        price: 1500,
                        type: "outfit"
                    }
                ]
            }
        ]
    },
    party: {
        current: [],
        pending: [],
        invites: [],
        pings: [],
        config: {
            privacy: "PUBLIC",
            joinConfirmation: false,
            joinability: "OPEN",
            maxSize: 16,
            subType: "default",
            type: "default",
            inviteTTL: 14400,
            chatEnabled: true
        }
    },
    stats: (accountId) => ({
        accountId: accountId,
        stats: {
            br: {
                wins: 500,
                kills: 10000,
                matches: 2000,
                kd: "5.0",
                minutesPlayed: 12000,
                playersOutlived: 50000,
                lastModified: new Date().toISOString()
            }
        }
    }),
    friends: {
        friends: [],
        incoming: [],
        outgoing: [],
        blocklist: [],
        settings: {
            acceptInvites: "public"
        }
    },
    presence: {
        presence: {
            status: "online",
            gameplayState: "inGame", 
            location: "lobby",
            party: {
                isPrivate: false,
                maxSize: 16
            }
        }
    },
    status: [{
        serviceInstanceId: "fortnite",
        status: "UP",
        message: "Fortnite is online",
        maintenanceUri: null,
        allowedActions: ["PLAY", "DOWNLOAD"],
        banned: false,
        launcherInfoDTO: {
            appName: "Fortnite",
            catalogItemId: "4fe75bbc5a674f4f9b356b5c90567da5",
            namespace: "fn"
        }
    }],
    waitingroom: {
        hasWaitingRoom: false,
        waitingRoomStatus: "NONE"
    }
};

// Enhanced auth middleware with token validation
const requireAuth = (req, res, next) => {
    const token = req.headers.authorization;
    if (!token) {
        console.log('[AUTH] Unauthorized request - no token');
        return res.status(401).json({ error: "Unauthorized", errorCode: "auth_failed" });
    }
    const session = sessions.get(token);
    if (!session) {
        console.log('[AUTH] Invalid session token');
        return res.status(401).json({ error: "Invalid session", errorCode: "session_invalid" });
    }
    req.session = session;
    console.log(`[AUTH] Authenticated request for user ${session.displayName}`);
    next();
};

// Enhanced auth routes with better error handling
app.post('/account/api/oauth/token', (req, res) => {
    try {
        const accountId = generateId('acc_');
        const displayName = req.body.username || `User${Math.floor(Math.random() * 10000)}`;
        const token = generateId('token_');
        
        const session = {
            accountId,
            displayName,
            token,
            created: new Date().toISOString(),
            expires: new Date(Date.now() + 28800000).toISOString()
        };
        
        sessions.set(token, session);
        profiles.set(accountId, {...defaultProfile, accountId, displayName});
        
        console.log(`[LOGIN] New login for user ${displayName} (${accountId})`);
        
        res.json({
            access_token: token,
            account_id: accountId, 
            displayName: displayName,
            expires_in: 28800,
            token_type: "bearer",
            client_id: "ec684b8c687f479fadea3cb2ad83f5c6",
            internal_client: true,
            client_service: "fortnite"
        });
    } catch (error) {
        console.error('[LOGIN] Error:', error);
        res.status(500).json({ error: "Internal server error", errorCode: "server_error" });
    }
});

// Main routes with enhanced error handling
app.all('/account/api/public/*', requireAuth, (req, res) => {
    console.log('[ACCOUNT] Handling public account request');
    res.json(defaultResponses.account(req.session.accountId, req.session.displayName));
});

app.all('/fortnite/api/storefront/v2/catalog', requireAuth, (req, res) => {
    console.log('[CATALOG] Handling catalog request');
    res.json(defaultResponses.catalog);
});

app.all('/party/api/*', requireAuth, (req, res) => {
    console.log('[PARTY] Handling party request');
    res.json(defaultResponses.party);
});

app.all('/fortnite/api/stats/accountId/:accountId', requireAuth, (req, res) => {
    console.log(`[STATS] Handling stats request for ${req.params.accountId}`);
    res.json(defaultResponses.stats(req.params.accountId));
});

app.all('/fortnite/api/game/v2/profile/:accountId/client/:command', requireAuth, (req, res) => {
    console.log(`[PROFILE] Handling profile ${req.params.command} for ${req.params.accountId}`);
    const profile = profiles.get(req.params.accountId) || profiles.get(req.session.accountId);
    res.json({
        profileId: 'athena',
        profileChanges: [{
            changeType: 'fullProfileUpdate',
            profile: profile
        }],
        serverTime: new Date().toISOString(),
        responseVersion: 1
    });
});

app.all('/friends/*', requireAuth, (req, res) => {
    console.log('[FRIENDS] Handling friends request');
    res.json(defaultResponses.friends);
});

app.all('/presence/*', requireAuth, (req, res) => {
    console.log('[PRESENCE] Handling presence request');
    res.json(defaultResponses.presence);
});

app.all('/lightswitch/api/service/bulk/status', (req, res) => {
    console.log('[STATUS] Handling service status request');
    res.json(defaultResponses.status);
});

app.all('/waitingroom/api/waitingroom', (req, res) => {
    console.log('[WAITROOM] Handling waiting room request');
    res.json(defaultResponses.waitingroom);
});

// Default handler for all other routes
app.all('*', (req, res) => {
    console.log(`[REQUEST] ${req.method} ${req.url}`);
    res.json({ status: "ok" });
});

// Enhanced Fortnite launcher with better error handling
const launchFortnite = () => {
    console.log('[LAUNCHER] Starting Fortnite launch sequence...');
    
    const pythonProcess = spawn('python', ['run.py'], {
        cwd: path.join(__dirname, '..'),
        stdio: 'pipe'
    });

    pythonProcess.stdout.on('data', (data) => {
        console.log(`[PYTHON] ${data.toString().trim()}`);
    });

    pythonProcess.stderr.on('data', (data) => {
        console.error(`[PYTHON ERROR] ${data.toString().trim()}`);
    });

    pythonProcess.on('close', (code) => {
        console.log(`[LAUNCHER] Python process exited with code ${code}`);
    });

    pythonProcess.on('error', (error) => {
        console.error('[LAUNCHER] Failed to start Python process:', error);
    });
};

// Start server with enhanced logging
app.listen(port, () => {
    console.log(`[SERVER] ZeroFN backend running on port ${port}`);
    console.log(`[SERVER] Enhanced authentication and profile system active`);
    console.log(`[SERVER] Server ready to handle Fortnite requests`);
    launchFortnite();
});
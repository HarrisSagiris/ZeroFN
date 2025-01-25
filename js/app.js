const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const crypto = require('crypto');
const { spawn } = require('child_process');
const path = require('path');

const app = express();
const port = 7777;

// Basic logging middleware
app.use((req, res, next) => {
    console.log(`${req.method} ${req.url}`);
    next();
});

app.use(bodyParser.json());
app.use(cors());

// Simple in-memory storage
const sessions = new Map();
const profiles = new Map();

// Basic profile template
const defaultProfile = {
    accountId: '',
    displayName: '',
    items: [
        'AthenaCharacter:CID_001_Athena_Commando_F_Default',
        'AthenaBackpack:BID_001_BlackShield',
        'AthenaPickaxe:DefaultPickaxe',
        'AthenaDance:EID_Floss'
    ],
    stats: {
        wins: 0,
        kills: 0
    }
};

// Auth endpoints
app.post('/account/api/oauth/token', (req, res) => {
    const accountId = crypto.randomBytes(16).toString('hex');
    const token = crypto.randomBytes(32).toString('hex');
    const displayName = req.body.username || `User${Math.floor(Math.random() * 1000)}`;
    
    sessions.set(token, {
        accountId,
        displayName,
        token
    });
    
    profiles.set(accountId, {
        ...defaultProfile,
        accountId,
        displayName
    });
    
    res.json({
        access_token: token,
        account_id: accountId,
        displayName,
        expires_in: 28800,
        token_type: "bearer"
    });
});

// Profile endpoint
app.post('/fortnite/api/game/v2/profile/:accountId/client/:command', (req, res) => {
    const profile = profiles.get(req.params.accountId) || defaultProfile;
    
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

// Matchmaking endpoints
app.get('/fortnite/api/matchmaking/session/:sessionId', (req, res) => {
    res.json({
        id: req.params.sessionId,
        ownerId: "server",
        ownerName: "Server",
        serverAddress: "127.0.0.1",
        serverPort: 7777,
        maxPublicPlayers: 100,
        openPublicPlayers: 100,
        attributes: {
            REGION_s: "NAE",
            GAMEMODE_s: "FORTATHENA",
            ALLOWBROADCASTING_b: true,
            SUBREGION_s: "NONE",
            DCID_s: "FORTNITE-LIVE",
            NEEDS_i: 0
        },
        publicPlayers: []
    });
});

// Status endpoints
app.get('/lightswitch/api/service/bulk/status', (req, res) => {
    res.json([{
        serviceInstanceId: "fortnite",
        status: "UP",
        message: "Fortnite is online",
        allowedActions: ["PLAY", "DOWNLOAD", "LOGIN", "PURCHASE"],
        banned: false,
        launcherInfoDTO: {
            appName: "Fortnite",
            catalogItemId: "4fe75bbc5a674f4f9b356b5c90567da5",
            namespace: "fn"
        }
    }]);
});

app.get('/waitingroom/api/waitingroom', (req, res) => {
    res.status(204).end();
});

app.get('/fortnite/api/game/v2/enabled_features', (req, res) => {
    res.json([]);
});

app.get('/fortnite/api/version', (req, res) => {
    res.json({ "version": "1.0.0", "cln": "1337" });
});

app.get('/fortnite/api/v2/versioncheck/*', (req, res) => {
    res.json({ "type": "NO_UPDATE" });
});

app.get('/fortnite/api/storefront/v2/catalog', (req, res) => {
    res.json({
        catalog: {
            refreshIntervalHrs: 24,
            dailyPurchaseHrs: 24,
            expiration: new Date(Date.now() + 86400000).toISOString(),
            storefronts: []
        }
    });
});

app.get('/fortnite/api/calendar/v1/timeline', (req, res) => {
    res.json({
        channels: {
            "standalone-store": {},
            "client-events": {
                states: [{
                    validFrom: "2020-01-01T20:28:47.830Z",
                    activeEvents: []
                }]
            },
            "tk": {},
            "featured-islands": {},
            "community-votes": {},
            "client-matchmaking": {},
            "client-events-2": {}
        },
        currentTime: new Date().toISOString(),
        cacheIntervalMins: 10,
        eventsTimeOffsetHrs: 0
    });
});

// Catch-all for other endpoints
app.all('*', (req, res) => {
    res.json({ status: "ok" });
});

// Start server and launch game
app.listen(port, () => {
    console.log(`Fortnite private server running on port ${port}`);
    
    // Launch the game
    const pythonProcess = spawn('python', ['run.py'], {
        cwd: path.join(__dirname, '..'),
        stdio: 'pipe'
    });

    pythonProcess.stdout.on('data', (data) => {
        console.log(`[LAUNCHER] ${data}`);
    });

    pythonProcess.stderr.on('data', (data) => {
        console.error(`[LAUNCHER ERROR] ${data}`);
    });
});
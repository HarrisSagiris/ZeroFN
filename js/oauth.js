const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const crypto = require('crypto');

const app = express();
const port = 3000;

// Middleware
app.use(bodyParser.json());
app.use(cors());

// In-memory storage
const sessions = new Map();

// OAuth endpoints
app.post('/account/api/oauth/token', (req, res) => {
    const accountId = crypto.randomBytes(16).toString('hex');
    const token = crypto.randomBytes(32).toString('hex');
    const displayName = req.body.username || `User${Math.floor(Math.random() * 1000)}`;

    // Store session
    sessions.set(token, {
        accountId,
        displayName,
        token
    });

    // Return OAuth response
    res.json({
        access_token: token,
        account_id: accountId,
        client_id: 'fn',
        expires_in: 28800,
        token_type: 'bearer',
        displayName: displayName,
        app: 'fortnite',
        in_app_id: accountId,
        device_id: 'aabbccddeeff11223344556677889900',
        expires_at: new Date(Date.now() + 28800000).toISOString(),
        refresh_token: token,
        refresh_expires: 115200,
        refresh_expires_at: new Date(Date.now() + 115200000).toISOString(),
        account_name: displayName,
        merged_accounts: [],
        display_name: displayName,
        first_name: '',
        last_name: '',
        email: '',
        failedLoginAttempts: 0,
        lastFailedLogin: new Date().toISOString(),
        lastLogin: new Date().toISOString(),
        numberOfDisplayNameChanges: 0,
        ageGroup: 'UNKNOWN',
        headless: false,
        country: 'US',
        preferredLanguage: 'en',
        lastDisplayNameChange: new Date().toISOString(),
        canUpdateDisplayName: true,
        tfaEnabled: false,
        emailVerified: true,
        minorVerified: false,
        minorExpected: false,
        minorStatus: 'UNKNOWN'
    });
});

app.get('/account/api/oauth/verify', (req, res) => {
    const token = req.headers.authorization?.split(' ')[1];
    
    if (!token || !sessions.has(token)) {
        return res.status(401).json({ error: 'Invalid token' });
    }

    const session = sessions.get(token);
    res.json({
        token,
        session_id: session.accountId,
        token_type: 'bearer',
        client_id: 'fn',
        internal_client: true,
        client_service: 'fortnite',
        displayName: session.displayName,
        app: 'fortnite',
        expires_in: 28800,
        expires_at: new Date(Date.now() + 28800000).toISOString(),
        auth_method: 'exchange_code',
        scope: [],
        device_id: 'aabbccddeeff11223344556677889900',
        account_id: session.accountId,
        merged_accounts: [],
        display_name: session.displayName,
        first_name: '',
        last_name: '',
        email: '',
        failedLoginAttempts: 0,
        lastFailedLogin: new Date().toISOString(),
        lastLogin: new Date().toISOString(),
        numberOfDisplayNameChanges: 0,
        ageGroup: 'UNKNOWN',
        headless: false,
        country: 'US',
        preferredLanguage: 'en',
        lastDisplayNameChange: new Date().toISOString(),
        canUpdateDisplayName: true,
        tfaEnabled: false,
        emailVerified: true,
        minorVerified: false,
        minorExpected: false,
        minorStatus: 'UNKNOWN'
    });
});

// Start server
app.listen(port, () => {
    console.log(`OAuth server running on port ${port}`);
});

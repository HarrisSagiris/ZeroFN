import express from "express"
import fs from "fs"
import path from "path"
import net from "net"

const app = express();
const port = 3000; // Changed to match LOCAL_PORT in zerofndll.cpp
const host = '127.0.0.1'; // Changed to match LOCAL_SERVER in zerofndll.cpp

// Middleware to parse JSON bodies
app.use(express.json());

// Enable CORS for all routes
app.use((req, res, next) => {
  res.header('Access-Control-Allow-Origin', '*');
  res.header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS');
  res.header('Access-Control-Allow-Headers', '*');
  if (req.method === 'OPTIONS') {
    return res.sendStatus(200);
  }
  next();
});

// Load database
let database = {
  users: {},
  cosmetics: [] as any[]
};

console.log('Initializing ZeroFN Backend...');

// Load initial cosmetics from database.json if exists
try {
  console.log('Loading cosmetics database...');
  const cosmeticsData = fs.readFileSync(path.join(__dirname, 'database.json'), 'utf8');
  database.cosmetics = JSON.parse(cosmeticsData).characters || [];
  console.log(`Loaded ${database.cosmetics.length} cosmetic items`);
} catch (err) {
  console.log('No existing database.json found, starting with empty cosmetics list');
}

// Save database helper function
const saveDatabase = () => {
  console.log('Saving user data to database...');
  fs.writeFileSync(
    path.join(__dirname, 'userdata.json'),
    JSON.stringify(database, null, 2)
  );
  console.log('Database saved successfully');
};

// Create TCP server to verify DLL connection
const tcpServer = net.createServer((socket) => {
  console.log('ZeroFN DLL connected to backend');
  socket.on('data', (data) => {
    console.log('Received data from DLL:', data.toString());
  });
  socket.on('end', () => {
    console.log('ZeroFN DLL disconnected');
  });
});

tcpServer.listen(3001, host, () => {
  console.log('TCP server listening for DLL connections on port 3001');
});

// Authentication bypass endpoints
app.get('/account/api/oauth/verify', (req, res) => {
  console.log('Client connected! Verifying authentication...');
  res.json({
    access_token: "eg1~-*",
    expires_in: 28800,
    token_type: "bearer", 
    refresh_token: "eg1~-*",
    refresh_expires: 115200,
    account_id: "ninja",
    client_id: "ec684b8c687f479fadea3cb2ad83f5c6", // Match client_id from zerofndll.cpp
    internal_client: true,
    client_service: "fortnite",
    displayName: "ZeroFN", // Match displayName from zerofndll.cpp
    app: "fortnite",
    in_app_id: "ninja",
    device_id: "164fb25bb44e42c5a027977d0d5da800"
  });
  console.log('Client authentication verified successfully');
});

app.post('/account/api/oauth/token', (req, res) => {
  console.log('Client requesting auth token...');
  // Generate random strings like in zerofndll.cpp
  const randomString = (length: number) => {
    const chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    let result = '';
    for(let i = 0; i < length; i++) {
      result += chars[Math.floor(Math.random() * chars.length)];
    }
    return result;
  };

  res.json({
    access_token: `eg1~${randomString(128)}`,
    expires_in: 28800,
    token_type: "bearer",
    refresh_token: randomString(32),
    refresh_expires: 115200,
    account_id: randomString(32),
    client_id: "ec684b8c687f479fadea3cb2ad83f5c6",
    internal_client: true,
    client_service: "fortnite", 
    displayName: "ZeroFN",
    app: "fortnite",
    in_app_id: randomString(32)
  });
  console.log('Auth token generated and sent to client');
});

app.get('/account/api/public/account/:accountId', (req, res) => {
  console.log(`Client requesting account info for ID: ${req.params.accountId}`);
  res.json({
    id: "ninja",
    displayName: "ZeroFN",
    name: "ZeroFN",
    email: "zerofn@zerofn.com",
    failedLoginAttempts: 0,
    lastLogin: new Date().toISOString(),
    numberOfDisplayNameChanges: 0,
    ageGroup: "UNKNOWN",
    headless: false,
    country: "US",
    lastName: "ZeroFN",
    preferredLanguage: "en",
    canUpdateDisplayName: false,
    tfaEnabled: false,
    emailVerified: true,
    minorVerified: false,
    minorExpected: false,
    minorStatus: "UNKNOWN",
    cabinedMode: false,
    hasHashedEmail: false,
    externalAuths: {}
  });
  console.log('Account info sent to client');
});

// Version check endpoints matching zerofndll.cpp responses
app.get('/fortnite/api/version', (req, res) => {
  console.log('Client checking game version...');
  res.json({
    type: 'NO_UPDATE',
    version: '++Fortnite+Release-Cert-CL-3807424',
    buildDate: '2023-01-01'
  });
  console.log('Version check completed');
});

app.get('/fortnite/api/versioncheck/:version', (req, res) => {
  console.log(`Client version check for: ${req.params.version}`);
  res.json({
    type: "NO_UPDATE",
    acceptedVersion: "++Fortnite+Release-Cert-CL-3807424",
    updateUrl: null,
    requiredVersion: "NONE",
    updatePriority: 0,
    message: "No update required."
  });
  console.log('Version compatibility confirmed');
});

// User cosmetics endpoints
app.get('/fortnite/api/cloudstorage/user/:accountId', (req, res) => {
  console.log(`Client requesting cloud storage for account: ${req.params.accountId}`);
  res.json([{
    uniqueFilename: "DefaultGame.ini",
    filename: "DefaultGame.ini",
    hash: randomString(32),
    hash256: randomString(64),
    length: 1234,
    contentType: "application/octet-stream",
    uploaded: "2025-01-31T20:57:02.000Z",
    storageType: "S3",
    doNotCache: false
  }]);
  console.log('Cloud storage data sent to client');
});

app.post('/fortnite/api/cloudstorage/user/:accountId/:uniqueFilename', (req, res) => {
  console.log(`Client updating cloud storage: ${req.params.uniqueFilename}`);
  res.status(204).send();
  console.log('Cloud storage update acknowledged');
});

// Catalog endpoints
app.get('/fortnite/api/storefront/v2/catalog', (req, res) => {
  console.log('Client requesting store catalog...');
  res.json({
    catalog: []
  });
  console.log('Empty catalog sent to client');
});

// Profile endpoints
app.post('/fortnite/api/game/v2/profile/:accountId/client/:command', (req, res) => {
  const { accountId, command } = req.params;
  const profileId = req.query.profileId || 'athena';

  console.log(`Client ${accountId} requesting profile command: ${command}`);

  const baseResponse = {
    profileRevision: 1,
    profileId: profileId,
    profileChangesBaseRevision: 1,
    profileChanges: [] as any[],
    serverTime: new Date().toISOString(),
    responseVersion: 1
  };

  switch (command) {
    case 'QueryProfile':
      console.log('Processing QueryProfile request...');
      // Add default cosmetic items
      const items: any = {
        // Default character
        "CID_Default": {
          "templateId": "AthenaCharacter:CID_001_Athena_Commando_F_Default",
          "attributes": {
            "favorite": false,
            "item_seen": true,
            "level": 1,
            "variants": [],
            "xp": 0
          },
          "quantity": 1
        },
        // Default pickaxe
        "DefaultPickaxe": {
          "templateId": "AthenaPickaxe:DefaultPickaxe",
          "attributes": {
            "favorite": false,
            "item_seen": true,
            "level": 1,
            "variants": [],
            "xp": 0
          },
          "quantity": 1
        },
        // Default backpack
        "BID_Default": {
          "templateId": "AthenaBackpack:BID_001_Default",
          "attributes": {
            "favorite": false,
            "item_seen": true,
            "level": 1,
            "variants": [],
            "xp": 0
          },
          "quantity": 1
        }
      };

      // Add any loaded cosmetics from database
      console.log('Adding custom cosmetics from database...');
      database.cosmetics.forEach(cosmetic => {
        items[cosmetic.id] = {
          templateId: cosmetic.templateId,
          attributes: {
            favorite: false,
            item_seen: true,
            level: 1,
            variants: [],
            xp: 0
          },
          quantity: 1
        };
      });

      baseResponse.profileChanges.push({
        changeType: 'fullProfileUpdate',
        profile: {
          _id: accountId,
          accountId: accountId,
          profileId: profileId,
          version: 'no_version',
          items: items,
          stats: {
            attributes: {
              past_seasons: [],
              season_match_boost: 0,
              loadouts: ["loadout_0"],
              mfa_reward_claimed: true,
              rested_xp_overflow: 0,
              quest_manager: {},
              book_level: 1,
              season_num: 20,
              book_xp: 0,
              permissions: [],
              season: {
                numWins: 0,
                numHighBracket: 0,
                numLowBracket: 0
              }
            }
          }
        }
      });
      console.log('QueryProfile response prepared');
      break;
    case 'ClientQuestLogin':
    case 'RefreshExpeditions':
    case 'SetMtxPlatform':
    case 'SetItemFavoriteStatusBatch':
    case 'EquipBattleRoyaleCustomization':
      console.log(`Processing ${command} request...`);
      baseResponse.profileChanges.push({
        changeType: 'fullProfileUpdate',
        profile: {
          _id: accountId,
          accountId: accountId,
          profileId: profileId,
          version: 'no_version',
          items: {},
          stats: {
            attributes: {
              past_seasons: [],
              season_match_boost: 0,
              loadouts: ["loadout_0"],
              mfa_reward_claimed: true,
              rested_xp_overflow: 0,
              quest_manager: {},
              book_level: 1,
              season_num: 20,
              book_xp: 0,
              permissions: [],
              season: {
                numWins: 0,
                numHighBracket: 0,
                numLowBracket: 0
              }
            }
          }
        }
      });
      console.log(`${command} response prepared`);
      break;
    default:
      console.log(`Warning: Unknown command received: ${command}`);
  }

  res.json(baseResponse);
  console.log(`Response sent for command: ${command}`);
});

// Start server
app.listen(port, host, () => {
  console.log(`ZeroFN Backend running on ${host}:${port}`);
  console.log('Server is ready to accept connections from ZeroFN DLL!');
});

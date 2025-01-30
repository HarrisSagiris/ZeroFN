const express = require('express');
const fs = require('fs');
const path = require('path');
const app = express();
const port = 7777;
const host = '127.0.0.1';

// Middleware to parse JSON bodies
app.use(express.json());

// Load database
let database = {
  users: {},
  cosmetics: []
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
    client_id: "3446cd72694c4a4485d81b77adbb2141",
    internal_client: true,
    client_service: "fortnite",
    displayName: "Ninja",
    app: "fortnite",
    in_app_id: "ninja",
    device_id: "164fb25bb44e42c5a027977d0d5da800"
  });
  console.log('Client authentication verified successfully');
});

app.post('/account/api/oauth/token', (req, res) => {
  console.log('Client requesting auth token...');
  res.json({
    access_token: "eg1~-*",
    expires_in: 28800,
    token_type: "bearer",
    refresh_token: "eg1~-*", 
    refresh_expires: 115200,
    account_id: "ninja",
    client_id: "3446cd72694c4a4485d81b77adbb2141",
    internal_client: true,
    client_service: "fortnite",
    displayName: "Ninja",
    app: "fortnite",
    in_app_id: "ninja",
    device_id: "164fb25bb44e42c5a027977d0d5da800"
  });
  console.log('Auth token generated and sent to client');
});

app.get('/account/api/public/account/:accountId', (req, res) => {
  console.log(`Client requesting account info for ID: ${req.params.accountId}`);
  res.json({
    id: "ninja",
    displayName: "Ninja",
    name: "Ninja",
    email: "ninja@ninja.com",
    failedLoginAttempts: 0,
    lastLogin: new Date().toISOString(),
    numberOfDisplayNameChanges: 0,
    ageGroup: "UNKNOWN",
    headless: false,
    country: "US",
    lastName: "Ninja",
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

// Version check endpoints
app.get('/fortnite/api/version', (req, res) => {
  console.log('Client checking game version...');
  res.json({
    type: 'NO_UPDATE',
    version: '++Fortnite+Release-20.00-CL-19458861',
    buildDate: '2023-01-01'
  });
  console.log('Version check completed');
});

app.get('/fortnite/api/versioncheck/:version', (req, res) => {
  console.log(`Client version check for: ${req.params.version}`);
  res.json({
    type: 'NO_UPDATE'
  });
  console.log('Version compatibility confirmed');
});

// User cosmetics endpoints
app.get('/fortnite/api/cloudstorage/user/:accountId', (req, res) => {
  console.log(`Client requesting cloud storage for account: ${req.params.accountId}`);
  res.json([{
    "uniqueFilename": "ClientSettings.Sav",
    "filename": "ClientSettings.Sav",
    "hash": "603E6907392C7212C4A7642D3247552A",
    "hash256": "973124FFC4A03E66D6A4458E587D5D6146F71FC57F359C8D516E0B12A50D96E0",
    "length": 0,
    "contentType": "application/octet-stream",
    "uploaded": "2023-01-01T00:00:00.000Z",
    "storageType": "S3",
    "doNotCache": false
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
    profileChanges: [],
    serverTime: new Date().toISOString(),
    responseVersion: 1
  };

  switch(command) {
    case 'QueryProfile':
      console.log('Processing QueryProfile request...');
      // Add default cosmetic items
      const items = {
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
  console.log('Server is ready to accept connections!');
});

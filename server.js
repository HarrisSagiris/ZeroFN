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

// Load initial cosmetics from database.json if exists
try {
  const cosmeticsData = fs.readFileSync(path.join(__dirname, 'database.json'), 'utf8');
  database.cosmetics = JSON.parse(cosmeticsData).characters || [];
} catch (err) {
  console.log('No existing database.json found, starting with empty cosmetics list');
}

// Save database helper function
const saveDatabase = () => {
  fs.writeFileSync(
    path.join(__dirname, 'userdata.json'),
    JSON.stringify(database, null, 2)
  );
};

// Authentication endpoints
app.get('/account/api/oauth/verify', (req, res) => {
  res.json({
    access_token: "zerofnaccesstoken",
    expires_in: 28800,
    token_type: "bearer",
    refresh_token: "zerofnrefreshtoken",
    refresh_expires: 115200,
    account_id: "zerofnaccount",
    client_id: "zerofnclient",
    internal_client: true,
    client_service: "fortnite",
    displayName: "ZeroFN User",
    app: "fortnite",
    in_app_id: "zerofnaccount"
  });
});

app.post('/account/api/oauth/token', (req, res) => {
  res.json({
    access_token: "zerofnaccesstoken",
    expires_in: 28800,
    token_type: "bearer",
    refresh_token: "zerofnrefreshtoken",
    refresh_expires: 115200,
    account_id: "zerofnaccount",
    client_id: "zerofnclient",
    internal_client: true,
    client_service: "fortnite",
    displayName: "ZeroFN User",
    app: "fortnite",
    in_app_id: "zerofnaccount"
  });
});

app.get('/account/api/public/account/:accountId', (req, res) => {
  res.json({
    id: "zerofnaccount",
    displayName: "ZeroFN User",
    externalAuths: {}
  });
});

app.get('/fortnite/api/game/v2/enabled_features', (req, res) => {
  res.json([]);
});

// Version check endpoints
app.get('/fortnite/api/version', (req, res) => {
  res.json({
    type: 'NO_UPDATE',
    version: '++Fortnite+Release-20.00-CL-19458861',
    buildDate: '2023-01-01'
  });
});

app.get('/fortnite/api/versioncheck/:version', (req, res) => {
  res.json({
    type: 'NO_UPDATE'
  });
});

// User cosmetics endpoints
app.get('/fortnite/api/cloudstorage/user/:accountId', (req, res) => {
  const { accountId } = req.params;
  const userCosmetics = database.users[accountId]?.cosmetics || [];
  res.json(userCosmetics);
});

app.post('/fortnite/api/cloudstorage/user/:accountId', (req, res) => {
  const { accountId } = req.params;
  const { cosmetics } = req.body;
  
  if (!database.users[accountId]) {
    database.users[accountId] = {
      cosmetics: []
    };
  }
  
  database.users[accountId].cosmetics = cosmetics;
  saveDatabase();
  
  res.json({ status: 'success' });
});

// Catalog endpoints
app.get('/fortnite/api/storefront/v2/catalog', (req, res) => {
  res.json({
    catalog: database.cosmetics.map(cosmeticId => ({
      id: cosmeticId,
      price: 0,
      unlocked: true
    }))
  });
});

// Profile endpoints
app.post('/fortnite/api/game/v2/profile/:accountId/client/:command', (req, res) => {
  const { accountId, command } = req.params;
  const profileId = req.query.profileId || 'athena';
  const rvn = req.query.rvn || -1;
  
  // Initialize user if not exists
  if (!database.users[accountId]) {
    database.users[accountId] = {
      cosmetics: database.cosmetics // Give all cosmetics by default
    };
    saveDatabase();
  }

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
    case 'ClientQuestLogin': 
    case 'RefreshExpeditions':
    case 'SetMtxPlatform':
    case 'SetItemFavoriteStatusBatch':
    case 'EquipBattleRoyaleCustomization':
      baseResponse.profileChanges.push({
        changeType: 'fullProfileUpdate',
        profile: {
          _id: accountId,
          accountId: accountId,
          profileId: profileId,
          version: 'no_version',
          items: database.users[accountId].cosmetics.reduce((acc, id) => {
            acc[id] = {
              templateId: id,
              attributes: {
                max_level_bonus: 0,
                level: 1,
                item_seen: true,
                xp: 0,
                variants: [],
                favorite: false
              },
              quantity: 1
            };
            return acc;
          }, {}),
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
      break;
    default:
      // Handle unknown commands
      console.log(`Unknown command: ${command}`);
  }
  
  res.json(baseResponse);
});

// Start server
app.listen(port, host, () => {
  console.log(`Fake Epic Games API running on ${host}:${port}`);
});

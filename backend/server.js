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
  console.log('Received verify request');
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
  console.log('Received token request');
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
  console.log('Received account info request');
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
  console.log('Received cloudstorage request');
  res.json([]);
});

app.post('/fortnite/api/cloudstorage/user/:accountId', (req, res) => {
  console.log('Received cloudstorage update');
  res.status(204).send();
});

// Catalog endpoints
app.get('/fortnite/api/storefront/v2/catalog', (req, res) => {
  res.json({
    catalog: []
  });
});

// Profile endpoints
app.post('/fortnite/api/game/v2/profile/:accountId/client/:command', (req, res) => {
  const { accountId, command } = req.params;
  const profileId = req.query.profileId || 'athena';
  
  console.log(`Received profile ${command} request`);

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
      break;
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
      break;
    default:
      console.log(`Unknown command: ${command}`);
  }
  
  res.json(baseResponse);
});

// Start server
app.listen(port, host, () => {
  console.log(`ZeroFN Backend running on ${host}:${port}`);
});

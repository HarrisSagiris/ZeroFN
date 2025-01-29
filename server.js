const express = require('express');
const fs = require('fs');
const path = require('path');
const app = express();
const port = 3000;

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
  
  // Initialize user if not exists
  if (!database.users[accountId]) {
    database.users[accountId] = {
      cosmetics: database.cosmetics // Give all cosmetics by default
    };
    saveDatabase();
  }

  // Handle different profile commands
  let response = {
    profileRevision: 1,
    profileId: 'athena',
    profileChanges: []
  };

  switch(command) {
    case 'QueryProfile':
    case 'ClientQuestLogin':
    case 'RefreshExpeditions':
      response.profileChanges.push({
        changeType: 'fullProfileUpdate',
        profile: {
          _id: accountId,
          accountId: accountId,
          profileId: 'athena',
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
              }
            };
            return acc;
          }, {})
        }
      });
      break;
  }
  
  res.json(response);
});

// Start server
app.listen(port, () => {
  console.log(`Fake Epic Games API running on port ${port}`);
});

import express, { Request, Response } from "express"
import fs from "fs"
import path from "path"
import net from "net"
import { fileURLToPath } from "url"
import { dirname } from "path"
import database from "@/database.json" assert { type: "json" }
import cors from "cors"

// Get __dirname equivalent in ES modules
const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const app = express()
const port = 3000 // Express server port
const tcpPort = 3001 // TCP server port for DLL
const host = "0.0.0.0" // Listen on all interfaces

// Middleware
app.use(express.json())

// CORS middleware
app.use(cors())

interface AuthBypassResponse {
  buildVersion?: string
  catalogItemId?: string
  expires?: string
  items?: Record<string, any>
  labelName?: string
  namespace?: string
  status?: string
  type?: string
  version?: string
  distributions?: any[]
  serviceInstanceId?: string
  message?: string | null
  maintenanceUri?: string | null
  allowedActions?: string[]
  banned?: boolean
  token?: string
  session_id?: string
  token_type?: string
  client_id?: string
  internal_client?: boolean
  client_service?: string
  account_id?: string
  expires_in?: number
  expires_at?: string
  auth_method?: string
  display_name?: string
  app?: string
  in_app_id?: string
}

// Authentication bypass responses
const authBypassResponses: Record<string, AuthBypassResponse | AuthBypassResponse[]> = {
  "launcher/api/public/assets/v2/platform/Windows/catalogItem/4fe75bbc5a674f4f9b356b5c90567da5/app/Fortnite/label/Live": {
    "buildVersion": "++Fortnite+Release-2.4.2-CL-3870737",
    "catalogItemId": "4fe75bbc5a674f4f9b356b5c90567da5", 
    "expires": "9999-12-31T23:59:59.999Z",
    "items": {},
    "labelName": "Live",
    "namespace": "fn",
    "status": "UP"
  },
  "fortnite/api/game/v2/enabled_features": [],
  "fortnite/api/version": {
    "type": "NO_UPDATE",
    "version": "++Fortnite+Release-2.4.2-CL-3870737"
  },
  "launcher/api/public/distributionpoints/": {
    "distributions": []
  },
  "lightswitch/api/service/bulk/status": [{
    "serviceInstanceId": "fortnite",
    "status": "UP",
    "message": "Fortnite is online",
    "maintenanceUri": null,
    "allowedActions": ["PLAY", "DOWNLOAD"],
    "banned": false
  }],
  "account/api/oauth/verify": {
    "token": "valid",
    "session_id": "valid",
    "token_type": "bearer",
    "client_id": "ec684b8c687f479fadea3cb2ad83f5c6",
    "internal_client": true,
    "client_service": "fortnite",
    "account_id": "ninja",
    "expires_in": 28800,
    "expires_at": "9999-12-31T23:59:59.999Z",
    "auth_method": "exchange_code",
    "display_name": "ZeroFN_C1S2",
    "app": "fortnite",
    "in_app_id": "ninja"
  }
}

app.get("/", (_: Request, res: Response) => {
    res.send("Welcome to ZeroFN!")
})

app.get("/fortnite/api/version", (_: Request, res: Response) => {
  res.json({ 
    type: "NO_UPDATE",
    version: "++Fortnite+Release-2.4.2-CL-3870737"
  });
});

app.get("/lightswitch/api/service/bulk/status", (_: Request, res: Response) => {
  res.json([{ serviceInstanceId: "fortnite", status: "UP" }]);
});

app.get("/account/api/oauth/verify", (_: Request, res: Response) => {
  res.json({ token: "valid", session_id: "valid" });
});

app.get("/fortnite/api/cloudstorage/user/:accountId", (_: Request, res: Response) => {
  res.json([]);
});

app.get("/fortnite/api/cloudstorage/user/:accountId/:uniqueFilename", (_: Request, res: Response) => {
  res.send("");
});

app.get("/fortnite/api/game/v2/matchmaking/account/:accountId/session/:sessionId", (req: Request, res: Response) => {
  res.json({ accountId: req.params.accountId, sessionId: req.params.sessionId });
});

interface Cosmetic {
  id: string
  template_id: string
}

app.post("/fortnite/api/game/v2/profile/:accountId/client/QueryProfile", (req: Request, res: Response) => {
  const { accountId } = req.params;
  const profileId = (req.query.profileId as string) || "athena";

  console.log(`Client ${accountId} requesting QueryProfile`);

  const items: Record<string, any> = {};

  (database.cosmetics as Cosmetic[]).forEach((cosmetic) => {
    items[cosmetic.id] = {
      templateId: cosmetic.template_id,
      attributes: {
        favorite: false,
        item_seen: true,
        level: 1,
        variants: [],
        xp: 0,
      },
      quantity: 1,
    }
  });

  res.json({
    profileRevision: 1,
    profileId: profileId,
    profileChangesBaseRevision: 1,
    profileChanges: [{
      changeType: "fullProfileUpdate",
      profile: {
        _id: accountId,
        accountId: accountId,
        profileId: profileId,
        version: "Chapter1_Season2",
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
            season_num: 2,
            book_xp: 0,
            permissions: [],
            season: {
              numWins: 0,
              numHighBracket: 0,
              numLowBracket: 0,
            },
          },
        },
      },
    }],
    serverTime: new Date().toISOString(),
    responseVersion: 1
  });
});

app.post("/fortnite/api/game/v2/profile/:accountId/client/ClientQuestLogin", (req: Request, res: Response) => {
  const { accountId } = req.params;
  const profileId = (req.query.profileId as string) || "athena";

  console.log(`Client ${accountId} requesting ClientQuestLogin`);

  res.json({
    profileRevision: 1,
    profileId: profileId,
    profileChangesBaseRevision: 1,
    profileChanges: [{
      changeType: "fullProfileUpdate", 
      profile: {
        _id: accountId,
        accountId: accountId,
        profileId: profileId,
        version: "Chapter1_Season2",
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
            season_num: 2,
            book_xp: 0,
            permissions: [],
            season: {
              numWins: 0,
              numHighBracket: 0,
              numLowBracket: 0,
            },
          },
        },
      },
    }],
    serverTime: new Date().toISOString(),
    responseVersion: 1
  });
});

app.post("/fortnite/api/game/v2/profile/:accountId/client/:command", (_: Request, res: Response) => {
  res.json({ profileRevision: 1, profileId: "athena", profileChanges: [] });
});

console.log("Initializing ZeroFN Backend...")

// Save database helper
const saveDatabase = () => {
    try {
        console.log("Saving user data to database...")
        fs.writeFileSync(path.join(__dirname, "userdata.json"), JSON.stringify(database, null, 2))
        console.log("Database saved successfully")
    } catch (err) {
        console.error("Error saving database:", err)
    }
}

// TCP server for DLL connection verification with heartbeat
const tcpServer = net.createServer((socket) => {
    console.log("ZeroFN DLL connected to backend")

    // Set keep-alive to true
    socket.setKeepAlive(true, 1000)

    // Store last heartbeat time
    let lastHeartbeat = Date.now()

    // Send initial ping and auth bypass data
    socket.write("ping")
    socket.write(JSON.stringify({
      type: "auth_bypass",
      data: authBypassResponses
    }))

    // Heartbeat interval
    const heartbeatInterval = setInterval(() => {
        // Check if too much time passed since last heartbeat
        if (Date.now() - lastHeartbeat > 10000) {
            console.log("DLL connection timed out - no heartbeat received")
            socket.end()
            clearInterval(heartbeatInterval)
            return
        }

        // Send ping and auth bypass data periodically
        socket.write("ping")
        socket.write(JSON.stringify({
          type: "auth_bypass",
          data: authBypassResponses
        }))
    }, 5000)

    socket.on("error", (err) => {
        console.error("Socket error:", err)
        clearInterval(heartbeatInterval)
    })

    socket.on("data", (data) => {
        const message = data.toString().trim()

        if (message === "pong") {
            // Update last heartbeat time
            lastHeartbeat = Date.now()
        } else {
            console.log("Received data from DLL:", message)
        }
    })

    socket.on("close", () => {
        console.log("ZeroFN DLL connection closed")
        clearInterval(heartbeatInterval)
    })
})

// Start TCP server with error handling
tcpServer.listen(tcpPort, host, () => {
    console.log(`TCP server listening for DLL connections on port ${tcpPort}`)
})

tcpServer.on("error", (err) => {
    console.error("TCP server error:", err)
})

// Random string generator
const randomString = (length: number): string => {
    const chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
    let result = ""
    for (let i = 0; i < length; i++) {
        result += chars[Math.floor(Math.random() * chars.length)]
    }
    return result
}

// Authentication endpoints
app.get("/account/api/oauth/verify", (req: Request, res: Response) => {
    // Extract bearer token from authorization header
    const authHeader = req.headers.authorization
    if (!authHeader || !authHeader.startsWith('Bearer ')) {
        console.log("Missing or invalid authorization header")
        return res.status(401).json({ error: "Invalid authorization" })
    }

    const token = authHeader.split(' ')[1]
    console.log(`Verifying auth token: ${token}`)

    // Verify token matches expected format from DLL
    if (!token.startsWith('eg1~')) {
        console.log("Invalid token format")
        return res.status(401).json({ error: "Invalid token format" })
    }

    console.log("Client connected! Verifying authentication...")
    res.json({
        access_token: token,
        expires_in: 28800,
        token_type: "bearer", 
        refresh_token: token,
        refresh_expires: 115200,
        account_id: "ninja",
        client_id: "ec684b8c687f479fadea3cb2ad83f5c6",
        internal_client: true,
        client_service: "fortnite",
        displayName: "ZeroFN_C1S2",
        app: "fortnite",
        in_app_id: "ninja",
        device_id: "164fb25bb44e42c5a027977d0d5da800"
    })
    console.log("Client authentication verified successfully")
})

app.post("/account/api/oauth/token", (_: Request, res: Response) => {
    console.log("Client requesting auth token...")
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
        displayName: "ZeroFN_C1S2",
        app: "fortnite",
        in_app_id: randomString(32),
    })
    console.log("Auth token generated and sent to client")
})

app.get("/account/api/public/account/:accountId", (req: Request, res: Response) => {
    console.log(`Client requesting account info for ID: ${req.params.accountId}`)
    res.json({
        id: "ninja",
        displayName: "ZeroFN_C1S2",
        name: "ZeroFN_C1S2",
        email: "zerofn@zerofn.com",
        failedLoginAttempts: 0,
        lastLogin: new Date().toISOString(),
        numberOfDisplayNameChanges: 0,
        ageGroup: "UNKNOWN",
        headless: false,
        country: "US",
        lastName: "ZeroFN_C1S2",
        preferredLanguage: "en",
        canUpdateDisplayName: false,
        tfaEnabled: false,
        emailVerified: true,
        minorVerified: false,
        minorExpected: false,
        minorStatus: "UNKNOWN",
        cabinedMode: false,
        hasHashedEmail: false,
        externalAuths: {},
    })
    console.log("Account info sent to client")
})

// Version check endpoints
app.get("/fortnite/api/version", (_: Request, res: Response) => {
    console.log("Client checking game version...")
    res.json({
        type: "NO_UPDATE",
        version: "++Fortnite+Release-2.4.2-CL-3870737",
        buildDate: "2023-01-01",
    })
    console.log("Version check completed")
})

// Lightswitch endpoint for service status
app.get("/lightswitch/api/service/bulk/status", (_: Request, res: Response) => {
    console.log("Client checking service status...")
    res.json([{
        serviceInstanceId: "fortnite",
        status: "UP",
        message: "Fortnite is online",
        maintenanceUri: null,
        overrideCatalogIds: ["a7f138b2e51945ffbfdacc1af0541053"],
        allowedActions: ["PLAY", "DOWNLOAD"],
        banned: false,
        launcherInfoDTO: {
            appName: "Fortnite",
            catalogItemId: "4fe75bbc5a674f4f9b356b5c90567da5",
            namespace: "fn"
        }
    }])
    console.log("Service status sent to client")
})

app.get("/fortnite/api/versioncheck/:version", (req: Request, res: Response) => {
    console.log(`Client version check for: ${req.params.version}`)
    res.json({
        type: "NO_UPDATE",
        acceptedVersion: "++Fortnite+Release-2.4.2-CL-3870737",
        updateUrl: null,
        requiredVersion: "NONE",
        updatePriority: 0,
        message: "No update required.",
    })
    console.log("Version compatibility confirmed")
})

// Cloud storage endpoints
app.get("/fortnite/api/cloudstorage/user/:accountId", (req: Request, res: Response) => {
    console.log(`Client requesting cloud storage for account: ${req.params.accountId}`)
    res.json([
        {
            uniqueFilename: "ClientSettings.Sav",
            filename: "ClientSettings.Sav",
            hash: randomString(32),
            hash256: randomString(64),
            length: 1234,
            contentType: "application/octet-stream",
            uploaded: new Date().toISOString(),
            storageType: "S3",
            doNotCache: false
        },
        {
            uniqueFilename: "ClientQualitySettings.Sav", 
            filename: "ClientQualitySettings.Sav",
            hash: randomString(32),
            hash256: randomString(64),
            length: 1234,
            contentType: "application/octet-stream",
            uploaded: new Date().toISOString(),
            storageType: "S3",
            doNotCache: false
        }
    ])
    console.log("Cloud storage data sent to client")
})

app.get("/fortnite/api/cloudstorage/user/:accountId/:uniqueFilename", (req: Request, res: Response) => {
    console.log(`Client requesting cloud storage file: ${req.params.uniqueFilename}`)
    // Send empty file content
    res.send(Buffer.from([]))
    console.log("Empty cloud storage file sent")
})

app.put("/fortnite/api/cloudstorage/user/:accountId/:uniqueFilename", (req: Request, res: Response) => {
    console.log(`Client uploading cloud storage file: ${req.params.uniqueFilename}`)
    res.status(204).send()
    console.log("Cloud storage file upload acknowledged") 
})

// Catalog endpoint
app.get("/fortnite/api/storefront/v2/catalog", (_: Request, res: Response) => {
    console.log("Client requesting store catalog...")
    res.json({
        catalog: [],
    })
    console.log("Empty catalog sent to client")
})

// Matchmaking session endpoint
app.get("/fortnite/api/game/v2/matchmaking/account/:accountId/session/:sessionId", (req: Request, res: Response) => {
    console.log(`Client requesting matchmaking session for account ${req.params.accountId}`)
    res.json({
        accountId: req.params.accountId,
        sessionId: req.params.sessionId,
        key: "none",
    })
    console.log("Matchmaking session response sent")
})

interface ProfileCommand {
    accountId: string
    command: string
}

// Profile endpoints
app.post("/fortnite/api/game/v2/profile/:accountId/client/:command", (req: Request<ProfileCommand>, res: Response) => {
    const { accountId, command } = req.params
    const profileId = (req.query.profileId as string) || "athena"

    console.log(`Client ${accountId} requesting profile command: ${command}`)

    const baseResponse = {
        profileRevision: 1,
        profileId: profileId,
        profileChangesBaseRevision: 1,
        profileChanges: [] as any[],
        serverTime: new Date().toISOString(),
        responseVersion: 1,
    }

    switch (command) {
        case "QueryProfile":
            console.log("Processing QueryProfile request...")
            const items: Record<string, any> = {}

            console.log("Adding custom cosmetics from database...")
            ;(database.cosmetics as Cosmetic[]).forEach((cosmetic) => {
                items[cosmetic.id] = {
                    templateId: cosmetic.template_id,
                    attributes: {
                        favorite: false,
                        item_seen: true,
                        level: 1,
                        variants: [],
                        xp: 0,
                    },
                    quantity: 1,
                }
            })

            baseResponse.profileChanges.push({
                changeType: "fullProfileUpdate",
                profile: {
                    _id: accountId,
                    accountId: accountId,
                    profileId: profileId,
                    version: "Chapter1_Season2",
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
                            season_num: 2,
                            book_xp: 0,
                            permissions: [],
                            season: {
                                numWins: 0,
                                numHighBracket: 0,
                                numLowBracket: 0,
                            },
                        },
                    },
                },
            })
            console.log("QueryProfile response prepared")
            break

        case "ClientQuestLogin":
        case "RefreshExpeditions":
        case "SetMtxPlatform":
        case "SetItemFavoriteStatusBatch":
        case "EquipBattleRoyaleCustomization":
            console.log(`Processing ${command} request...`)
            baseResponse.profileChanges.push({
                changeType: "fullProfileUpdate",
                profile: {
                    _id: accountId,
                    accountId: accountId,
                    profileId: profileId,
                    version: "Chapter1_Season2",
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
                            season_num: 2,
                            book_xp: 0,
                            permissions: [],
                            season: {
                                numWins: 0,
                                numHighBracket: 0,
                                numLowBracket: 0,
                            },
                        },
                    },
                },
            })
            console.log(`${command} response prepared`)
            break

        default:
            console.log(`Warning: Unknown command received: ${command}`)
    }

    res.json(baseResponse)
    console.log(`Response sent for command: ${command}`)
})

// Start express server with error handling
app.listen(port, host, () => {
    console.log(`ZeroFN Backend running on ${host}:${port}`)
    console.log("Server is ready to accept connections from ZeroFN DLL!")
}).on("error", (err) => {
    console.error("Express server error:", err)
})

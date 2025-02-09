import express from "express"
import fs from "fs"
import path from "path"
import net from "net"
import { fileURLToPath } from "url"
import { dirname } from "path"
import database from "@/database.json" with { type: "json" }
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

app.get("/", (_, res) => {
    res.send("Welcome to ZeroFN!")
})

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

    // Send initial ping
    socket.write("ping")

    // Heartbeat interval
    const heartbeatInterval = setInterval(() => {
        // Check if too much time passed since last heartbeat
        if (Date.now() - lastHeartbeat > 10000) {
            console.log("DLL connection timed out - no heartbeat received")
            socket.end()
            clearInterval(heartbeatInterval)
            return
        }

        // Send ping
        socket.write("ping")
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
app.get("/account/api/oauth/verify", (req, res) => {
    console.log("Client connected! Verifying authentication...")
    res.json({
        access_token: "eg1~-*",
        expires_in: 28800,
        token_type: "bearer",
        refresh_token: "eg1~-*",
        refresh_expires: 115200,
        account_id: "ninja",
        client_id: "ec684b8c687f479fadea3cb2ad83f5c6",
        internal_client: true,
        client_service: "fortnite",
        displayName: "ZeroFN",
        app: "fortnite",
        in_app_id: "ninja",
        device_id: "164fb25bb44e42c5a027977d0d5da800",
    })
    console.log("Client authentication verified successfully")
})

app.post("/account/api/oauth/token", (req, res) => {
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
        displayName: "ZeroFN",
        app: "fortnite",
        in_app_id: randomString(32),
    })
    console.log("Auth token generated and sent to client")
})

app.get("/account/api/public/account/:accountId", (req, res) => {
    console.log(`Client requesting account info for ID: ${req.params.accountId}`)
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
        externalAuths: {},
    })
    console.log("Account info sent to client")
})

// Version check endpoints
app.get("/fortnite/api/version", (req, res) => {
    console.log("Client checking game version...")
    res.json({
        type: "NO_UPDATE",
        version: "++Fortnite+Release-Cert-CL-3807424",
        buildDate: "2023-01-01",
    })
    console.log("Version check completed")
})

app.get("/fortnite/api/versioncheck/:version", (req, res) => {
    console.log(`Client version check for: ${req.params.version}`)
    res.json({
        type: "NO_UPDATE",
        acceptedVersion: "++Fortnite+Release-Cert-CL-3807424",
        updateUrl: null,
        requiredVersion: "NONE",
        updatePriority: 0,
        message: "No update required.",
    })
    console.log("Version compatibility confirmed")
})

// Cloud storage endpoints
app.get("/fortnite/api/cloudstorage/user/:accountId", (req, res) => {
    console.log(`Client requesting cloud storage for account: ${req.params.accountId}`)
    res.json([
        {
            uniqueFilename: "DefaultGame.ini",
            filename: "DefaultGame.ini",
            hash: randomString(32),
            hash256: randomString(64),
            length: 1234,
            contentType: "application/octet-stream",
            uploaded: "2025-01-31T20:57:02.000Z",
            storageType: "S3",
            doNotCache: false,
        },
    ])
    console.log("Cloud storage data sent to client")
})

app.post("/fortnite/api/cloudstorage/user/:accountId/:uniqueFilename", (req, res) => {
    console.log(`Client updating cloud storage: ${req.params.uniqueFilename}`)
    res.status(204).send()
    console.log("Cloud storage update acknowledged")
})

// Catalog endpoint
app.get("/fortnite/api/storefront/v2/catalog", (req, res) => {
    console.log("Client requesting store catalog...")
    res.json({
        catalog: [],
    })
    console.log("Empty catalog sent to client")
})

// Matchmaking session endpoint
app.get("/fortnite/api/game/v2/matchmaking/account/:accountId/session/:sessionId", (req, res) => {
    console.log(`Client requesting matchmaking session for account ${req.params.accountId}`)
    res.json({
        accountId: req.params.accountId,
        sessionId: req.params.sessionId,
        key: "none",
    })
    console.log("Matchmaking session response sent")
})

// Profile endpoints
app.post("/fortnite/api/game/v2/profile/:accountId/client/:command", (req, res) => {
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
            database.cosmetics.forEach((cosmetic) => {
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
                    version: "no_version",
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
                    version: "no_version",
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

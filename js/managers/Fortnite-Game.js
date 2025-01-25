module.exports = (app, port) => {
    var oauth = require("./oauth")
    const NeoLog = require('../structs/NeoLog')
    //${AccountIDP.accountIdP}
    app.get("/content/api/pages/fortnite-game", (req, res) => {
        NeoLog.Log(`Loaded config file for ${oauth.accountIDT}`)
        module.exports.authID = oauth.accountIDT;
        var Userconfig = require(`../config/${oauth.accountIDT}/profiles/profile_uConfig.json`)
        res.json({
            "jcr:isCheckedOut": true,
            "_title": "Fortnite Game",
            "jcr:baseVersion": "a7ca237317f1e7b00bc82e-d9a2-4f0d-b951-704d295cd1aa",
            "_activeDate": "2017-07-24T22:24:02.300Z",
            "lastModified": "2020-11-01T17:36:19.024Z",
            "_locale": "en-US",
            "emergencynotice": {
                "news": {
                    "platform_messages": [],
                    "_type": "Battle Royale News",
                    "messages": [{
                        "hidden": false,
                        "_type": "CommonUI Simple Message Base",
                        "subgame": "br",
                        "body": "Made by TFN and Pokelevel\r\n \r\nDue to an issue we disabled the Collision event (v20.40) we'll provide an update when this issue is resolved!",
                        "title": "ZeroFN",
                        "spotlight": false
                    }, 
                    ],
                },
                "jcr:isCheckedOut": true,
                "_title": "emergencynotice",
                "_noIndex": false,
                "alwaysShow": true,
                "jcr:baseVersion": "a7ca237317f1e761d4ee60-7c40-45a8-aa3e-bb0a2ffa9bb5",
                "_activeDate": "2018-08-06T19:00:26.217Z",
                "lastModified": "2020-10-30T04:50:59.198Z",
                "_locale": "en-US"
            },
            playlistinformation: {
                is_tile_hidden: true,
                frontend_matchmaking_header_style: "Basic",
                conversion_config: {
                    containerName: "playlist_info",
                    _type: "Conversion Config",
                    enableReferences: true,
                    contentName: "playlists"
                },
                show_ad_violator: true,
                _title: "playlistinformation",
                frontend_matchmaking_header_text_description: "LIVE EVENT",
                frontend_matchmaking_header_text: "PLAY LIVE EVENTS NOW!",
                playlist_info: {
                    _type: "Playlist Information",
                    playlists: [
                        {
                            image: "https://cdn2.unrealengine.com/20br-armadillo-creativetile-tile-1920x1080-1920x1080-f40652491ad0.jpg",
                            playlist_name: "Playlist_Armadillo",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Weapons Team to battle stations. Today, we’re gonna put an end to the Imagined Order. For Good.",
                            display_name: "Collision"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/20br-nopermit-tiles-4-1920x1080-1920x1080-7566d41dbe4e.jpg",
                            playlist_name: "Playlist_NoBuildBR_Squad",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "All battle, no building!\n\nPlayers now have a recharging Overshield for added defense as they fight to be the last one standing.",
                            display_name: "Zero Build - Squads"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/20br-nopermit-tiles-3-1920x1080-1920x1080-23d0d21bae4b.jpg",
                            playlist_name: "Playlist_NoBuildBR_Trio",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "All battle, no building!\n\nPlayers now have a recharging Overshield for added defense as they fight to be the last one standing.",
                            display_name: "Zero Build - Trios"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/20br-nopermit-tiles-2-1920x1080-1920x1080-12f463833ab2.jpg",
                            playlist_name: "Playlist_NoBuildBR_Duo",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "All battle, no building!\n\nPlayers now have a recharging Overshield for added defense as they fight to be the last one standing.",
                            display_name: "Zero Build - Duos"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/20br-nopermit-tiles-1-1920x1080-1920x1080-ca7d3703484c.jpg",
                            playlist_name: "Playlist_NoBuildBR_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "All battle, no building!\n\nPlayers now have a recharging Overshield for added defense as they fight to be the last one standing.\n",
                            display_name: "Zero Build - Solo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/18br-guava-playlisttile-1920x1080-1920x1080-e66a6a0a08cf.jpg",
                            playlist_name: "Playlist_Guava",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/18pr-in-game-discovery-defaulttile-1920x1080-3ab459cdf025.jpg",
                            playlist_name: "Playlist_Papaya",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Welcome to Party Royale, an experimental and evolving space. Leave your weapons and mats behind and hang out with friends, play games and enjoy live entertainment. Catch live shows. Race around obstacle courses by land or sea. Go fishing with friends, perfect your skydive, and get up close and connect with some of your favorite artists. Hop on the party bus and enjoy the ride."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17br-rifttour-ltm-1024x512-796d91a07a1b.jpg",
                            playlist_name: "Playlist_Buffet",
                            violator: "EVENT!",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Dive into the Rift Tour featuring Ariana Grande, a musical experience unlike any other. \n\n(Photosensitivity Warning)",
                            display_subname: "FT. ARIANA GRANDE",
                            display_name: "RIFT TOUR"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17cm-the02-primary-playlisttile-1024x512-70cb9a84329e.jpg",
                            playlist_name: "Playlist_Creative_LTM_O2Exp",
                            violator: "CREATOR MADE!",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "A musical adventure awaits! Jump into this Creative Island for an interactive music experience created by O2. Enjoy easy life’s music as you go on a journey in tune to the gig.",
                            display_subname: "EXPLORE THE SHOW",
                            display_name: "EASY LIFE AT THE O2"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17br-teambrawlltm-1024x512-1024x512-88330ce96a1f.png",
                            playlist_name: "Playlist_Madman_Squads",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17br-mole-motd-1920x1080-1920x1080-0fa22131cdbc.jpg",
                            playlist_name: "Playlist_MoleGame",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17br-kiwi-keyart-motd-1920x1080-1920x1080-50ccef17c86f.jpg",
                            playlist_name: "Playlist_Kiwi",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: "OPERATION: SKY FIRE"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17br-mole-motd-1920x1080-1920x1080-0fa22131cdbc.jpg",
                            playlist_name: "Playlist_MoleGameVoice",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "The Bridge is under attack! Keep the Bridge safe as an Agent or eliminate the IO as an Impostor.\n\nNow with Role Bias! Select if you prefer to be an Impostor or Agent.",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17br-teambrawlltm-1024x512-1024x512-88330ce96a1f.png",
                            playlist_name: "Playlist_Madman_Trios",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/horderush-tile-800x450-800x450-3868af8c93e8.jpg",
                            playlist_name: "playlist_mash_squads_legacy",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            playlist_name: "Playlist_ShowdownAlt_BlueFlatline16_1v1",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Prove your skill in this competitive mode! During each match, you'll earn Hype by either achieving high placements or eliminating opponents. As you gain Hype, you'll compete against tougher opponents with similar skill level."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-aloylara-modetile-1024x512-ad4c5c3e7d15.jpg",
                            playlist_name: "Playlist_LeadAlloy_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/floorrestores-1024x512-9c6fcb596b31.jpg",
                            playlist_name: "Playlist_Fluoride_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-impossibleescape-motd-1024x512-1024x512-b55f2be68f49.jpg",
                            playlist_name: "Playlist_Daybreak_PE_Squads",
                            hidden: false,
                            violator: "WIN A LOADING SCREEN!",
                            _type: "FortPlaylistInfo",
                            description: "You wake up alone and unarmed. Quickly search for weapons and ammunition before night falls and creatures begin hunting for prey. Keep an eye out for other survivors — they may prove to be invaluable allies. Search for parts that can be used to repair one of the downed helicopters. With a repaired and fueled Choppa, you may be able to complete the Impossible Escape! NOTE: Players loading in with a squad / party will be separated. Search for your friends or other players on the Island and form a new squad!",
                            display_name: "Impossible Escape! PvE"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-impossibleescape-motd-1024x512-1024x512-b55f2be68f49.jpg",
                            playlist_name: "Playlist_Daybreak_PP_Squads",
                            hidden: false,
                            violator: "WIN AN UMBRELLA!",
                            _type: "FortPlaylistInfo",
                            description: "You wake up alone and unarmed. Quickly search for weapons and ammunition before night falls and creatures begin hunting for prey. Keep an eye out for other survivors — they may prove to be invaluable allies. Search for parts that can be used to repair one of the downed helicopters. With a repaired and fueled Choppa, you may be able to complete the Impossible Escape! NOTE: Players loading in with a squad / party will be separated. Search for your friends or other players on the Island and form a new squad!",
                            display_name: "Impossible Escape! PvP"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/floorrestores-1024x512-9c6fcb596b31.jpg",
                            playlist_name: "Playlist_Fluoride_Duos",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/takeover-2670x1502-f4f7ec6acc48.png",
                            playlist_name: "Playlist_Hydrogen_24",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-yogurtevent-playlisttile-1024x512-1024x512-9bd732a3d474.png",
                            playlist_name: "Playlist_Yogurt",
                            hidden: false,
                            violator: "LEAVING SOON",
                            _type: "FortPlaylistInfo",
                            description: "ZERO CRISIS FINALE",
                            display_subname: "",
                            display_name: "ZERO CRISIS FINALE"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-mandoltm-social-playlisttile-1024x512-1024x512-7a88bcc1902f.jpg",
                            playlist_name: "Playlist_Vendetta_Trios",
                            hidden: false,
                            violator: "NEW",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-mandoltm-social-playlisttile-1024x512-1024x512-4fe6dd9896ce.jpg",
                            playlist_name: "Playlist_Vendetta_Squads",
                            hidden: false,
                            violator: "NEW",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-mandoltm-social-playlisttile-1024x512-1024x512-1fbe6e699e16.jpg",
                            playlist_name: "Playlist_Vendetta_Solo",
                            hidden: false,
                            violator: "NEW",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-mandoltm-social-playlisttile-1024x512-1024x512-91e6971d8a5a.jpg",
                            playlist_name: "Playlist_Vendetta_Duos",
                            hidden: false,
                            violator: "NEW",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15cm-spywithin-goldmansion-modetile-wchars-1024x512-07086a098dd0.jpg",
                            playlist_name: "Playlist_Creative_LTM_Spy_Gold",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created by: DolphinDom, Bunni_, and KKSlider\n\nSeveral key islands have been infiltrated by Spies. It’s your job to track them down and flush them out. Watch your back, you’ll be arriving at each location with limited information. You don’t know who the Spies are, so don’t trust anyone... We can trust you, right?\n\nThe team of Agents must discover the identity of the Spies and vote them off to win. Alternatively, they can complete the objectives on the map before the Spies eliminate them. The team of Spies must stop the Agents from completing objectives and eliminate them while keeping their identity a secret."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15cm-spywithin-shoppersdelight-modetile-wchar-1024x512-85bf539711a3.jpg",
                            playlist_name: "Playlist_Creative_LTM_Spy_Shopper",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created by: Ritual, MackJack, and Snownymous\n\nSeveral key islands have been infiltrated by Spies. It’s your job to track them down and flush them out. Watch your back, you’ll be arriving at each location with limited information. You don’t know who the Spies are, so don’t trust anyone... We can trust you, right?\n\nThe team of Agents must discover the identity of the Spies and vote them off to win. Alternatively, they can complete the objectives on the map before the Spies eliminate them. The team of Spies must stop the Agents from completing objectives and eliminate them while keeping their identity a secret."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15cm-spywithin-gildedgalaxy-modetile-wchar-1024x512-c2249eacbd95.jpg",
                            playlist_name: "Playlist_Creative_LTM_Spy_Gilded",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created by: DolphinDom, Bunni_, KKSlider, Wert, Ritual, MackJack, Snownymous, jstKamui, and Blanky\n\nSeveral key islands have been infiltrated by Spies. It’s your job to track them down and flush them out. Watch your back, you’ll be arriving at each location with limited information. You don’t know who the Spies are, so don’t trust anyone... We can trust you, right?\n\nThe team of Agents must discover the identity of the Spies and vote them off to win. Alternatively, they can complete the objectives on the map before the Spies eliminate them. The team of Spies must stop the Agents from completing objectives and eliminate them while keeping their identity a secret."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-rallyroyaleltm-playlisttile-1024x512-3d0ebcc13a4c.jpg",
                            playlist_name: "Playlist_Argon_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-rallyroyaleltm-playlisttile-1024x512-23ab03be2972.jpg",
                            playlist_name: "Playlist_Argon_Duos_SP",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-airroyale-ltm-playlisttile-1024x512-1024x512-7b5ff8fdd58b.jpg",
                            playlist_name: "Playlist_Goose_Duos_16",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-airroyale-ltm-playlisttile-1024x512-1024x512-de9c82c31463.jpg",
                            playlist_name: "Playlist_Goose_Duos_16_SP",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-pickaxefrenzy-playlisttile-1024x512-1024x512-391638031de7.jpg",
                            playlist_name: "Playlist_Nickel_Evergreen_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-pickaxefrenzy-playlisttile-1024x512-1024x512-53682cf5e48f.jpg",
                            playlist_name: "Playlist_Nickel_Evergreen_Duos_SP",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-tiltedtaxi-motd-1024x512-1024x512-890727217.jpg",
                            playlist_name: "Playlist_Nitrogen12_Duos_SP",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-shockwave-ltm-playlisttile-1024x512-1024x512-b2ab79381dee.jpg",
                            playlist_name: "Playlist_PhosphorusWipeout_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-shockwave-ltm-playlisttile-1024x512-1024x512-9b48e174921e.jpg",
                            playlist_name: "Playlist_PhosphorusWipeout_Duos_SP",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/br05-lobbytile-ltm-scoreroyale-1024x512-b13e6d98ff70.jpg",
                            playlist_name: "Playlist_Score_Duos_SP",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11br-catch-ltm-modetile-1024x512-1024x512-cc821b528106.jpg",
                            playlist_name: "Playlist_Toss_Duos_SP",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-rotating-modetile-1024x512-97cd69813471.jpg",
                            playlist_name: "Playlist_WinterArena_Duos",
                            hidden: false,
                            violator: "DUOS",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15cm-spywithin-holidayworkshop-modetile-wchar-1024x512-753e1d69c939.jpg",
                            playlist_name: "Playlist_Creative_LTM_Spy_Santa",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created by: DolphinDom, Bunni_, and KKSlider\n\nSeveral key islands have been infiltrated by Spies. It’s your job to track them down and flush them out. Watch your back, you’ll be arriving at each location with limited information. You don’t know who the Spies are, so don’t trust anyone... We can trust you, right?\n\nThe team of Agents must discover the identity of the Spies and vote them off to win. Alternatively, they can complete the objectives on the map before the Spies eliminate them. The team of Spies must stop the Agents from completing objectives and eliminate them while keeping their identity a secret."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15cm-spywithin-steamboat-modetile-wchar-1024x512-a26ab2e7546b.jpg",
                            playlist_name: "Playlist_Creative_LTM_Spy_Steam",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created by: Wert\n\nSeveral key islands have been infiltrated by Spies. It’s your job to track them down and flush them out. Watch your back, you’ll be arriving at each location with limited information. You don’t know who the Spies are, so don’t trust anyone... We can trust you, right?\n\nThe team of Agents must discover the identity of the Spies and vote them off to win. Alternatively, they can complete the objectives on the map before the Spies eliminate them. The team of Spies must stop the Agents from completing objectives and eliminate them while keeping their identity a secret."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15cm-spywithin-midnightcity-modetile-wchar-1024x512-f16ba473478c.jpg",
                            playlist_name: "Playlist_Creative_LTM_Spy_Crystal",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created by: DolphinDom, Bunni_, KKSlider, Wert, Ritual, MackJack, Snownymous, jstKamui, and Blanky\n\nSeveral key islands have been infiltrated by Spies. It’s your job to track them down and flush them out. Watch your back, you’ll be arriving at each location with limited information. You don’t know who the Spies are, so don’t trust anyone... We can trust you, right?\n\nThe team of Agents must discover the identity of the Spies and vote them off to win. Alternatively, they can complete the objectives on the map before the Spies eliminate them. The team of Spies must stop the Agents from completing objectives and eliminate them while keeping their identity a secret."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15cm-spywithin-midasmuseum-modetile-wchar-1024x512-eae1af8d714e.jpg",
                            playlist_name: "Playlist_Creative_LTM_Spy_Midas",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created by: Blanky, jstKamui\n\nSeveral key islands have been infiltrated by Spies. It’s your job to track them down and flush them out. Watch your back, you’ll be arriving at each location with limited information. You don’t know who the Spies are, so don’t trust anyone... We can trust you, right?\n\nThe team of Agents must discover the identity of the Spies and vote them off to win. Alternatively, they can complete the objectives on the map before the Spies eliminate them. The team of Spies must stop the Agents from completing objectives and eliminate them while keeping their identity a secret."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/08br-ltm-floorislava-modetile-1024x512-93f004aa2dec.jpg",
                            playlist_name: "Playlist_Melt_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/08br-ltm-floorislava-modetile-1024x512-93f004aa2dec.jpg",
                            playlist_name: "Playlist_Melt_Solo",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-pickaxefrenzy-playlisttile-1024x512-1024x512-049fb2d9cf0c.jpg",
                            playlist_name: "Playlist_Nickel_Evergreen_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/rumble-one-shot-1024x512-64bb9a287c8a.jpg",
                            playlist_name: "Playlist_Respawn_24_Low",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11br-solid-gold-playlist-tile-1025x512-1024x512-c69af2bc91c0.jpg",
                            playlist_name: "Playlist_Respawn_24_SolidGold",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-shockwave-ltm-motd-800x450-800x450-8183c5111fc3.jpg",
                            playlist_name: "Playlist_PhosphorusWipeout_Trios",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-comebackltm-modetile-1024x512-9edb41e51f52.jpg",
                            playlist_name: "Playlist_Rebirth_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-unvaulted-ltm-motd-1024x512-1024x512-1832962690a3.jpg",
                            playlist_name: "Playlist_Respawn_24_Unvaulted",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/08br-ltm-floorislava-modetile-1024x512-6d9014c851d5.jpg",
                            playlist_name: "Playlist_Melt_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/08br-ltm-floorislava-modetile-1024x512-56d3d2dfd36a.jpg",
                            playlist_name: "Playlist_MeltDisarmed_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-rebirthltm-motd-1920x1080-1920x1080-6711d20ac065.jpg",
                            playlist_name: "Playlist_Rebirth_Duos",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/shadowsvsghosts-motd-1024x512-1024x512-a866cfd23de0.jpg",
                            playlist_name: "Playlist_Creative_LTM_ShadowVsGhost",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-shockwave-ltm-playlisttile-1024x512-1024x512-b2ab79381dee.jpg",
                            playlist_name: "Playlist_Phosphorus_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/sunrisecity-tile-1024x512-4cee4b2452ce.jpg",
                            playlist_name: "Playlist_Creative_LTM_SunriseHorde",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17br-rocketsvcars-ltm-motd-1024x512-1024x512-d78b51805d48.jpg",
                            playlist_name: "Playlist_Creative_LTM_RocketsVsCars",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/vehiclehub-tile-1024x512-3040c650f68f.jpg",
                            playlist_name: "Playlist_Creative_LTM_VehicleHub",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-shockwave-ltm-playlisttile-1024x512-1024x512-ef63789d9c6e.jpg",
                            playlist_name: "Playlist_PhosphorusWipeout_Squads",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/08br-ltm-floorislava-modetile-1024x512-56d3d2dfd36a.jpg",
                            playlist_name: "Playlist_MeltDisarmed_Solo",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-shockwave-ltm-playlisttile-1024x512-1024x512-b2ab79381dee.jpg",
                            playlist_name: "Playlist_Phosphorus_Duos",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/08br-ltm-floorislava-modetile-1024x512-56d3d2dfd36a.jpg",
                            playlist_name: "Playlist_MeltDisarmed_Trios",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/18br-headband-creativeleafhub-1920x1080-1920x1080-861f82ff5285.jpg",
                            playlist_name: "Playlist_Creative_LTM_DAD_1.",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/18cm-headbandscrnshot-2-1920x1080-011960708e22.png",
                            playlist_name: "Playlist_Creative_LTM_DAD_2",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/08br-ltm-floorislava-modetile-1024x512-6d9014c851d5.jpg",
                            playlist_name: "Playlist_Melt_Trios",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/galactus-is-here-1024x512-615573933.jpg",
                            playlist_name: "playlist_junior_32",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: "Defend the Island to save all Reality!",
                            display_subname: "",
                            display_name: "THE DEVOURER OF WORLDS"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11br-catch-ltm-modetile-1024x512-1024x512-cc821b528106.jpg",
                            playlist_name: "Playlist_Toss_Trios",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-bodyguard-ltm-playlisttile-1024x512-1024x512-b78c16142f41.jpg",
                            playlist_name: "Playlist_Bodyguard_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-bodyguard-ltm-playlisttile-1024x512-1024x512-b78c16142f41.jpg",
                            playlist_name: "Playlist_Bodyguard_Trios",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-bodyguard-ltm-playlisttile-1024x512-1024x512-b78c16142f41.jpg",
                            playlist_name: "Playlist_Bodyguard_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: "Suit up, it's time to protect the VIP! In Bodyguard, each duo has a VIP. The other player is a bodyguard sworn to defend them. If a duo's VIP is eliminated then both are out.\n\nVIPs are vulnerable. They're marked to all enemies and can't pick up weapons. But, they aren't helpless either. VIPs spawn with a Legendary pistol and a rechargeable Revive Grenade."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/01-18-1v1buildwars-512-1024x512-cd8703650792.jpg",
                            playlist_name: "Playlist_Creative_LTM_1V1BF",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/1-25-darkmines-murdermystery-tile-1024x512-4733e26e2991.png",
                            playlist_name: "Playlist_Creative_LTM_Mystery",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/2xzonewars-1024x512-f91103904103.png",
                            playlist_name: "Playlist_Creative_LTM_2XZoneWars",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/02-01-zonewars-purgatory-ltmtile-2046x1023-0466f60b5208.jpg",
                            playlist_name: "Playlist_Creative_LTM_ZoneWarsP",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Zone Wars Purgatory is the first of its kind. This LTM offers players unending gameplay with activities like Box Fights or Parkour to keep you entertained when eliminated early.\n\nQueue up solo or with a duo partner and battle it out in high action Zone Wars! You will be automatically matched with other players to fill a lobby.",
                            display_name: "Duo Zone Wars: Purgatory"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/0203-zodiaclibrary-1024-1024x512-3a9991b907d8.png",
                            playlist_name: "Playlist_Creative_LTM_ZodiacGG",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/ritualsskydockboxfights-tile-1920x960-8161824b2929.jpg",
                            playlist_name: "Playlist_Creative_LTM_Skydock",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/2xzonewars-1024x512-f91103904103.png",
                            playlist_name: "Playlist_Creative_LTM_ZWBF",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/0208-noscope-1024-1024x512-be96a0c1fa46.png",
                            playlist_name: "Playlist_Creative_LTM_OneShot",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/02-12-boltbash-1024-1024x512-577aa79bb8f5.png",
                            playlist_name: "Playlist_Creative_LTM_BoltBash",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/02-15-skyfall-tile-1024x512-4fa470275765.png",
                            playlist_name: "Playlist_Creative_LTM_ClixBF",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/poseidonv-1024-1024x512-7d3cc3b68273.png",
                            playlist_name: "Playlist_Creative_LTM_LoadoutWarfare",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Attack and defend strategic points that offer a chance for victory! If not, then next Phase you'll get your chance.\n\nChoose between 12 unique Loadouts that have their own unique special abilities and battle it out in a Futuristic Spaceship. "
                        },
                        {
                            image: "https://cdn2.unrealengine.com/waterpark-tile-1024x512-3eccbe62f50e.jpg",
                            playlist_name: "Playlist_Creative_LTM_Waterpark",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/survivalsimulator-1024-1024x512-b19376cf9075.png",
                            playlist_name: "Playlist_Creative_LTM_SurvivalSim",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-finesty2real2v2-tile-1024x512-1057892faa69.jpg",
                            playlist_name: "Playlist_Creative_LTM_FinestYTReal2v2",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: "Finest's Realistic 2v2"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/pararoyale-1024-1024x512-6a1d8744a522.png",
                            playlist_name: "Playlist_Creative_LTM_Pararoyale",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitymade-jduth100leveldefaultdeathrun-ltm-1024x512-cf83ac053042.jpg",
                            playlist_name: "Playlist_Creative_LTM_JDuth100",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Get ready all you Defaults, time to jump into the 100 Level Default Deathrun LTM created by JDuth.\n\nFeaturing 100 Default Deathrun levels, tutorial levels and an EPIC boss fight against the Default King!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/gogoated-1024-1024x512-fc6be49eb32c.png",
                            playlist_name: "Playlist_Creative_LTM_GoGoated3v3",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-prisonbreakout-v2-tile-1024x512-6020d7571174.jpg",
                            playlist_name: "Playlist_Creative_LTM_FortniteJB",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "You've been caught, and must be crafty to escape your confines. As a Prisoner, you must loot the surrounding area to power up and fight guards for your ticket out of this dump.\n\nOr you can play as a Guard, and prevent Prisoners from escaping.\n\nCause mayhem or prevent it in the Prison Breakout LTM by Echo."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/containment-1024-1024x512-cb2f17b4b831.png",
                            playlist_name: "Playlist_Creative_LTM_Containment",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/millionairetycoon-1024-1024x512-4b3e1bf6f25e.png",
                            playlist_name: "Playlist_Creative_LTM_Tycoon",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/bioszonewars-1024-1024x512-cf9d3d3a3403.jpg",
                            playlist_name: "Playlist_Creative_LTM_BIOSZWTrio",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-200-level-default-deathrun-tile-1024x512-d2a5f09064ca.jpg",
                            playlist_name: "Playlist_Creative_LTM_200LevelRun",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Queue up solo or with your friends to complete all levels in the 200 Level Default Deathrun LTM by FxxD1.\n\nFeaturing 200 unique and easy levels to complete and a warm-up area to get ready!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/wallwars-1024-1024x512-6d3c56e9ff9d.png",
                            playlist_name: "Playlist_Creative_LTM_WallWars",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/fireball-1024-1024x512-3137f9341efb.png",
                            playlist_name: "Playlist_Creative_LTM_CageFGG",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-laracroftmanor-modetile-1024x512-c231635a85b5.jpg",
                            playlist_name: "Playlist_Creative_LTM_CM",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-greenvspurplegogoated-ltm-1024x512-93d1a02c5d9f.jpg",
                            playlist_name: "Playlist_Creative_LTM_GVP",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/mig8v8-1024-1024x512-d7ccf9e62796.png",
                            playlist_name: "Playlist_Creative_LTM_MIG8v8",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/bps8-fortilla1024-1024x512-fca04dcb5d77.png",
                            playlist_name: "Playlist_Creative_LTM_PiratesOF",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/redvsblue-1024-1024x512-775768f91a3b.jpg",
                            playlist_name: "Playlist_Creative_LTM_AdvancedRVB",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/community-cltm-colour-switch-3840x2160-07e46d69cc7b.png",
                            playlist_name: "Playlist_Creative_LTM_ColorSwitch",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/nick30eh-1024-1024x512-bc917e50762f.jpg",
                            playlist_name: "Playlist_Creative_LTM_NE30ZW",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/holeffa-1024-1024x512-507aa4e8e513.png",
                            playlist_name: "Playlist_Creative_LTM_TheHoleFFA",
                            hidden: false,
                            violator: "SOCIAL FFA",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/lockdown-hordesurvival-tile-2151x1076-c472d135f8e6.jpg",
                            playlist_name: "Playlist_Creative_LTM_LockdownHorde",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/prophuntinfinite-1024-1024x512-b74b40c4b5cc.jpg",
                            playlist_name: "Playlist_Creative_LTM_PropHuntInfinite",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/fireball-1024-1024x512-03bda46b2f0e.png",
                            playlist_name: "Playlist_Creative_LTM_LavaCTF",
                            hidden: false,
                            violator: "12v12!",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-neymarjrpuma-modetile-1024x512-a9e7e4dba1da.jpg",
                            playlist_name: "Playlist_Creative_LTM_GoCrazyArena",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-ltm-redvsblue-rumble-motd-1024x512-1024x512-3eed6c35e093.jpg",
                            playlist_name: "Playlist_Creative_LTM_RVBRumble",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-ltm-redvsblue-xs-motd-1024x512-1024x512-c72dd8d8b8ea.jpg",
                            playlist_name: "Playlist_Creative_LTM_RVBXS",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-proffaarena-modetile-1024x512-9d1a00c98d06.jpg",
                            playlist_name: "Playlist_Creative_LTM_ProFFAArena",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/freakyflights-tile-2151x1076-b76acb695360.png",
                            playlist_name: "Playlist_Creative_LTM_FreakyFlights",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/spytycoon-tile-1024x512-e2174cb5e72a.jpg",
                            playlist_name: "Playlist_Creative_LTM_SpyTycoon",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/outbreakltm-resize-1024x512-d95c08f4b169.png",
                            playlist_name: "Playlist_Creative_LTM_Outbreak",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/survivethecity-tile-1024x512-6d5631561dda.png",
                            playlist_name: "Playlist_Creative_LTM_SurviveTheCity",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/proredvsblue-rojoazul-modetile-1024x512-d79c75c8f007.jpg",
                            playlist_name: "Playlist_Creative_LTM_ProRVB",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/redvsblue2-0-modetile-1024x512-32a7b1a5b6fc.jpg",
                            playlist_name: "Playlist_Creative_LTM_NewRVB2",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "The classic team vs team deathmatch is back with Red vs Blue 2.0!\n\nChoose your loadout and jump into the battle. Infinite respawns get you right back into the action.\n\nEarn enough eliminations to claim victory!\n\nTags: just for fun, 8v8, team deathmatch\n\nOriginal Map Name: RED VS BLUE 2.0",
                            display_name: "RED VS BLUE 2.0"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/primalrumble-1024-1024x512-30fc3c1780fe.png",
                            playlist_name: "Playlist_Creative_LTM_PrimalRumble",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16br-nba-creativehubstaticpromo-modetile-1024x512-e647e5993dc8.jpg",
                            playlist_name: "Playlist_Creative_LTM_CourtCrashers",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/carroyale-tile-1024x512-358f2a979ee8.png",
                            playlist_name: "Playlist_Creative_LTM_CarRoyaleRumble",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/shadowsvsghosts-tile-1024x512-e112911fdad6.png",
                            playlist_name: "Playlist_Creative_LTM_GhostVsShadowPro",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-ltm-redvsblue-redvsblueogprison-modetile-1024x512-fd11bfc882ab.jpg",
                            playlist_name: "Playlist_Creative_LTM_RVBOGPrison",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/vertigodeathtower-tile-1024x512-d81265419557.jpg",
                            playlist_name: "Playlist_Creative_LTM_VertigoDR",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Make your way to the top of the Deathtower if you have the nerves. Or take your time in the relaxing default deathrun and wait for the gravity bonus. Built by: CROSS_MCPENNA"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-ltm-redvsblue-proredvsblue-modetile-1024x512-2b66e178a79c.jpg",
                            playlist_name: "Playlist_Creative_LTM_ProRVB100",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-ltm-redvsblue-greenredvsyellowblue-modetile-1024x512-76a9e2abb108.jpg",
                            playlist_name: "Playlist_Creative_LTM_GvRvYvB",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17br-carfight-modetile-1024x512-8ea900035bf5.jpg",
                            playlist_name: "Playlist_Creative_LTM_RGYPCarFight",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Team Deathmatch with 2 Teams 12 players each. First to 100 points wins",
                            display_name: "Red&Green vs Yellow&Purple Car Fight"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/redvsbluearenafightnrmlver-3840x2160-089380c881f5.png",
                            playlist_name: "Playlist_Creative_LTM_RVBGG8v8",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/cozy-ltm-1024x512-d0e06ec0344a.jpg",
                            playlist_name: "Playlist_Creative_LTM_PHModernMall",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-outlawshaven-v2-ltm-1024x512-4b153a249220.jpg",
                            playlist_name: "Playlist_Creative_LTM_OutlawsHaven",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-16v16domination-tile-1024x512-760c18227a12.jpg",
                            playlist_name: "Playlist_Creative_LTM_16v16Dom",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/tropicalthunder-tile-1024x512-0ba3b032b4ed.jpg",
                            playlist_name: "Playlist_Creative_LTM_TropicalThunder",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-tinytown-tile-1024x512-7e050c2f0d17.jpg",
                            playlist_name: "Playlist_Creative_LTM_PWRTinyTown",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-ltm-redvsblue-procleanestredvsblue-modetile-1024x512-f25b2782ab7f.jpg",
                            playlist_name: "Playlist_Creative_LTM_BlitzRVB",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/watermania-tile-1024x512-d8e5827e09fe.jpg",
                            playlist_name: "Playlist_Creative_LTM_WatermaniaUltimate",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/fishinggame-tile-1024x512-6f20e4be96c8.jpg",
                            playlist_name: "Playlist_Creative_LTM_ProFishing",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-volcanowars-tile-1024x512-29d1e3dda875.jpg",
                            playlist_name: "Playlist_Creative_LTM_VolcanoWars",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "It’s Team Chicken vs Team Lizard in this epic battle between two giant volcanoes!\n\nEarn gold by eliminating other players. Visit the Witch Doctor and the Warrior to purchase powerful items and use your gold to upgrade your weapons.\n\nCollect Gold to unleash the power of your team's Volcano God!\n\nFirst team to 1000 points wins!\n\nBuilt by: KINGQUILLO",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-ltm-redvsblue-redvsbluelava-modetile-1024x512-9cdf65d33849.jpg",
                            playlist_name: "Playlist_Creative_LTM_RVBLava",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/cityroyale-tile-1024x512-845b717bcd44.jpg",
                            playlist_name: "Playlist_Creative_LTM_CityRoyale",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/apollonias-tile-1024x512-f527ac8e3e52.jpg",
                            playlist_name: "Playlist_Creative_LTM_FiendSlayer",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/lastmaze-cltm-tile-1024x512-d1df0fea5d4f.jpg",
                            playlist_name: "Playlist_Creative_LTM_16v16LastMaze",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/marstycoonpaint-tile-1024x512-774f4081a5e6.jpg",
                            playlist_name: "Playlist_Creative_LTM_MarsTycoon",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/zonewarswithplacementpoints-tile-1024x512-c96ebf0e804d.png",
                            playlist_name: "Playlist_Creative_LTM_SimpleTriosZoneWars",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-tide-tycoon-v2-tile-1024x512-f690fab9c5f4.jpg",
                            playlist_name: "Playlist_Creative_LTM_TideTycoonFishing",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/redvsgreenskywarsdinosaurs-modetile-1024x512-ed656e702565.jpg",
                            playlist_name: "Playlist_Creative_LTM_RVGDinosaurs",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/tile-fishingtycoon-1024x512-182b090e54ad.jpg",
                            playlist_name: "Playlist_Creative_LTM_FishtopiaFishTycoon",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/cltm-skyland-tile-1024x512-b39a7b067455.jpg",
                            playlist_name: "Playlist_Creative_LTM_Skyland",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Welcome to Skyland. Build your island, unlock new areas, team up and take down your opponents in this open world adventure.\n\nBuilt by: BrendannnD"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/zoogungame-tile-1024x512-986798edc9b4.jpg",
                            playlist_name: "Playlist_Creative_LTM_ZooGunGame",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/fortwars-tile-1024x512-b2d34ba7a6cf.jpg",
                            playlist_name: "Playlist_Creative_LTM_FortWars",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-pandvilboxfight-tile-1024x512-a4c933e57f01.jpg",
                            playlist_name: "Playlist_Creative_LTM_PandvilBoxfights",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-britecitymafia-tile-1024x512-8fd79d140393.jpg",
                            playlist_name: "Playlist_Creative_LTM_BriteCityMafia",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-hotelfight-ltm-1024x512-10e64ee920fb.jpg",
                            playlist_name: "Playlist_Creative_LTM_HotelFight",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-infiniteleveldeathrun-ltm-1024x512-b13eef017ef9.jpg",
                            playlist_name: "Playlist_Creative_LTM_InfiniteDefaultDeathRun",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-themeparksummerevent-ltm-1024x512-8b0d1b06811d.jpg",
                            playlist_name: "Playlist_Creative_LTM_ThemeParkSummerEvent",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-worldsendmillionairemansion-tile-1024x512-89271ccdc915.jpg",
                            playlist_name: "Playlist_Creative_LTM_WorldsEndMillionaireMansion",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-salvagesurvival-tile-1024x512-393d568a7827.jpg",
                            playlist_name: "Playlist_Creative_LTM_PWR_SalvageSurvival",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: "Salvage Survival | PWR"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/ltmlivingroom-1024x512-906e536310c0.png",
                            playlist_name: "Playlist_Creative_LTM_LivingRoomPVP",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-zombieisland-tile-1024x512-ef65950d75f5.jpg",
                            playlist_name: "Playlist_Creative_LTM_ZombieIsland",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/17cm-themarch-modetile-1024x512-2bab4ba16987.jpg",
                            playlist_name: "Playlist_Creative_LTM_TheMarch",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/communitycltm-wildlands-tile-1024x512-fd2c08227990.jpg",
                            playlist_name: "Playlist_Creative_LTM_WildlandsSurvival",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/16cm-ltm-orangevsblue-motd-1024x512-1024x512-f505e1ea069a.jpg",
                            playlist_name: "Playlist_Creative_LTM_OVB100GG",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/snipervsrunners-playlist-1024x512-570337797.png",
                            playlist_name: "Playlist_Creative_LTM_Doll",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: eudyn\n\nEnter with your team and face others to show which team has the best sniping skills!\n\nYou have limited respawn and only 1% Health. Be careful with those jumps: they could cost you a life. As a Runner, you can collect coins to get Smoke Grenades, but use them wisely."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11-20-realisticsolos-4302x2420-581106352.png",
                            playlist_name: "Playlist_Creative_LTM_Realistic",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: PrettyBoyyAM\n\nRealistic Solos, a place where players can practice and enhance their endgame skills. Random storms, random loot, and a random POI each round! \n\nEarn points to win! Points are earned by getting eliminations and/or remaining alive."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/moneywars-updated-1024x512-1202x601-8fe62d7f03f2.jpg",
                            playlist_name: "Playlist_Creative_LTM_Money",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Created By: DolphinDom and Wertandrew\n\nCompete against other players in this bed war mode: protect your bed at all costs!\n\nCollect materials and upgrade your base defenses while unlocking better weapons to defeat your opponents!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11-25-thefishlymansion-hideandseek-4302x2420-622382121.png",
                            playlist_name: "Playlist_Creative_LTM_Fishy",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: Fatal Creations\n\nPlay Hide & Seek in this luxurious modern mansion with up to 15 others!\n\nThe initial Seeker must eliminate all Hiders until none remain. If the Hiders survive the given time limit, they win!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11-30-summersplash-1024-1024x512-676123100.png",
                            playlist_name: "Playlist_Creative_LTM_Summer",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created by: Team Unite\n\nTake it head-to-head with 15 other players in the ultimate floating deathrun race!\n\nRace to the finish by completing 25 fun and challenging levels to be crowned the winner."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/12-02-tropicalzonewars-1024-1024x512-686642274.png",
                            playlist_name: "Playlist_Creative_LTM_Tropical",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: DonnySC\n\nSurvive as long as you can in this close combat Zone Wars map!\n\nEliminate your opponents and survive as long as possible to earn the most points to win the game!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/12-04-gungamemars-1024-1024x512-f8d00922e3ac.png",
                            playlist_name: "Playlist_Creative_LTM_Mars",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: Senix\n\nTake it head-to-head with 15 other players in the ultimate floating deathrun race!\n\nRace to the finish by completing 25 fun and challenging levels to be crowned the winner."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/12-07-fortnitefashionshow-1024-1024x512-87c1ec87344a.png",
                            playlist_name: "Playlist_Creative_LTM_Fashion",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: Bunni_ & KKSlider\n\nShow off your best cosmetic combos in the Fortnite Fashion Show!\n\nVote for your favorite styles on-stage and show yourself off over 4 rounds that feature Outfits, Emotes, Gliders, and weapons/items!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/1209-lasertagprophunt-1024-1024x512-e10610b17bbc.png",
                            playlist_name: "Playlist_Creative_LTM_Laser",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: Puzzler\n\nCustomize your Prop Hunt experience in Laser Tag Prop Hunt!\n\nAs a Prop, find all the best and top secret hiding spots to avoid the Hunters. As a Hunter, show off your skills in tracking down even the most elusive players hiding as Props."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/12-11-skyfall-duoboxfight-512-1024x512-d8621e90ebc2.png",
                            playlist_name: "Playlist_Creative_LTM_Skyfall",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: Immature Gamer\n\nTeam up and get ready for action in this new spin on Box Fights!\n\nPeriodically, all builds are destroyed and the remaining players are dropped into a smaller arena with upgraded weapons in this Duo Box Fight!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/0113-underwaterlabs-1024-1024x512-7c922c2d982f.png",
                            playlist_name: "Playlist_Creative_LTM_Underwater",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/01-15-theunderworldgungame-1024-1024x512-c23baaed5963.png",
                            playlist_name: "Playlist_Creative_LTM_Underworld",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Save your soul from the firey inferno known as the Underworld!  2-16 player Gun Game.\n\nEliminate opponents to progress through the weapons. The first player to reach 30 wins."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/0120-pro100-1024-1024x512-ccdec6b69980.jpg",
                            playlist_name: "Playlist_Creative_LTM_PRO100",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/survivalsimulator-desert-tile-1920x960-6593f44247be.jpg",
                            playlist_name: "Playlist_Creative_LTM_SurvivalSimDesert",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/pro2-1024-1024x512-ca03daa7c6d6.png",
                            playlist_name: "Playlist_Creative_LTM_PRO2",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/zonewars-4-1024-1024x512-589455537977.jpg",
                            playlist_name: "Playlist_Creative_LTM_ZWPlacement",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/222level-defaultdeathrun-tile-1920x960-b762e5df25f3.jpg",
                            playlist_name: "Playlist_Creative_LTM_222DeathRun",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11-27-thepitffa-4302x2420-622472348.png",
                            playlist_name: "Playlist_Creative_LTM_Pit",
                            hidden: false,
                            violator: "COMMUNITY CREATION",
                            _type: "FortPlaylistInfo",
                            description: "Created By: Geerzy\n\nJump into The Pit and battle it out with any weapons you want!\n\nEarn coins by eliminating players and use them to purchase powerups. Builds will reset every 15 minutes and the player with the most eliminations when the time runs out wins!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-marvelknockout-mystique-ltm-tile-1024x512-969813783.jpg",
                            playlist_name: "Playlist_Cobalt_HT_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-marvelknockout-wolverine-ltm-tile-1024x512-969818067.jpg",
                            playlist_name: "Playlist_Cobalt_HT_Trios",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/marvel-standoff-1024x512-010756809.jpg",
                            playlist_name: "Playlist_hightower",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/marvel-standoff-1024x512-010756809.jpg",
                            playlist_name: "Playlist_hightower_12",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-marveltakeoverltm-playlisttile-1024x512-1024x512-081336658.jpg",
                            playlist_name: "Playlist_HighTower_Hydro",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-marveltakeoverltm-playlisttile-1024x512-1024x512-081332895.jpg",
                            playlist_name: "Playlist_HighTower_Hydro_12v12",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: "Marvel Takeover"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-rallyroyaleltm-playlisttile-1024x512-268454418.jpg",
                            playlist_name: "Playlist_Argon_HighTower_Duos_16",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-rallyroyaleltm-playlisttile-1024x512-268454418.jpg",
                            playlist_name: "Playlist_Argon_HighTower_Solo_16",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_subname: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-tiltedtaxi-motd-1024x512-1024x512-890727217.jpg",
                            playlist_name: "Playlist_Nitrogen12_Trios",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-tiltedtaxi-motd-1024x512-1024x512-890727217.jpg",
                            playlist_name: "Playlist_Nitrogen12_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-knockoutairraid-motd-1024x512-1024x512-674280089.jpg",
                            playlist_name: "Playlist_Antares_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-knockoutairraid-motd-1024x512-1024x512-674280089.jpg",
                            playlist_name: "Playlist_Antares_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-tankbattle-motd-1024x512-1024x512-674279961.jpg",
                            playlist_name: "Playlist_Tank_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-tankbattle-motd-1024x512-1024x512-674279961.jpg",
                            playlist_name: "Playlist_Tank_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-ragstoriches-motd-1024x512-1024x512-674279937.jpg",
                            playlist_name: "Playlist_Titanium_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-ragstoriches-motd-1024x512-1024x512-674279937.jpg",
                            playlist_name: "Playlist_Titanium_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-ragstoriches-motd-1024x512-1024x512-674279937.jpg",
                            playlist_name: "Playlist_Titanium_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-knockoutshuffle-motd-1024x512-1024x512-674280182.jpg",
                            playlist_name: "Playlist_Mira_Squads",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-knockoutshuffle-motd-1024x512-1024x512-674280182.jpg",
                            playlist_name: "Playlist_Mira_Duos",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-knockoutshuffle-motd-1024x512-1024x512-674280182.jpg",
                            playlist_name: "Playlist_Mira_Trios",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-fishingfrenzy-motd-1024x512-1024x512-674279890.jpg",
                            playlist_name: "Playlist_Polaris_Solo",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Solid_Gold_Playlist_Tile_1025x512-1024x512-ea1253854e55ed01f3d6e012ecfa373cc1b38d37.jpg",
                            playlist_name: "Playlist_SolidGold_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-payback-motd-1024x512-1024x512-500823017.jpg",
                            playlist_name: "Playlist_RespawnPB_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Infiltration_PlaylistTile-1024x512-795e414e6a7e6b050d19c3ef3f226ae0ece00875.jpg",
                            playlist_name: "Playlist_Infiltration",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Payload_PlaylistTile_1024x512-1024x512-93cd224901b5d4d6585ed1170c700be4b4d2a5cf.png",
                            playlist_name: "Playlist_Uranium_P",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Cyclone_Astronomical_PlaylistTile_Main-1024x512-ab95f8d30d0742ba1759403320a08e4ea6f0faa0.jpg",
                            playlist_name: "Playlist_Music_Highest   ",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Drop into Sweaty Sands for the ride of your life. (Photosensitivity Warning)",
                            display_name: "Travis Scott’s Astronomical"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Cyclone_Astronomical_PlaylistTile_Main-1024x512-ab95f8d30d0742ba1759403320a08e4ea6f0faa0.jpg",
                            playlist_name: "Playlist_Music_Higher",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Drop into Sweaty Sands for the ride of your life. (Photosensitivity Warning)",
                            display_name: "Travis Scott’s Astronomical"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Cyclone_Astronomical_PlaylistTile_Main-1024x512-ab95f8d30d0742ba1759403320a08e4ea6f0faa0.jpg",
                            playlist_name: "Playlist_Music_High",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Drop into Sweaty Sands for the ride of your life. (Photosensitivity Warning)",
                            display_name: "Travis Scott’s Astronomical"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Cyclone_Astronomical_PlaylistTile_Main-1024x512-ab95f8d30d0742ba1759403320a08e4ea6f0faa0.jpg",
                            playlist_name: "Playlist_Music_Med",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Drop into Sweaty Sands for the ride of your life. (Photosensitivity Warning)",
                            display_name: "Travis Scott’s Astronomical"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Cyclone_Astronomical_PlaylistTile_Main-1024x512-ab95f8d30d0742ba1759403320a08e4ea6f0faa0.jpg",
                            playlist_name: "Playlist_Music_Low",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Drop into Sweaty Sands for the ride of your life. (Photosensitivity Warning)",
                            display_name: "Travis Scott’s Astronomical"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Cyclone_Astronomical_PlaylistTile_Main-1024x512-ab95f8d30d0742ba1759403320a08e4ea6f0faa0.jpg",
                            playlist_name: "Playlist_Music_Lowest",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Drop into Sweaty Sands for the ride of your life. (Photosensitivity Warning)",
                            display_name: "Travis Scott’s Astronomical"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Knockout_PlaylistTile-1024x512-455f3aa88d1c74b606b17f84852a4c1bbd82eedb.jpg",
                            playlist_name: "Playlist_Cobalt_Duos_P_Respawn",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Quibi_TheSauce_Nighttime_LTM_1024x512-1024x512-7baf11f7c087e3b74e5a2c3eaf5c2d9cf9f445e4.jpg",
                            playlist_name: "Playlist_NowPlaying",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Ayo & Teo are on the hunt to find the squad with the most sauce. Stop by Risky Reels to catch part of the show!",
                            display_name: "Now Playing!"
                        },
                        {
                            playlist_name: "Playlist_Bots_ArsenicCore_Solo_MaxFog",
                            hidden: false,
                            violator: "",
                            _type: "FortPlaylistInfo",
                            display_name: "Dev Autoflush Solos"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/12BR_Knockout_PlaylistTile-1024x512-455f3aa88d1c74b606b17f84852a4c1bbd82eedb.jpg",
                            playlist_name: "playlist_cobalt_squads_p_respawn",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v12/DropZone-1024x512-22ae7474a6c8bdf181e6cee1c546b341b4438fd9.jpg",
                            playlist_name: "playlist_rumble_op",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/SearchDestroy_Tile_1024x512-1024x512-1e61afa9f8eeced10f55155b9a32530322492138.jpg",
                            playlist_name: "Playlist_Creative_SourDropper_2",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Search and Destroy is a community made LTM. ",
                            display_subname: "COVE",
                            display_name: "Search and Destroy"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/SearchDestroy_Tile_1024x512-1024x512-1e61afa9f8eeced10f55155b9a32530322492138.jpg",
                            playlist_name: "Playlist_Creative_SourDropper",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Search and Destroy is a community made LTM. ",
                            display_subname: "FACTORY",
                            display_name: "Search and Destroy"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11br-battlelabs-motd-1920x1080-1920x1080-a39ad0545dba.jpg",
                            playlist_name: "Playlist_BattleLab",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_BattleLabs_PlaylistTile-(2)-1024x512-ca5f4e84a2941264f787239caa5458d0eabd39e3.jpg",
                            playlist_name: "Playlist_BattleLab_Fortnitemares_MaxFog",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile_Blue-1024x512-0f76af6296545de1b2d9da766e76475418bc5940.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_Random",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Party queue Zone Wars.  Zone Wars games are a fast paced fight to the finish, forcing you to battle high ground while running from the fast moving storm. Zone Wars takes place on smaller, community made maps. Randomized spawns and inventory make each round feel unique. Play against 15 other players including anyone in your party.",
                            display_name: "PARTY FFA"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile_Red-1024x512-2e1c5e38b652093029befb6a86a44db844474af8.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_Random2",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Solo queue Zone Wars.  Zone Wars games are a fast paced fight to the finish, forcing you to battle high ground while running from the fast moving storm. Zone Wars takes place on smaller, community made maps. Randomized spawns and inventory make each round feel unique. Play solo against 15 other random players.",
                            display_name: "SOLO FFA"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile_Blue-1024x512-0f76af6296545de1b2d9da766e76475418bc5940.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_solos",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            display_name: "Zone Wars 0"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile_Blue-1024x512-0f76af6296545de1b2d9da766e76475418bc5940.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_D",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "A solo queue, FFA simulation of the end-game scenario in Battle Royale with a quick moving zone. Randomized spawns and inventory items make each round unique. Stick around after the first game. there are multiple rounds in each session.  Zone Wars is a collection of games made by the community. The four maps included in this playlist are:  Desert created by JotaPeGame. Code: jotapegame Downhill River created by Enigma. Code: enigma Vortex created by Zeroyahero. Code: zeroyahero Colosseum created by Jesgran. Code: jesgran",
                            display_name: "Zone Wars 4"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile_Black-1024x512-23ba95e82931361ce535a643fdac54e120254374.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_Random_Solos",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "A solo queue, FFA simulation of the end-game scenario in Battle Royale with a quick moving zone. Randomized spawns and inventory items make each round unique. Stick around after the first game. there are multiple rounds in each session.  Zone Wars is a collection of games made by the community. The four maps included in this playlist are:  Desert created by JotaPeGame. Code: jotapegame Downhill River created by Enigma. Code: enigma Vortex created by Zeroyahero. Code: zeroyahero Colosseum created by Jesgran. Code: jesgran",
                            display_name: "Zone Wars 3"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile-1024x512-a2741f113a7178ca15d71d281dcc2b614ff90754.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_Random_Squads",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "A solo queue, FFA simulation of the end-game scenario in Battle Royale with a quick moving zone. Randomized spawns and inventory items make each round unique. Stick around after the first game. there are multiple rounds in each session.  Zone Wars is a collection of games made by the community. The four maps included in this playlist are:  Desert created by JotaPeGame. Code: jotapegame Downhill River created by Enigma. Code: enigma Vortex created by Zeroyahero. Code: zeroyahero Colosseum created by Jesgran. Code: jesgran",
                            display_name: "Zone Wars 0"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-siphon-motd-1024x512-1024x512-501004672.jpg",
                            playlist_name: "Playlist_Vamp_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-siphon-motd-1024x512-1024x512-501004672.jpg",
                            playlist_name: "Playlist_Respawn_Vamp_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_2v2_GunFright_LTM-1024x512-f3b0f0157e8652a23db8abc23814d97893179e20.jpg",
                            playlist_name: "Playlist_Creative_Hyena_G",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Code BluDrive "
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_LTM_ModeTile-1024x512-aae4d5b5eb1ea4eeb31f852c8b98516681bfe769.jpg",
                            playlist_name: "Playlist_DADBRO_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_LTM_ModeTile-1024x512-aae4d5b5eb1ea4eeb31f852c8b98516681bfe769.jpg",
                            playlist_name: "Playlist_DADBRO_Squads_12",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_LTM_ModeTile-1024x512-aae4d5b5eb1ea4eeb31f852c8b98516681bfe769.jpg",
                            playlist_name: "Playlist_DADBRO_Squads_8",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile-1024x512-a2741f113a7178ca15d71d281dcc2b614ff90754.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_A",
                            violator: "PARTY FFA",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Code jesgran Zone Wars - Arena  A party, FFA simulation of the end-game scenario in Battle Royale with a quick-moving zone. Eliminate the competition as you avoid the Storm. Randomized spawns and inventory items make each round unique. Stick around after the first game. There are multiple rounds in each session. Become the ultimate gladiator in this Colosseum style island. An open style island demands quick building. Created by Jesgran.",
                            display_name: "Colosseum"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile_Blue-1024x512-0f76af6296545de1b2d9da766e76475418bc5940.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_DH",
                            violator: "PARTY FFA",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Code enigma S10 Enigmas Downhill River Zonewars X  A party, FFA simulation of the end-game scenario in Battle Royale with a quick-moving zone. Eliminate the competition as you avoid the Storm. Randomized spawns and inventory items make each round unique. Stick around after the first game. There are multiple rounds in each session.  Stay out of the storm as you move downhill through a river in this original style Zone Wars island. Community launch pads and a consistent Storm path allows for familiarity after a few rounds.  Created by Enigma.",
                            display_name: "Downhill River"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_ZoneWars_In-Game_ModeTile_Black-1024x512-23ba95e82931361ce535a643fdac54e120254374.jpg",
                            playlist_name: "Playlist_Creative_ZebraWallet_V",
                            violator: "PARTY FFA",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Code zeroyahero Vortex Zone Wars A party, FFA simulation of the end-game scenario in Battle Royale with a quick-moving zone. Eliminate the competition as you avoid the Storm. Randomized spawns and inventory items make each round unique. Stick around after the first game. There are multiple rounds in each session.  This island puts a unique twist on the Zone Wars game with mountainous terrain to traverse. The elevation change from zone to zone can be quite drastic!  Created by Zeroyahero",
                            display_name: "Vortex"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10BR_TheCombine_ModeTile-1024x512-3aa8ebdfe1df7d9995e824a781eacdb954ee9615.jpg",
                            playlist_name: "Playlist_Crucible_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlisttiles/BR_LTM-Tile_Playground-1024x512-53db8a4b5fb41251af279eaf923bc00ecbc17792.jpg",
                            playlist_name: "Playlist_Creative_PlayOnly_40",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/10CM_LTM_KnockTown_Playlist-1024x512-72e32b88b332b4d3ee3ee5255eff9522b660485c.jpg",
                            playlist_name: "Playlist_Creative_KaleTofu",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Bounty_LTM_V2_Playlist_Tile-1024x512-3528a95d869a442d61687b52253a56a36c92eea3.jpg",
                            playlist_name: "Playlist_Bounty_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Bounty_LTM_V2_Playlist_Tile-1024x512-3528a95d869a442d61687b52253a56a36c92eea3.jpg",
                            playlist_name: "Playlist_Bounty_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Bounty_LTM_V2_Playlist_Tile-1024x512-3528a95d869a442d61687b52253a56a36c92eea3.jpg",
                            playlist_name: "Playlist_Bounty_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/09CM_WorldCup_FeatIsland_WorldRun_ModeTile-1024x512-34d66c90603f4e64ebd56054b889c4ec163abea5.jpg",
                            playlist_name: "Playlist_Creative_Squad_Battle_16_B",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_SneakySilencers-1024x512-1669e2eeddca63b61e9b94cc19c3ec502fd33f29.jpg",
                            playlist_name: "Playlist_Sneaky_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_SneakySilencers-1024x512-1669e2eeddca63b61e9b94cc19c3ec502fd33f29.jpg",
                            playlist_name: "Playlist_Sneaky_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/09CM_WorldCup_FeatIsland_JunkyardJuke_ModeTile-1024x512-7a2585ce248f1efa438674c368b37116dc5514de.jpg",
                            playlist_name: "Playlist_Creative_Squad_Battle_16_A",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlisttiles/09CM_WorldCup_FeatIsland_SkyStation_ModeTile-1024x512-a5424f9ac27626a73646c9fd158901c4c363ec0c.jpg",
                            playlist_name: "Playlist_Creative_Squad_Battle_32_A",
                            violator: "COMMUNITY CREATION",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Created by Team Evolve. Featured in the Fortnite World Cup Creative Finals.  Battle other squads to capture zones and score points! Any player can capture a zone and score points for your team. Use impulse grenades to blast other teams out of the capture zones. Players can now earn XP after each game and the top three teams will earn bonus XP."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-oneshot-motd-1024x512-1024x512-347318026.jpg",
                            playlist_name: "Playlist_Low_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_HeavyMetal_ModeTile-1024x512-4db8223707fb313220eef577dafde5c14106e49d.jpg",
                            playlist_name: "Playlist_Heavy_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/09CM_In-Game_PropHunt_ModeTile-1024x512-1510311027a93a720b42ed22e711c7e478931adb.jpg",
                            playlist_name: "Playlist_Creative_PuppyHugs",
                            violator: "PLAYER MADE!",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Unvaulted-1024x512-d3cbe3c4a756190279af4ce98773d6599f7aab4f.jpg",
                            playlist_name: "Playlist_Unvaulted_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/EN_CM09_BeachAssaultCreativeLTM_ContestWinner_ModeTile-1024x512-9cdeb2e0ea37179a37d3384cf73c9949d2d19546.jpg",
                            playlist_name: "Playlist_Creative_BeachAssault",
                            violator: "PLAYER MADE",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            display_name: "BEACH ASSAULT BY PRUDIZ"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Barebones-1024x512-4a29337febb04e9043d57c9e61afe849f8a9e9c7.jpg",
                            playlist_name: "Playlist_Hard_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "This mode has the map, compass, storm timer and many other elements of the Heads Up Display turned off."
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_BlueWeapons_ModeTile-1024x512-0c38f1bc3b991943e3f6650bf7acfbcdd8739b1e.jpg",
                            playlist_name: "Playlist_Blue_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11br-solid-gold-motd-1920x1080-1920x1080-43bce9103b4a.jpg",
                            playlist_name: "Playlist_SolidGold_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11br-solid-gold-motd-1920x1080-1920x1080-43bce9103b4a.jpg",
                            playlist_name: "Playlist_SolidGold_Trios",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_PurpleReign_ModeTile-1024x512-c5a7e2bd3f32b83f17e4fa28817312ab6210133c.jpg",
                            playlist_name: "Playlist_Purple_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_FullAuto_ModeTile-1024x512-d1532221d738ba3aed434512b7c670e72b89f474.jpg",
                            playlist_name: "Playlist_Auto_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_HeavyMetal_ModeTile-1024x512-4db8223707fb313220eef577dafde5c14106e49d.jpg",
                            playlist_name: "Playlist_Heavy_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_HeavyMetal_ModeTile-1024x512-4db8223707fb313220eef577dafde5c14106e49d.jpg",
                            playlist_name: "Playlist_Heavy_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-gungame-motd-1024x512-1024x512-501005598.jpg",
                            playlist_name: "Playlist_Gungame_Reverse",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-gungame-motd-1024x512-1024x512-501005598.jpg",
                            playlist_name: "Playlist_Gungame_Reverse_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_Surfin_ModeTile-1024x512-ebff23e30b121cfe3eecf173949c055325522090.jpg",
                            playlist_name: "Playlist_Race_12",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_DeaglesandHeadshots_ModeTile-1024x512-4d969f5a9126ba71b7ee77088fd22df5b4c7caba.jpg",
                            playlist_name: "Playlist_Beagles_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_LeaveNoneBehind_ModeTile-1024x512-bfe65d02a5d42577d22c133d25ad9c9fb62a35a0.jpg",
                            playlist_name: "Playlist_Behind_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_LoadoutSwap_ModeTile-1024x512-f73c146f8ccc7998aab14f8c1957f0ad01faa933.jpg",
                            playlist_name: "Playlist_Swap_Squads_Respawn",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_TankBattle_ModeTile-1024x512-48554aae511d9c5a7ac1a5d4fd54e0f5a37bd66d.jpg",
                            playlist_name: "Playlist_Tank_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_StrategicStructures_ModeTile-1024x512-4f6f448375284fef60fe2a2c15f292115ebec558.jpg",
                            playlist_name: "Playlist_Strategic_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_BuildersParadise_ModeTile-1024x512-730a5ffe51c8f0420b91529d1fc05e081aa2071c.jpg",
                            playlist_name: "Playlist_Paradise_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_PowerUp_ModeTile-1024x512-7e1824071133f9eac4ca44c701605923893c85bf.jpg",
                            playlist_name: "Playlist_Pow_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_BuilttoLast_ModeTile-1024x512-cf95f4701f41c608ae8590fe588a5a0ea25ed68a.jpg",
                            playlist_name: "Playlist_Care_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_Tag_ModeTile-1024x512-d4471981ccdc8d9f444d1f416b3f4458612da006.jpg",
                            playlist_name: "Playlist_Tag_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_SiphonRumble_ModeTile-1024x512-02ad3c97e4cdc7172f6ea59140b89b004f95886a.jpg",
                            playlist_name: "Playlist_Respawn_20_Sif",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/14DaysofSummer/09BR_14DoS_LTM_LavaRumble_ModeTile-1024x512-29cc6ad680519d8f792bf1fa4053cf9191f84b6e.jpg",
                            playlist_name: "Playlist_Respawn_20_Lava",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-payback-motd-1024x512-1024x512-500823017.jpg",
                            playlist_name: "Playlist_Respawn_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/09BR_LTM_HordeRush_Mode_Tile-1024x512-a844840eb58db868b6abfbe18fc8a8f483e18c60.jpg",
                            playlist_name: "Playlist_Mash_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/09BR_LTM_DowntownDrop_Screenshot_ModeTile-1024x512-d8ce0a16ae59e2a2f501813ddf540a00e60098b5.jpg",
                            playlist_name: "Playlist_Creative_Vigilante",
                            violator: "PLAYER MADE!",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Created by NotNellaf & Tollmolia in collaboration with Jordan Brand. "
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/09BR_Social_LTM_WicksBounty_Announce_PlaylistTile-1024x512-df3870c355530a7591c7a3fa453c15686c862989.jpg",
                            playlist_name: "Playlist_Wax_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/09BR_Social_LTM_WicksBounty_Announce_PlaylistTile-1024x512-df3870c355530a7591c7a3fa453c15686c862989.jpg",
                            playlist_name: "Playlist_Wax_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/09BR_Social_LTM_WicksBounty_Announce_PlaylistTile-1024x512-df3870c355530a7591c7a3fa453c15686c862989.jpg",
                            playlist_name: "Playlist_Wax_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_LTM_Endgame_InGame_Mode-Tile-1024x512-03bb0dc121ae8b2dcd27b8b386670737093d0c83.jpg",
                            playlist_name: "Playlist_Ashton_Sm",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/EN_08BR_DeepFried_Mode_Tile-1024x512-227979fa27053f858066a1a47d68b55f792fded1.jpg",
                            playlist_name: "Playlist_Barrier_16_B_Lava",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-airroyale-ltm-playlisttile-1024x512-1024x512-f7a7548d223d.jpg",
                            playlist_name: "Playlist_Goose_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-airroyale-ltm-playlisttile-1024x512-1024x512-f7a7548d223d.jpg",
                            playlist_name: "Playlist_Goose_Duos_24",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-snowdown-airroyale-ltm-playlisttile-1024x512-1024x512-f7a7548d223d.jpg",
                            playlist_name: "Playlist_Goose_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/cm07-news-featured-creativemode-announce-v2-1920x1080-267ac4e33a15.jpg",
                            playlist_name: "Playlist_PlaygroundV2",
                            hidden: true,
                            _type: "FortPlaylistInfo",
                            description: "Matchmake with others and play the best games made by the community!"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_LTM_FloorIsLava_ModeTile-1024x512-f1af4cd98c7ff0ce4058f4e3b65a853641d0a35e.jpg",
                            playlist_name: "Playlist_Fill_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_LTM_FloorIsLava_ModeTile-1024x512-f1af4cd98c7ff0ce4058f4e3b65a853641d0a35e.jpg",
                            playlist_name: "Playlist_Fill_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_LTM_FloorIsLava_ModeTile-1024x512-f1af4cd98c7ff0ce4058f4e3b65a853641d0a35e.jpg",
                            playlist_name: "Playlist_Fill_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_LTM_SolidGreen_ModeTile_Squads-1024x512-f0d931472907d54ffaa52ef81f78bf9d5fcfaa2d.jpg",
                            playlist_name: "Playlist_Green_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_LTM_SolidGreen_ModeTile_Squads-1024x512-f0d931472907d54ffaa52ef81f78bf9d5fcfaa2d.jpg",
                            playlist_name: "Playlist_Green_Squad",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_LTM_SolidGreen_ModeTile_Squads-1024x512-f0d931472907d54ffaa52ef81f78bf9d5fcfaa2d.jpg",
                            playlist_name: "Playlist_Green_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_WinterFest_Social_Slide_LTM_MOTD_1024x512-1024x512-e568425bdc8ac88c7cea6a277d0cf3f081563541.jpg",
                            playlist_name: "Playlist_Slide_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Unvaulted-1024x512-d3cbe3c4a756190279af4ce98773d6599f7aab4f.jpg",
                            playlist_name: "Playlist_Classic_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Unvaulted-1024x512-d3cbe3c4a756190279af4ce98773d6599f7aab4f.jpg",
                            playlist_name: "Playlist_Classic_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_LTM_Drifting50s_Powder-1024x512-5fb24cfd4d83e4cd3a3589b126313beba9cc69a7.jpg",
                            playlist_name: "Playlist_Hover",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_LTM_Drifting50s_Powder-1024x512-5fb24cfd4d83e4cd3a3589b126313beba9cc69a7.jpg",
                            playlist_name: "Playlist_Hover_64",
                            violator: "Large Team Mode",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_LTM_Drifting50s_Powder-1024x512-5fb24cfd4d83e4cd3a3589b126313beba9cc69a7.jpg",
                            playlist_name: "Playlist_Hover_48",
                            violator: "Large Team Mode",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-sword-ltm-2-1920x1080-3b4c55aa225e.jpg",
                            playlist_name: "Playlist_Sword_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-sword-ltm-2-1920x1080-3b4c55aa225e.jpg",
                            playlist_name: "Playlist_Sword_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-sword-ltm-2-1920x1080-3b4c55aa225e.jpg",
                            playlist_name: "Playlist_Sword_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_LTM_LoveShot_v2-1024x512-cd7b917157be2472bebc3db3b125e9b20174c748.jpg",
                            playlist_name: "Playlist_Love_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_LTM_LoveShot_v2-1024x512-cd7b917157be2472bebc3db3b125e9b20174c748.jpg",
                            playlist_name: "Playlist_Love_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_LTM_LoveShot_v2-1024x512-cd7b917157be2472bebc3db3b125e9b20174c748.jpg",
                            playlist_name: "Playlist_Love_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Catch_LTM_ModeTile_1024x512-1024x512-c082fbf692a53d713da02592e9de69faeefd587c.jpg",
                            playlist_name: "Playlist_Toss_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_LTM_CatchSquads-1024x512-7289222d56b08ef8de20c7187af2670496dca3df.jpg",
                            playlist_name: "Playlist_Toss_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-catch-motd-1024x512-1024x512-500636478.jpg",
                            playlist_name: "Playlist_Toss_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/15br-nfl-sb55-playlist-tile-1024x512-d1c4ab078f45.jpg",
                            playlist_name: "Playlist_Omaha",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_WinterDeimos_Squads-1024x512-cf4323aa9c2cfd027484cf4da14544128e3d4c7e.jpg",
                            playlist_name: "Playlist_Deimos_Squad_Winter",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_WinterDeimos_Duos_-1024x512-84315aac8d1fcfb840deba46c4dafda8e9005b2a.jpg",
                            playlist_name: "Playlist_Deimos_Duo_Winter",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_ModeTile_WinterDeimos_Solo-1024x512-5c759fe60ec85988f35c729b5fb6a7993d8dbb58.jpg",
                            playlist_name: "Playlist_Deimos_Solo_Winter",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/11br-oneshot-motd-1920x1080-1920x1080-6f69e6c34cce.jpg",
                            playlist_name: "Playlist_Low_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_GroundGame-1024x512-37a4d1d335b4c9427bdc672db0f335f4df813874.jpg",
                            playlist_name: "Playlist_Ground_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_WildWest-1024x512-42779242a5a73d654332d9d0afe0983f9d8401d0.jpg",
                            playlist_name: "Playlist_WW_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Slide-1024x512-189625349e80dc81e225691aa952ffd280996058.jpg",
                            playlist_name: "Playlist_Slide_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_GroundGame-1024x512-37a4d1d335b4c9427bdc672db0f335f4df813874.jpg",
                            playlist_name: "Playlist_Ground_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_GroundGame-1024x512-37a4d1d335b4c9427bdc672db0f335f4df813874.jpg",
                            playlist_name: "Ground_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-oneshot-motd-1024x512-1024x512-347318026.jpg",
                            playlist_name: "Playlist_Low_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_HighExplosives50s-1024x512-3a8d44af3c2718b5aaaaebbd4627258a657bf0bf.jpg",
                            playlist_name: "Playlist_50v50HE",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_HighExplosives-1024x512-4afc4219531db710e56f3b038e7cd84ca2be7675.jpg",
                            playlist_name: "Playlist_HighExplosives_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_HighExplosives-1024x512-4afc4219531db710e56f3b038e7cd84ca2be7675.jpg",
                            playlist_name: "Playlist_HighExplosives_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_HighExplosives-1024x512-4afc4219531db710e56f3b038e7cd84ca2be7675.jpg",
                            playlist_name: "Playlist_HighExplosives_Squads / Event 24",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_WildWest-1024x512-42779242a5a73d654332d9d0afe0983f9d8401d0.jpg",
                            playlist_name: "Playlist_WW_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_SneakySilencers-1024x512-1669e2eeddca63b61e9b94cc19c3ec502fd33f29.jpg",
                            playlist_name: "Playlist_Sneaky_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_FoodFight16-1024x512-309538a1b961b5ab0c22417ab34170cc302bbab8.jpg",
                            playlist_name: "Playlist_Barrier_16",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_CloseEncounters50s-1024x512-03dcc058e1bec3e853b3ee20594128805223b5a3.jpg",
                            playlist_name: "Playlist_Close_50",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_HolidayDisco-1024x512-684bd4b41613e59d895a477389515a8b4878da6a.jpg",
                            playlist_name: "Playlist_Disco_32_Alt",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Slide-1024x512-189625349e80dc81e225691aa952ffd280996058.jpg",
                            playlist_name: "Playlist_Slide_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Barebones-1024x512-4a29337febb04e9043d57c9e61afe849f8a9e9c7.jpg",
                            playlist_name: "Playlist_Hard_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/13br-summersplash-ltm-siphon-motd-1024x512-1024x512-501004672.jpg",
                            playlist_name: "Playlist_Vamp_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR07_14-DoF_Social-1024x512-5fa7dd4752d1f0cc1a101f09cb170d0f5b2a31cf.jpg",
                            playlist_name: "Playlist_33",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Unvaulted-1024x512-d3cbe3c4a756190279af4ce98773d6599f7aab4f.jpg",
                            playlist_name: "Playlist_Unvaulted_Squads",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR_LTM_Unvaulted-1024x512-d3cbe3c4a756190279af4ce98773d6599f7aab4f.jpg",
                            playlist_name: "Playlist_Unvaulted_Trios",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/br06-teamrumble-800x450-800x450-a2265b85af06.jpg",
                            playlist_name: "Playlist_Respawn_24",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/team-rumble-1024x512-534456830.jpg",
                            playlist_name: "Playlist_Respawn_24_Vamp",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/br06-teamrumble-1920x1080-1920x1080-3379c4a1f2e0.jpg",
                            playlist_name: "Playlist_Respawn_24_Alt",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Two large teams fight for the Victory Royale in an action-packed mode where the first team to get the target number of eliminations wins!\n\nRespawn on elimination is ON for the entire match, and gliders can be freely deployed whenever you are far enough from the ground.",
                            display_name: "Team Rumble"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR06_ModeTile_LTM_WildWest-1024x512-f67d9d1dd2ca0b290c92b1380240429f0f257a10.jpg",
                            playlist_name: "Playlist_WW_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR06_LobbyTile_FoodFight-1024x512-5e1540a0a2ba0a1f663d32c60cfec3a360278672.png",
                            playlist_name: "Playlist_Barrier_12",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR06_In-GamePlaylist_TeamTerror-1024x512-310430bdaf1b1dd0ecb4d3b180bb6409b7ff6e27.jpg",
                            playlist_name: "playlist_deimos_50",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR06_Fortnitemares_In-GamePlaylist_1024x512_-Solo-1024x512-7e7fe76e48beb3a06da0592cb26e412265986e4d.jpg",
                            playlist_name: "Playlist_Deimos_Solo",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR06_Fortnitemares_In-GamePlaylist_1024x512_Squads-1024x512-783a0812f6acf1f5931c8015e6ad13c0b76c5a9c.jpg",
                            playlist_name: "Playlist_Deimos_Squad",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR06_Fortnitemares_In-GamePlaylist_1024x512_Duos-1024x512-1eadb7cfab62c9eb5c90c65577c676d5d0bb15c2.jpg",
                            playlist_name: "Playlist_Deimos_Duo",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            display_name: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR06_LobbyTile_LTM_DiscoDomination-1024x512-c79f07de78d8283656fcf4d1ee757f880911d775.jpg",
                            playlist_name: "Playlist_Disco_32",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR06_LobbyTile_LTM_DiscoDomination-1024x512-c79f07de78d8283656fcf4d1ee757f880911d775.jpg",
                            playlist_name: "Playlist_Disco",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LTM_Soaring50s-1024x512-80762dcc260cc959c11dac2ca2f6ae176eb63ef3.jpg",
                            playlist_name: "Playlist_Soaring_Squads",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LTM_Soaring50s-1024x512-80762dcc260cc959c11dac2ca2f6ae176eb63ef3.jpg",
                            playlist_name: "Playlist_Soaring_Duos",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LTM_Soaring50s-1024x512-80762dcc260cc959c11dac2ca2f6ae176eb63ef3.jpg",
                            playlist_name: "Playlist_Soaring_Solo",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_HighStakes_ModeTile-1024x512-741e576e8ae2e30c256ff3508011760ace890711.jpg",
                            playlist_name: "Playlist_Bling_Squads",
                            violator: "",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_HighStakes_ModeTile-1024x512-741e576e8ae2e30c256ff3508011760ace890711.jpg",
                            playlist_name: "Playlist_Bling_Duos",
                            violator: "",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo",
                            description: ""
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/08BR_HighStakes_ModeTile-1024x512-741e576e8ae2e30c256ff3508011760ace890711.jpg",
                            playlist_name: "Playlist_Bling_Solo",
                            violator: "",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Solid_Gold_Playlist_Tile_1025x512-1024x512-ea1253854e55ed01f3d6e012ecfa373cc1b38d37.jpg",
                            playlist_name: "Playlist_50v50SAU",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LobbyTile_LTM_ScoreRoyale-1024x512-b608aaf7840cdf6b7a702c5cbe1848a2247516d6.jpg",
                            playlist_name: "Playlist_Score_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LobbyTile_LTM_ScoreRoyale-1024x512-b608aaf7840cdf6b7a702c5cbe1848a2247516d6.jpg",
                            playlist_name: "Playlist_Score_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LobbyTile_LTM_ScoreRoyale-1024x512-b608aaf7840cdf6b7a702c5cbe1848a2247516d6.jpg",
                            playlist_name: "Playlist_Score_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LTM_Soaring50s-1024x512-80762dcc260cc959c11dac2ca2f6ae176eb63ef3.jpg",
                            playlist_name: "Playlist_Soaring_50s",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-steadystorm-playlisttile-1024x512-1024x512-484343493.jpg",
                            playlist_name: "Playlist_Steady_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-steadystorm-playlisttile-1024x512-1024x512-484343493.jpg",
                            playlist_name: "Playlist_Steady_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-steadystorm-playlisttile-1024x512-1024x512-484343493.jpg",
                            playlist_name: "Playlist_Steady_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LTM_FlyExplosives-1024x512-6283e3392b3aa44794dac64423b22606f8773503.png",
                            playlist_name: "Playlist_FlyExplosives_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LTM_FlyExplosives-1024x512-6283e3392b3aa44794dac64423b22606f8773503.png",
                            playlist_name: "Playlist_FlyExplosives_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR05_LTM_FlyExplosives-1024x512-6283e3392b3aa44794dac64423b22606f8773503.png",
                            playlist_name: "Playlist_FlyExplosives_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlisttiles/LTM_Tile_FinalFight-1024x576-5af82788940faeef422ad204aaa241e36e7c9c56.jpg",
                            playlist_name: "Playlist_Final_12",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlisttiles/BR_LTM-Tile_Playground-1024x512-53db8a4b5fb41251af279eaf923bc00ecbc17792.jpg",
                            playlist_name: "Playlist_Creative_PlayOnly",
                            violator: "",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo",
                            display_subname: "-"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistimages/BR_LTM-Tile_Tactics-Showdown-1024x512-a84753f49eb70d8751a99b4db83cdb5eb8290166.jpg",
                            playlist_name: "Playlist_Taxes_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistimages/BR_LTM-Tile_Tactics-Showdown-1024x512-a84753f49eb70d8751a99b4db83cdb5eb8290166.jpg",
                            playlist_name: "Playlist_Taxes_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistimages/BR_LTM-Tile_Tactics-Showdown-1024x512-a84753f49eb70d8751a99b4db83cdb5eb8290166.jpg",
                            playlist_name: "Playlist_Taxes_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlisttiles/LTM_Tile_FinalFight-1024x576-5af82788940faeef422ad204aaa241e36e7c9c56.jpg",
                            playlist_name: "Playlist_Final_20",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR04_LTM_SniperShootout-1024x512-bcaf8004961e4e374d0603813f840f4b575d230b.jpg",
                            playlist_name: "Playlist_Snipers_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR04_LTM_SniperShootout-1024x512-bcaf8004961e4e374d0603813f840f4b575d230b.jpg",
                            playlist_name: "Playlist_Snipers_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR04_LTM_SniperShootout-1024x512-bcaf8004961e4e374d0603813f840f4b575d230b.jpg",
                            playlist_name: "Playlist_Snipers_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlisttiles/BR04_LTM_BlitzShowdown-1024x512-7eccbc505214ac522cc5dde7b3ceaa3a5f99e754.png",
                            playlist_name: "Playlist_Comp_Blitz_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR04_LTM_5x20-1024x512-451b402db5751c25a1e7616930c5ae37d8b20710.png",
                            playlist_name: "Playlist_5x20",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_In-Game_LTM_Blitz_Masked_ModeTile-1024x512-ca1788d0b956e789da358481747272e1bb64c710.jpg",
                            playlist_name: "Playlist_Blitz_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_In-Game_LTM_Blitz_Masked_ModeTile-1024x512-ca1788d0b956e789da358481747272e1bb64c710.jpg",
                            playlist_name: "Playlist_Blitz_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_In-Game_LTM_Blitz_Masked_ModeTile-1024x512-ca1788d0b956e789da358481747272e1bb64c710.jpg",
                            playlist_name: "Playlist_Blitz_Squad",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Close_EncountersMOTD_1024x512-1024x512-b8ae46224a1de0d4db833c23755f4afabffb34b9.jpg",
                            playlist_name: "Playlist_Close_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Close_EncountersMOTD_1024x512-1024x512-b8ae46224a1de0d4db833c23755f4afabffb34b9.jpg",
                            playlist_name: "Playlist_Close_Squads",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Close_EncountersMOTD_1024x512-1024x512-b8ae46224a1de0d4db833c23755f4afabffb34b9.jpg",
                            playlist_name: "Playlist_Close_Trios",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Close_EncountersMOTD_1024x512-1024x512-b8ae46224a1de0d4db833c23755f4afabffb34b9.jpg",
                            playlist_name: "Playlist_Close_Duos",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlisttiles/BR04_LTM_SoloShowdown-1024x512-0f522b0881adebfe241c6527f03c9140f70b88a7.png",
                            playlist_name: "Playlist_Comp_Solo",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/v94/11BR_Solid_Gold_Playlist_Tile_1025x512-1024x512-ea1253854e55ed01f3d6e012ecfa373cc1b38d37.jpg",
                            playlist_name: "Playlist_SolidGold_Solo",
                            violator: "",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlisttiles/LTM_50v50-1024x512-788bf1a67426f54307c4296123ac2d3ff8cc0d6c.png",
                            playlist_name: "Playlist_50v50",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/Fortnite/fortnite-game/playlistinformation/BR04_LTM_Gauntlet-1024x512-a0dca279534a121957275bcdb0b42042d8a010fd.png",
                            playlist_name: "Playlist_Carmine",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://wallpaperaccess.com/full/1595934.jpg",
                            playlist_name: "Playlist_DefaultSolo",
                            hidden: false,
                            violator: "Updated for ZeroFN",
                            special_border: "None",
                            _type: "FortPlaylistInfo",
                            description: "Go at it alone in a battle to be the last one standing.\n\n",
                            display_name: "Solo - ZeroFN"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-fortnitemares-solo-1024x512-326832464.jpg",
                            playlist_name: "Playlist_Arseniccore_Solo_Maxfog",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://th.bing.com/th/id/R.e6052802318d5d47224be254a6f4aff9?rik=Tc3HsX%2fssPSsAA&pid=ImgRaw&r=0",
                            playlist_name: "Playlist_DefaultDuo",
                            hidden: false,
                            violator: "Broken as fuck",
                            special_border: "None",
                            _type: "FortPlaylistInfo",
                            description: "If you are on one of the following versions you can play live events: v7.20, v7.30, v8.51, v9.40, v10.40, v12.41, v12.61, v14.60, v17.50, v18.40, v20.40! If your build isn't listed you will be in normal match without livevents!\n\n",
                            display_name: "[ALPHA] Playlist_Event"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-fortnitemares-duos-1024x512-326842571.jpg",
                            playlist_name: "Playlist_Arseniccore_Duos_Maxfog",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/21br-launch-modetiles-trios-1920x1080-1920x1080-45daa6910446.jpg",
                            playlist_name: "playlist_trios",
                            hidden: false,
                            _type: "FortPlaylistInfo",
                            description: "Classic Battle Royale with Three-Person Squads.\n\n",
                            display_name: "Trios"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-fortnitemares-trios-1024x512-326846465.jpg",
                            playlist_name: "playlist_arseniccore_trios_maxfog",
                            hidden: false,
                            _type: "FortPlaylistInfo"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/21br-launch-modetiles-squads-1920x1080-1920x1080-a187ec1717d7.jpg",
                            playlist_name: "Playlist_DefaultSquad",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo",
                            description: "Group up and outlast all of the other squads gunning for victory.\n\n",
                            display_name: "Squads"
                        },
                        {
                            image: "https://cdn2.unrealengine.com/14br-fortnitemares-squad-1024x512-326850231.jpg",
                            playlist_name: "playlist_arseniccore_squads_maxfog",
                            hidden: false,
                            special_border: "None",
                            _type: "FortPlaylistInfo"
                        }
                    ]
                },
                "_noIndex": false,
                "jcr:baseVersion": "a7ca237317f1e7e358ae2f-0f54-4e20-8ec7-267b4668393c",
                "_activeDate": "2018-04-25T15:05:39.956Z",
                "lastModified": "2020-10-16T11:57:44.884Z",
                "_locale": "en-US"
            },
            "dynamicbackgrounds": {
                "jcr:isCheckedOut": true,
                "backgrounds": {
                    "backgrounds": [{   
                        "backgroundimage": "https://cdn2.unrealengine.com/t-bp17-30-lobbyss-2048x1024-cefd64cd2ff2.png",
				"stage": "defaultnotris",
                        "_type": "DynamicBackground",
                        "key": "lobby"
                    },
                    {
                        "stage": "Buffet",
                        "_type": "DynamicBackground",
                        "key": "vault"
                    }],
                    "_type": "DynamicBackgroundList"
                },
                "_title": "dynamicbackgrounds",
                "_noIndex": false,
                "jcr:baseVersion": "a7ca237317f1e7bac3355e-3ec1-4054-baf5-f88613799961",
                "_activeDate": "2019-08-21T15:59:59.342Z",
                "lastModified": "2020-08-27T08:04:47.051Z",
                "_locale": "en-US"
            },
            "shopSections": {
                "jcr:isCheckedOut": true,
                "_title": "shop-sections",
                "sectionList": {
                    "_type": "ShopSectionList",
                    "sections": [{
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 70,
                        "sectionId": "Featured",
                        "bShowTimer": true,
                        "sectionDisplayName": "Featured",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 69,
                        "sectionId": "Featured2",
                        "bShowTimer": false,
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 65,
                        "sectionId": "CustomizeHero",
                        "bShowTimer": true,
                        "sectionDisplayName": "Customize Your Hero!",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 64,
                        "sectionId": "HeroGear",
                        "bShowTimer": true,
                        "sectionDisplayName": "Hero Gear",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": false,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 65,
                        "sectionId": "BannerBrigade",
                        "bShowTimer": true,
                        "sectionDisplayName": "Banner Brigade",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 64,
                        "sectionId": "BannerGear",
                        "bShowTimer": true,
                        "sectionDisplayName": "Banner Gear",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 60,
                        "sectionId": "Daily",
                        "bShowTimer": true,
                        "sectionDisplayName": "Daily",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 59,
                        "sectionId": "Daily2",
                        "bShowTimer": true,
                        "sectionDisplayName": "",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 60,
                        "sectionId": "Special",
                        "bShowTimer": true,
                        "sectionDisplayName": "Special Offers",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 59,
                        "sectionId": "Special2",
                        "bShowTimer": true,
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 65,
                        "sectionId": "StarWars",
                        "bShowTimer": true,
                        "sectionDisplayName": "Star Wars",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 65,
                        "sectionId": "StartParty",
                        "bShowTimer": true,
                        "sectionDisplayName": "Start the Party!",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 64,
                        "sectionId": "PartyGear",
                        "bShowTimer": true,
                        "sectionDisplayName": "Party Gear",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 64,
                        "sectionId": "TurnMusicUp",
                        "bShowTimer": true,
                        "sectionDisplayName": "Turn the Music Up!",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 30,
                        "sectionId": "SpookyOffers",
                        "bShowTimer": true,
                        "sectionDisplayName": "Spooky Offers",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 29,
                        "sectionId": "SpookyOffers2",
                        "bShowTimer": true,
                        "sectionDisplayName": "",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 65,
                        "sectionId": "Fortnitemares",
                        "bShowTimer": true,
                        "sectionDisplayName": "Fortnitemares",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 65,
                        "sectionId": "Ghostbusters",
                        "bShowTimer": true,
                        "sectionDisplayName": "Ghostbusters",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 64,
                        "sectionId": "GhostbustersGear",
                        "bShowTimer": true,
                        "sectionDisplayName": "Ghostbusters Gear",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 40,
                        "sectionId": "Marvel",
                        "bShowTimer": true,
                        "sectionDisplayName": "Marvel",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 39,
                        "sectionId": "Marvel2",
                        "bShowTimer": true,
                        "sectionDisplayName": "",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 38,
                        "sectionId": "Marvel3",
                        "bShowTimer": true,
                        "sectionDisplayName": "",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "_type": "DynamicBackground"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 37,
                        "sectionId": "Marvel4",
                        "bShowTimer": true,
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "sectionId": "Bundles",
                        "bShowTimer": true,
                        "sectionDisplayName": "Bundles",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "sectionId": "Characters",
                        "bShowTimer": true,
                        "sectionDisplayName": "Outfits",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "sectionId": "Backpacks",
                        "bShowTimer": true,
                        "sectionDisplayName": "Back Blings",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 0,
                        "sectionId": "Pickaxes",
                        "bShowTimer": true,
                        "sectionDisplayName": "Harvesting Tools",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "sectionId": "Gliders",
                        "bShowTimer": true,
                        "sectionDisplayName": "Gliders",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "sectionId": "MusicPacks",
                        "bShowTimer": true,
                        "sectionDisplayName": "Music Packs",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "sectionId": "Wraps",
                        "bShowTimer": true,
                        "sectionDisplayName": "Wraps",
                        "bShowIneligibleOffers": true
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": true,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 80,
                        "sectionId": "LimitedTime",
                        "bShowTimer": false,
                        "sectionDisplayName": "Limited Time",
                        "bShowIneligibleOffers": false
                    }, {
                        "bSortOffersByOwnership": false,
                        "bShowIneligibleOffersIfGiftable": false,
                        "bEnableToastNotification": false,
                        "background": {
                            "stage": "default",
                            "_type": "DynamicBackground",
                            "key": "vault"
                        },
                        "_type": "ShopSection",
                        "landingPriority": 90,
                        "sectionId": "Battlepass",
                        "bShowTimer": false,
                        "sectionDisplayName": "Battle Pass",
                        "bShowIneligibleOffers": true
                    }]
                },
                "_noIndex": false,
                "jcr:baseVersion": "a7ca237317f1e77f7abaac-2b4d-4df8-828b-6056a1904470",
                "_activeDate": "2020-10-28T00:00:00.000Z",
                "lastModified": "2020-11-02T15:56:44.648Z",
                "_locale": "en-US"
            },
            "battleroyalenewsv2": {
                "news": {
                    "motds": [{
                        "entryType": "Website",
                        "image": `https://th.bing.com/th/id/R.b9283741234b7dd744be042555c851ca?rik=s%2badEyQJELx6Wg&pid=ImgRaw&r=0`,
                        "tileImage": `https://th.bing.com/th/id/R.b9283741234b7dd744be042555c851ca?rik=s%2badEyQJELx6Wg&pid=ImgRaw&r=0`,
                        "videoMute": false,
                        "hidden": false,
                        "tabTitleOverride": "ZeroFN Credits",
                        "_type": "CommonUI Simple Message MOTD",
                        "title": "ZeroFN Credits",
                        "body": "Credits:\r\nBackend: Neonite\r\nModified Backend: TFN\r\nIdea and Owner: Pokelevel\r\nCarbon: Neonite",
                        "videoLoop": false,
                        "videoStreamingEnabled": false,
                        "sortingPriority": 3,
                        "id": "ZFNCreditNewsBR",
                        "videoAutoplay": false,
                        "videoFullscreen": false,
                        "spotlight": false,
                        "websiteURL": "https://discord.gg/Nx5SW25kNT",
                        "websiteButtonText": "Join our discord"
                    },
		    {
                        "entryType": "Text",
                        "image": `https://th.bing.com/th/id/R.e6052802318d5d47224be254a6f4aff9?rik=Tc3HsX%2fssPSsAA&pid=ImgRaw&r=0`,
                        "tileImage": `https://th.bing.com/th/id/R.e6052802318d5d47224be254a6f4aff9?rik=Tc3HsX%2fssPSsAA&pid=ImgRaw&r=0`,
                        "videoMute": false,
                        "hidden": false,
                        "tabTitleOverride": "Event Chapter 1",
                        "_type": "CommonUI Simple Message MOTD",
                        "title": "Events Ch1",
                        "body": "V7.20: Ice King\r\nV7.30: Marshmello\r\nV8.51: Unvaulting\r\nV9.40: The Final Showdown\r\nV10.40: The End",
                        "videoLoop": false,
                        "videoStreamingEnabled": false,
                        "sortingPriority": 2,
                        "id": "ZFNCh1NewsBR",
                        "videoAutoplay": false,
                        "videoFullscreen": false,
                        "spotlight": false,
                    },
		    {
                        "entryType": "Text",
                        "image": `https://hdwallsource.com/img/2020/2/fortnite-the-device-event-wallpaper-71327-73778-hd-wallpapers.jpg`,
                        "tileImage": `https://hdwallsource.com/img/2020/2/fortnite-the-device-event-wallpaper-71327-73778-hd-wallpapers.jpg`,
                        "videoMute": false,
                        "hidden": false,
                        "tabTitleOverride": "Event Chapter 2",
                        "_type": "CommonUI Simple Message MOTD",
                        "title": "Events Ch2",
                        "body": "V12.41: Travis Scott\r\nV12.61: The Device\r\nV14.60: The Devourer of Worlds\r\nV17.50: Operation: Sky Fire\r\nV18.40: The End (Chapter 2 Finale)",
                        "videoLoop": false,
                        "videoStreamingEnabled": false,
                        "sortingPriority": 1,
                        "id": "ZFNCh2NewsBR",
                        "videoAutoplay": false,
                        "videoFullscreen": false,
                        "spotlight": false,
                    },
		    {
                        "entryType": "Text",
                        "image": `https://www.jaxon.gg/wp-content/uploads/2022/05/fortnite-mech-vs-monster-2-1.jpg`,
                        "tileImage": `https://www.jaxon.gg/wp-content/uploads/2022/05/fortnite-mech-vs-monster-2-1.jpg`,
                        "videoMute": false,
                        "hidden": false,
                        "tabTitleOverride": "Event Chapter 3",
                        "_type": "CommonUI Simple Message MOTD",
                        "title": "Events Ch3",
                        "body": "V20.40: Collision\r\nV20.40: Collider Pulse",
                        "videoLoop": false,
                        "videoStreamingEnabled": false,
                        "sortingPriority": 0,
                        "id": "ZFNCh3NewsBR",
                        "videoAutoplay": false,
                        "videoFullscreen": false,
                        "spotlight": false,
			}
                ],
                },
                "jcr:isCheckedOut": true,
                "_title": "battleroyalenewsv2",
                "header": "",
                "style": "None",
                "_noIndex": false,
                "alwaysShow": false,
                "jcr:baseVersion": "a7ca237317f1e704b1a186-6846-4eaa-a542-c2c8ca7e7f29",
                "_activeDate": "2020-01-21T14:00:00.000Z",
                "lastModified": "2020-11-01T17:36:19.024Z",
                "_locale": "en-US"
            },
            "Creativenewsv2": {
                "news": {
                    "motds": [{
                        "entryType": "Website",
                        "image": `https://th.bing.com/th/id/R.b9283741234b7dd744be042555c851ca?rik=s%2badEyQJELx6Wg&pid=ImgRaw&r=0`,
                        "tileImage": `https://th.bing.com/th/id/R.b9283741234b7dd744be042555c851ca?rik=s%2badEyQJELx6Wg&pid=ImgRaw&r=0`,
                        "videoMute": false,
                        "hidden": false,
                        "tabTitleOverride": "ZeroFN Credits",
                        "_type": "CommonUI Simple Message MOTD",
                        "title": "ZeroFN Credits",
                        "body": "Credits:\r\nBackend: Neonite\r\nModified Backend: TFN\r\nIdea and Owner: Pokelevel\r\nCarbon: Neonite",
                        "videoLoop": false,
                        "videoStreamingEnabled": false,
                        "sortingPriority": 3,
                        "id": "ZFNCreditNewsBR",
                        "videoAutoplay": false,
                        "videoFullscreen": false,
                        "spotlight": false,
                        "websiteURL": "https://discord.gg/Nx5SW25kNT",
                        "websiteButtonText": "Join our discord"
                    },
		    {
                        "entryType": "Text",
                        "image": `https://th.bing.com/th/id/R.e6052802318d5d47224be254a6f4aff9?rik=Tc3HsX%2fssPSsAA&pid=ImgRaw&r=0`,
                        "tileImage": `https://th.bing.com/th/id/R.e6052802318d5d47224be254a6f4aff9?rik=Tc3HsX%2fssPSsAA&pid=ImgRaw&r=0`,
                        "videoMute": false,
                        "hidden": false,
                        "tabTitleOverride": "Event Chapter 1",
                        "_type": "CommonUI Simple Message MOTD",
                        "title": "Events Ch1",
                        "body": "V7.20: Ice King\r\nV7.30: Marshmello\r\nV8.51: Unvaulting\r\nV9.40: The Final Showdown\r\nV10.40: The End",
                        "videoLoop": false,
                        "videoStreamingEnabled": false,
                        "sortingPriority": 2,
                        "id": "ZFNCh1NewsBR",
                        "videoAutoplay": false,
                        "videoFullscreen": false,
                        "spotlight": false,
                    },
		    {
                        "entryType": "Text",
                        "image": `https://hdwallsource.com/img/2020/2/fortnite-the-device-event-wallpaper-71327-73778-hd-wallpapers.jpg`,
                        "tileImage": `https://hdwallsource.com/img/2020/2/fortnite-the-device-event-wallpaper-71327-73778-hd-wallpapers.jpg`,
                        "videoMute": false,
                        "hidden": false,
                        "tabTitleOverride": "Event Chapter 2",
                        "_type": "CommonUI Simple Message MOTD",
                        "title": "Events Ch2",
                        "body": "V12.41: Travis Scott\r\nV12.61: The Device\r\nV14.60: The Devourer of Worlds\r\nV17.50: Operation: Sky Fire\r\nV18.40: The End (Chapter 2 Finale)",
                        "videoLoop": false,
                        "videoStreamingEnabled": false,
                        "sortingPriority": 1,
                        "id": "ZFNCh2NewsBR",
                        "videoAutoplay": false,
                        "videoFullscreen": false,
                        "spotlight": false,
                    },
		    {
                        "entryType": "Text",
                        "image": `https://www.jaxon.gg/wp-content/uploads/2022/05/fortnite-mech-vs-monster-2-1.jpg`,
                        "tileImage": `https://www.jaxon.gg/wp-content/uploads/2022/05/fortnite-mech-vs-monster-2-1.jpg`,
                        "videoMute": false,
                        "hidden": false,
                        "tabTitleOverride": "Event Chapter 3",
                        "_type": "CommonUI Simple Message MOTD",
                        "title": "Events Ch3",
                        "body": "V20.40: Collision\r\nV20.40: Collider Pulse",
                        "videoLoop": false,
                        "videoStreamingEnabled": false,
                        "sortingPriority": 0,
                        "id": "ZFNCh3NewsBR",
                        "videoAutoplay": false,
                        "videoFullscreen": false,
                        "spotlight": false,
			}
                ],
                },
                "jcr:isCheckedOut": true,
                "_title": "Creativenewsv2",
                "header": "",
                "style": "None",
                "_noIndex": false,
                "alwaysShow": false,
                "jcr:baseVersion": "a7ca237317f1e704b1a186-6846-4eaa-a542-c2c8ca7e7f29",
                "_activeDate": "2020-01-21T14:00:00.000Z",
                "lastModified": "2020-11-01T17:36:19.024Z",
                "_locale": "en-US"
            },
            "shopCarousel": {
                "jcr:isCheckedOut": true,
                "itemsList": {
                    "_type": "ShopCarouselItemList",
                    "items": [{
                        "tileImage": Userconfig.news[0].title,
                        "fullTitle": Userconfig.news[0].title,
                        "hidden": false,
                        "_type": "ShopCarouselItem",
                        "landingPriority": 100,
                        "action": "ShowOfferDetails",
                        "offerId": "v2:/cc29bec6d0bdd2876fb412cf674a7960003bbb76b2eb9b930833a61b604a3b4e",
                        "title": Userconfig.news[0].title
                    }]
                },
                "_title": "shop-carousel",
                "_noIndex": false,
                "jcr:baseVersion": "a7ca237317f1e76d4c1405-e84c-4ece-bc50-7112ba557b33",
                "_activeDate": "2020-09-25T12:00:00.000Z",
                "lastModified": "2020-10-26T15:37:05.775Z",
                "_locale": "en-US"
            },
            "specialoffervideo": {
                "jcr:isCheckedOut": true,
                "specialoffervideo": {
                    "videoString": Userconfig.videoString, // Literally only this makes the entire thing works (for now I suppose), can be random too.
                    "videoUID": Userconfig.videoUID,
                    "_type": "SpecialOfferVideoConfig",
                    "bStreamingEnabled": true,
                    "bCheckAutoPlay": true
                },
                "_title": "specialoffervideo",
                "_noIndex": false,
                "jcr:baseVersion": "a7ca237317f1e7994a617c-6fd5-4f16-bff1-e4709ead09ad",
                "bSpecialOfferEnabled": true,
                "_activeDate": new Date(),
                "lastModified": new Date(),
                "_locale": "en-US"
            },
            "_suggestedPrefetch": []
        })
    })
}
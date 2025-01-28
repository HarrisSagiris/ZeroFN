#define _WIN32_WINNT 0x0602 // Target Windows 8 or later

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <chrono>
#include <experimental/filesystem>
#include <direct.h>
#include <map>
#include <mutex>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <TlHelp32.h>
#include <Psapi.h>
#include <processthreadsapi.h>
#include <winternl.h>

// ZeroFN Version 1.2.4 
// Developed by DevHarris
// A private server implementation for Fortnite Season 2 Chapter 1
// Added improved auth bypass and crash prevention

namespace fs = std::experimental::filesystem;

struct GameSession {
    std::string sessionId;
    std::vector<std::string> players;
    bool inProgress;
    std::string playlistId;
    std::map<std::string, std::string> playerLoadouts;
};

class FortniteServer {
private:
    SOCKET serverSocket;
    bool running;
    const int PORT = 7777;
    std::string authToken;
    std::string displayName;
    std::string accountId;
    std::string installPath;
    HANDLE gameProcess;
    HANDLE patcherProcess;
    
    std::map<std::string, GameSession> activeSessions;
    std::vector<std::string> matchmakingQueue;
    std::mutex matchmakingMutex;
    
    std::map<std::string, std::string> playerLoadout;
    std::map<std::string, std::map<std::string, std::string>> cosmeticsDb;

    void loadCosmeticsDatabase() {
        std::ifstream cosmeticsFile("cosmetics.json");
        if(cosmeticsFile.good()) {
            std::string line;
            while(std::getline(cosmeticsFile, line)) {
                // Simple parsing of cosmetics file
                // Format: category,id,name
                std::istringstream iss(line);
                std::string category, id, name;
                std::getline(iss, category, ',');
                std::getline(iss, id, ',');
                std::getline(iss, name);
                cosmeticsDb[category][id] = name;
            }
        } else {
            // Initialize Season 2 Chapter 1 cosmetics database
            cosmeticsDb["characters"]["CID_001_Athena_Commando_F_Default"] = "Default";
            cosmeticsDb["characters"]["CID_002_Athena_Commando_F_Default"] = "Ramirez";
            cosmeticsDb["characters"]["CID_003_Athena_Commando_F_Default"] = "Banshee";
            cosmeticsDb["characters"]["CID_004_Athena_Commando_F_Default"] = "Black Knight";
            cosmeticsDb["characters"]["CID_005_Athena_Commando_F_Default"] = "Blue Squire";
            
            cosmeticsDb["backpacks"]["BID_001_BlackKnight"] = "Black Shield";
            cosmeticsDb["backpacks"]["BID_002_BlueSquire"] = "Blue Shield";
            cosmeticsDb["backpacks"]["BID_003_RedKnight"] = "Red Shield";
            
            cosmeticsDb["pickaxes"]["Pickaxe_Default"] = "Default Pickaxe";
            cosmeticsDb["pickaxes"]["Pickaxe_ID_001"] = "AC/DC";
            cosmeticsDb["pickaxes"]["Pickaxe_ID_002"] = "Raider's Revenge";
            
            cosmeticsDb["gliders"]["Glider_Default"] = "Default Glider";
            cosmeticsDb["gliders"]["Glider_ID_001"] = "Mako";
            cosmeticsDb["gliders"]["Glider_ID_002"] = "Snowflake";
            
            cosmeticsDb["emotes"]["EID_DanceDefault"] = "Dance Moves";
            cosmeticsDb["emotes"]["EID_Floss"] = "Floss";
            cosmeticsDb["emotes"]["EID_TakeTheL"] = "Take The L";
            cosmeticsDb["emotes"]["EID_Dab"] = "Dab";
            cosmeticsDb["emotes"]["EID_Wave"] = "Wave";
            cosmeticsDb["emotes"]["EID_RideThePony"] = "Ride The Pony";

            std::ofstream out("cosmetics.json");
            for(const auto& category : cosmeticsDb) {
                for(const auto& item : category.second) {
                    out << category.first << "," << item.first << "," << item.second << "\n";
                }
            }
        }
    }

    void initializePlayerData() {
        loadCosmeticsDatabase();
        
        playerLoadout["character"] = "CID_004_Athena_Commando_F_Default"; // Black Knight
        playerLoadout["backpack"] = "BID_001_BlackKnight"; // Black Shield
        playerLoadout["pickaxe"] = "Pickaxe_ID_001"; // AC/DC
        playerLoadout["glider"] = "Glider_ID_002"; // Snowflake
        playerLoadout["contrail"] = "Trails_Default";
        playerLoadout["emote1"] = "EID_Floss";
        playerLoadout["emote2"] = "EID_TakeTheL";
        playerLoadout["emote3"] = "EID_RideThePony";
        playerLoadout["emote4"] = "EID_Wave";
    }

    void sendResponse(SOCKET clientSocket, const std::string& response) {
        if (send(clientSocket, response.c_str(), response.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send response: " << WSAGetLastError() << std::endl;
        }
    }

    std::string parseRequest(const std::string& request) {
        std::istringstream iss(request);
        std::string requestLine;
        std::getline(iss, requestLine);
        
        size_t pos1 = requestLine.find(' ');
        size_t pos2 = requestLine.find(' ', pos1 + 1);
        
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            return requestLine.substr(pos1 + 1, pos2 - pos1 - 1);
        }
        return "";
    }

    std::map<std::string, std::string> parseRequestBody(const std::string& request) {
        std::map<std::string, std::string> result;
        size_t bodyStart = request.find("\r\n\r\n");
        if(bodyStart != std::string::npos) {
            std::string body = request.substr(bodyStart + 4);
            std::istringstream iss(body);
            std::string line;
            while(std::getline(iss, line)) {
                size_t pos = line.find(':');
                if(pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    result[key] = value;
                }
            }
        }
        return result;
    }

    std::string generateMatchId() {
        return "MATCH_" + std::to_string(rand());
    }

    void handleMatchmaking() {
        while(running) {
            {
                std::lock_guard<std::mutex> lock(matchmakingMutex);
                
                if(matchmakingQueue.size() >= 2) {
                    GameSession session;
                    session.sessionId = generateMatchId();
                    session.inProgress = true;
                    session.playlistId = "Playlist_DefaultSolo";
                    
                    const size_t maxPlayers = 16;
                    while(!matchmakingQueue.empty() && session.players.size() < maxPlayers) {
                        std::string playerId = matchmakingQueue.back();
                        session.players.push_back(playerId);
                        session.playerLoadouts[playerId] = playerLoadout["character"];
                        matchmakingQueue.pop_back();
                    }
                    
                    activeSessions[session.sessionId] = session;
                    
                    std::cout << "[MATCHMAKING] Created session " << session.sessionId 
                             << " with " << session.players.size() << " players" << std::endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void handleClient(SOCKET clientSocket) {
        const int bufferSize = 16384;
        std::vector<char> buffer(bufferSize);
        
        int bytesReceived = recv(clientSocket, buffer.data(), bufferSize, 0);
        if (bytesReceived > 0) {
            std::string request(buffer.data(), bytesReceived);
            std::string endpoint = parseRequest(request);
            auto requestBody = parseRequestBody(request);

            std::string headers = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: application/json\r\n"
                                "Access-Control-Allow-Origin: *\r\n"
                                "Connection: keep-alive\r\n\r\n";

            if (endpoint == "/account/api/oauth/token") {
                std::string response = "{\"access_token\":\"" + authToken + "\","
                    "\"expires_in\":999999999,"  // Extended token expiry
                    "\"expires_at\":\"9999-12-31T23:59:59.999Z\","
                    "\"token_type\":\"bearer\","
                    "\"refresh_token\":\"" + authToken + "\","
                    "\"refresh_expires\":999999999,"  // Extended refresh expiry
                    "\"refresh_expires_at\":\"9999-12-31T23:59:59.999Z\","
                    "\"account_id\":\"" + accountId + "\","
                    "\"client_id\":\"ec684b8c687f479fadea3cb2ad83f5c6\","
                    "\"internal_client\":true,"
                    "\"client_service\":\"fortnite\","
                    "\"displayName\":\"" + displayName + "\","
                    "\"app\":\"fortnite\","
                    "\"in_app_id\":\"" + accountId + "\","
                    "\"device_id\":\"" + accountId + "\","  // Added device ID
                    "\"secret\":\"secret_token\"}";  // Added secret token
                
                sendResponse(clientSocket, headers + response);
            }
            else if (endpoint == "/account/api/public/account") {
                std::string response = "{\"id\":\"" + accountId + "\","
                    "\"displayName\":\"" + displayName + "\","
                    "\"email\":\"user@zerofn.local\","  // Added email
                    "\"failedLoginAttempts\":0,"  // Added login attempts
                    "\"lastLogin\":\"2023-12-31T23:59:59.999Z\","  // Added last login
                    "\"numberOfDisplayNameChanges\":0,"  // Added display name changes
                    "\"ageGroup\":\"ADULT\","  // Added age group
                    "\"headless\":false,"  // Added headless flag
                    "\"country\":\"US\","  // Added country
                    "\"lastName\":\"User\","  // Added last name
                    "\"preferredLanguage\":\"en\","  // Added language
                    "\"canUpdateDisplayName\":true,"  // Added display name update permission
                    "\"tfaEnabled\":false,"  // Added 2FA status
                    "\"emailVerified\":true,"  // Added email verification
                    "\"minorVerified\":false,"  // Added minor verification
                    "\"minorExpected\":false,"  // Added minor expected
                    "\"minorStatus\":\"NOT_MINOR\","  // Added minor status
                    "\"cabinedMode\":false,"  // Added cabined mode
                    "\"hasHashedEmail\":false,"  // Added hashed email flag
                    "\"externalAuths\":{}}";  // Kept external auths empty
                
                sendResponse(clientSocket, headers + response);
            }
            else if (endpoint.find("/fortnite/api/game/v2/profile/" + accountId + "/client") != std::string::npos) {
                std::string response = "{\"profileId\":\"athena\","
                    "\"profileChanges\":[{\"_type\":\"fullProfileUpdate\","
                    "\"profile\":{\"_id\":\"" + accountId + "\","
                    "\"accountId\":\"" + accountId + "\","
                    "\"profileId\":\"athena\","
                    "\"version\":\"1\","
                    "\"items\":{";

                // Add cosmetics
                bool first = true;
                for(const auto& category : cosmeticsDb) {
                    for(const auto& item : category.second) {
                        if(!first) response += ",";
                        first = false;
                        response += "\"" + item.first + "\":{\"name\":\"" + item.second + "\"}";
                    }
                }

                response += "},\"stats\":{\"attributes\":{\"season_num\":2,"
                    "\"accountLevel\":100,"  // Added account level
                    "\"level\":100,"  // Added level
                    "\"xp\":999999,"  // Added XP
                    "\"season_match_boost\":999,"  // Added match boost
                    "\"loadout\":{";

                // Add loadout
                first = true;
                for(const auto& item : playerLoadout) {
                    if(!first) response += ",";
                    first = false;
                    response += "\"" + item.first + "\":\"" + item.second + "\"";
                }

                response += "}}},\"commandRevision\":1}}],"
                    "\"profileCommandRevision\":1,"
                    "\"serverTime\":\"2023-12-31T23:59:59.999Z\","
                    "\"responseVersion\":1}";

                if(endpoint.find("SetCosmeticLockerSlot") != std::string::npos) {
                    std::string category = requestBody["category"];
                    std::string itemId = requestBody["itemId"];
                    
                    if(cosmeticsDb[category].find(itemId) != cosmeticsDb[category].end()) {
                        playerLoadout[category] = itemId;
                        std::cout << "[COSMETICS] Player changed " << category << " to " << itemId << std::endl;
                    }
                }
                
                sendResponse(clientSocket, headers + response);
            }
            else if (endpoint.find("/fortnite/api/matchmaking/session/findPlayer/") != std::string::npos) {
                std::lock_guard<std::mutex> lock(matchmakingMutex);
                matchmakingQueue.push_back(accountId);
                
                std::string response = "{\"status\":\"waiting\","
                    "\"priority\":0,"
                    "\"ticket\":\"ticket_" + std::to_string(rand()) + "\","
                    "\"queuedPlayers\":" + std::to_string(matchmakingQueue.size()) + "}";
                
                sendResponse(clientSocket, headers + response);
            }
            else if (endpoint.find("/fortnite/api/matchmaking/session/matchMakingRequest") != std::string::npos) {
                std::string response;
                std::lock_guard<std::mutex> lock(matchmakingMutex);
                
                bool found = false;
                for(const auto& session : activeSessions) {
                    auto it = std::find(session.second.players.begin(), 
                                      session.second.players.end(), 
                                      accountId);
                    if(it != session.second.players.end()) {
                        response = "{\"status\":\"found\","
                            "\"matchId\":\"" + session.first + "\","
                            "\"sessionId\":\"" + session.first + "\","
                            "\"playlistId\":\"" + session.second.playlistId + "\"}";
                        found = true;
                        break;
                    }
                }
                
                if(!found) {
                    response = "{\"status\":\"waiting\","
                        "\"estimatedWaitSeconds\":5}";
                }
                
                sendResponse(clientSocket, headers + response);
            }
            else {
                // Default response for any unhandled endpoints
                std::string response = "{\"status\":\"ok\",\"errorCode\":\"errors.com.epicgames.common.not_found\",\"errorMessage\":\"Sorry the resource you were trying to find could not be found\",\"messageVars\":[],\"numericErrorCode\":1004,\"originatingService\":\"fortnite\",\"intent\":\"prod\"}";
                sendResponse(clientSocket, headers + response);
            }
        }

        closesocket(clientSocket);
    }

public:
    bool LivePatchFortnite() {
        std::cout << "\n[LIVE PATCHER] Starting rapid patching process...\n";

        // Wait for process to be fully initialized
        Sleep(3000);

        // Get Fortnite process ID
        DWORD processId = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W processEntry;
            processEntry.dwSize = sizeof(processEntry);
            
            if (Process32FirstW(snapshot, &processEntry)) {
                do {
                    if (_wcsicmp(processEntry.szExeFile, L"FortniteClient-Win64-Shipping.exe") == 0) {
                        processId = processEntry.th32ProcessID;
                        break;
                    }
                } while (Process32NextW(snapshot, &processEntry));
            }
            CloseHandle(snapshot);
        }

        if (processId == 0) {
            std::cout << "[LIVE PATCHER] Fortnite process not found\n";
            return false;
        }

        // Open process with required access rights
        HANDLE processHandle = OpenProcess(
            PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION,
            FALSE,
            processId
        );

        if (!processHandle) {
            std::cout << "[LIVE PATCHER] Failed to open process\n";
            return false;
        }

        // Wait for modules to load
        Sleep(2000);

        // Get module information
        HMODULE moduleHandle = NULL;
        DWORD cbNeeded;
        if (!EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &cbNeeded)) {
            CloseHandle(processHandle);
            return false;
        }

        MODULEINFO moduleInfo;
        if (!GetModuleInformation(processHandle, moduleHandle, &moduleInfo, sizeof(moduleInfo))) {
            CloseHandle(processHandle);
            return false;
        }

        // Critical patches for Season 2
        std::vector<std::pair<std::vector<BYTE>, std::vector<BYTE>>> patches = {
            // Auth bypass
            {{0x75, 0x04, 0x33, 0xC0, 0x5D, 0xC3}, {0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3}},
            {{0x74, 0x23, 0x8B, 0x46, 0x10}, {0x90, 0x90, 0x8B, 0x46, 0x10}},
            
            // SSL pinning bypass
            {{0x0F, 0x84, 0x85, 0x00, 0x00, 0x00}, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90}},
            
            // Server validation bypass
            {{0x75, 0x1D, 0x8B, 0x45, 0x08}, {0x90, 0x90, 0x8B, 0x45, 0x08}},
            
            // Season check bypass
            {{0x83, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x02}, {0xB8, 0x02, 0x00, 0x00, 0x00, 0x90, 0x90}}
        };

        int patchesApplied = 0;
        const size_t bufferSize = 0x1000;
        std::vector<BYTE> buffer(bufferSize);
        
        // Scan and patch in smaller chunks for speed
        for (SIZE_T offset = 0; offset < moduleInfo.SizeOfImage; offset += bufferSize) {
            SIZE_T bytesRead;
            if (ReadProcessMemory(processHandle, (LPCVOID)((DWORD_PTR)moduleInfo.lpBaseOfDll + offset),
                buffer.data(), bufferSize, &bytesRead)) {
                
                for (const auto& patch : patches) {
                    for (size_t i = 0; i < bytesRead - patch.first.size(); i++) {
                        if (memcmp(buffer.data() + i, patch.first.data(), patch.first.size()) == 0) {
                            LPVOID patchAddr = (LPVOID)((DWORD_PTR)moduleInfo.lpBaseOfDll + offset + i);
                            DWORD oldProtect;
                            
                            if (VirtualProtectEx(processHandle, patchAddr, patch.second.size(), 
                                PAGE_EXECUTE_READWRITE, &oldProtect)) {
                                
                                if (WriteProcessMemory(processHandle, patchAddr, patch.second.data(),
                                    patch.second.size(), NULL)) {
                                    
                                    VirtualProtectEx(processHandle, patchAddr, patch.second.size(),
                                        oldProtect, &oldProtect);
                                    
                                    patchesApplied++;
                                    std::cout << "[LIVE PATCHER] Applied patch " << patchesApplied << "\n";
                                }
                            }
                        }
                    }
                }
            }
        }

        CloseHandle(processHandle);

        if (patchesApplied > 0) {
            std::cout << "[LIVE PATCHER] Successfully applied " << patchesApplied << " patches\n";
            return true;
        }

        std::cout << "[LIVE PATCHER] No patches were applied\n";
        return false;
    }

private:
    void startPatcher() {
        std::cout << "[PATCHER] Starting optimized patch monitoring...\n";
        std::thread([this]() {
            while (running) {
                LivePatchFortnite();
                Sleep(60000); // Check every minute
            }
        }).detach();
    }

public:
    FortniteServer() : running(false), serverSocket(INVALID_SOCKET), gameProcess(NULL), patcherProcess(NULL) {
        srand(static_cast<unsigned>(time(0)));
        
        system("cls");
        std::cout << "\nZeroFN Version 1.2.4 - Season 2 Chapter 1\n";
        std::cout << "Developed by @Devharris\n\n";
        
        std::cout << "Enter your desired in-game username: ";
        std::getline(std::cin, displayName);
        if(displayName.empty()) {
            displayName = "ZeroFN_User";
        }
        
        accountId = "zerofn_" + std::to_string(rand());
        authToken = "zerofn_token_" + std::to_string(rand());
        
        initializePlayerData();
        loadOrSetupInstallPath();
    }

    void loadOrSetupInstallPath() {
        std::ifstream path_file("path.json");
        if(path_file.good()) {
            std::string json;
            std::getline(path_file, json);
            path_file.close();

            size_t pathStart = json.find("\"path\":\"");
            size_t pathEnd = json.find("\"", pathStart + 8);
            if(pathStart != std::string::npos && pathEnd != std::string::npos) {
                installPath = json.substr(pathStart + 8, pathEnd - (pathStart + 8));
                if(fs::exists(installPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe")) {
                    std::cout << "\nLoaded Fortnite path: " << installPath << "\n";
                    return;
                }
            }
        }

        std::cout << "\nFortnite Installation Setup\n";
        std::cout << "===========================\n\n";
        
        std::string defaultPath = "C:\\Program Files\\Epic Games\\Fortnite";
        if(fs::exists(defaultPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe")) {
            std::cout << "Found existing installation at: " << defaultPath << "\n";
            installPath = defaultPath;
        } else {
            std::cout << "Enter Fortnite installation path: ";
            std::getline(std::cin, installPath);
            
            if(!fs::exists(installPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe")) {
                std::cout << "Invalid path or Fortnite not found. Please install Fortnite first.\n";
                system("pause");
                exit(1);
            }
        }

        std::ofstream path_out("path.json");
        path_out << "{\"path\":\"" << installPath << "\"}";
        path_out.close();
        
        std::cout << "\nPath saved successfully!\n";
    }

    bool start() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed" << std::endl;
            WSACleanup();
            return false;
        }

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        running = true;
        std::cout << "\n[SERVER] Started on port " << PORT << std::endl;
        
        std::ofstream auth_file("auth_token.json");
        auth_file << "{\"access_token\":\"" << authToken << "\",\"displayName\":\"" << displayName << "\"}";
        auth_file.close();

        std::thread(&FortniteServer::handleMatchmaking, this).detach();
        startPatcher();

        std::thread([this]() {
            while (running) {
                SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
                if (clientSocket != INVALID_SOCKET) {
                    std::thread(&FortniteServer::handleClient, this, clientSocket).detach();
                }
            }
        }).detach();

        return true;
    }

    void launchGame() {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));
        si.cb = sizeof(si);

        // Enhanced Season 2 launch parameters
        std::string cmd = "\"" + installPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe\"";
        cmd += " -NOSSLPINNING -AUTH_TYPE=epic -AUTH_LOGIN=unused -AUTH_PASSWORD=" + authToken;
        cmd += " -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck";
        cmd += " -HTTP=127.0.0.1:7777 -AUTH_HOST=127.0.0.1:7777";
        cmd += " -NOTEXTURESTREAMING -USEALLAVAILABLECORES -PREFERREDPROCESSOR=0";
        cmd += " -NOSSLPINNING_V2 -ALLOWALLSSL -BYPASSSSL -NOENCRYPTION";
        cmd += " -DISABLEPATCHCHECK -DISABLELOGGEDOUT";
        cmd += " -SEASON=2 -FORCESEASONCLIENT=2";
        cmd += " -NOCRASHREPORT -NOCRASHREPORTCLIENT";
        cmd += " -HEAPSIZE=2048";
        cmd += " -NOTEXTURESTREAMING";
        cmd += " -USEALLAVAILABLECORES";
        cmd += " -NOSOUND";
        cmd += " -LANPLAY";
        cmd += " -NOSTEAM";

        std::string workingDir = installPath + "\\FortniteGame\\Binaries\\Win64";

        std::cout << "[LAUNCHER] Starting Fortnite Season 2...\n";

        if (!CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                         CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS,
                         NULL, workingDir.c_str(), &si, &pi)) {
            std::cerr << "Failed to launch game" << std::endl;
            return;
        }

        gameProcess = pi.hProcess;

        // Wait for process to initialize
        Sleep(3000);

        std::cout << "[LAUNCHER] Applying Season 2 patches...\n";
        if (!LivePatchFortnite()) {
            std::cerr << "Failed to apply Season 2 patches\n";
            TerminateProcess(gameProcess, 0);
            CloseHandle(gameProcess);
            return;
        }

        SetPriorityClass(gameProcess, HIGH_PRIORITY_CLASS);
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);

        std::cout << "\n[LAUNCHER] Season 2 launched successfully!\n";
        std::cout << "[LAUNCHER] Waiting for game to initialize...\n";
        
        Sleep(5000);
        
        std::thread([this]() {
            while (WaitForSingleObject(gameProcess, 100) == WAIT_TIMEOUT) {
                Sleep(1000);
            }
            DWORD exitCode;
            GetExitCodeProcess(gameProcess, &exitCode);
            std::cout << "\n[LAUNCHER] Game process ended (Exit code: " << exitCode << ")\n";
            stop();
        }).detach();
    }

    void stop() {
        if(gameProcess != NULL) {
            TerminateProcess(gameProcess, 0);
            CloseHandle(gameProcess);
        }
        
        if(patcherProcess != NULL) {
            TerminateProcess(patcherProcess, 0);
            CloseHandle(patcherProcess);
        }
        
        running = false;
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
        WSACleanup();
    }
};

int main(int argc, char* argv[]) {
    SetConsoleTitle("ZeroFN Launcher");
    
    FortniteServer server;
    if(server.start()) {
        std::cout << "\nServer started successfully!\n";
        std::cout << "Starting Fortnite...\n";
        server.launchGame();
        
        std::cout << "\nPress Enter to stop the server...";
        std::cin.get();
        server.stop();
    }
    
    return 0;
}


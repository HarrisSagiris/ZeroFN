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
#include <nlohmann/json.hpp>

// ZeroFN Version 1.2.2
// Developed by DevHarris
// A private server implementation for Fortnite

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;

struct GameSession {
    std::string sessionId;
    std::vector<std::string> players;
    bool inProgress;
    std::string playlistId;
    std::map<std::string, json> playerLoadouts;
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
    
    json playerLoadout;
    json playerInventory;
    json cosmeticsDb;

    void loadCosmeticsDatabase() {
        std::ifstream cosmeticsFile("cosmetics.json");
        if(cosmeticsFile.good()) {
            cosmeticsFile >> cosmeticsDb;
        } else {
            // Initialize default cosmetics database
            cosmeticsDb = {
                {"characters", {
                    {"CID_001_Athena_Commando_F_Default", "Default"},
                    {"CID_002_Athena_Commando_F_Default", "Ramirez"},
                    {"CID_003_Athena_Commando_F_Default", "Banshee"}
                }},
                {"backpacks", {
                    {"BID_001_Default", "Default Cape"},
                    {"BID_002_Default", "Black Shield"},
                    {"BID_003_Default", "Wings"}
                }},
                {"pickaxes", {
                    {"Pickaxe_Default", "Default Pickaxe"},
                    {"Pickaxe_ID_001", "AC/DC"},
                    {"Pickaxe_ID_002", "Raider's Revenge"}
                }},
                {"gliders", {
                    {"Glider_Default", "Default Glider"},
                    {"Glider_ID_001", "Mako"},
                    {"Glider_ID_002", "Snowflake"}
                }},
                {"emotes", {
                    {"EID_DanceDefault", "Dance Moves"},
                    {"EID_Floss", "Floss"},
                    {"EID_TakeTheL", "Take The L"},
                    {"EID_Dab", "Dab"}
                }}
            };

            std::ofstream out("cosmetics.json");
            out << cosmeticsDb.dump(4);
        }
    }

    void initializePlayerData() {
        loadCosmeticsDatabase();
        
        playerLoadout = {
            {"character", "CID_001_Athena_Commando_F_Default"},
            {"backpack", "BID_001_Default"},
            {"pickaxe", "Pickaxe_Default"},
            {"glider", "Glider_Default"},
            {"contrail", "Trails_Default"},
            {"emotes", json::array({
                "EID_DanceDefault",
                "EID_Floss",
                "EID_TakeTheL",
                "EID_Dab"
            })}
        };

        playerInventory = cosmeticsDb;
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

    json parseRequestBody(const std::string& request) {
        size_t bodyStart = request.find("\r\n\r\n");
        if(bodyStart != std::string::npos) {
            std::string body = request.substr(bodyStart + 4);
            try {
                return json::parse(body);
            } catch(...) {}
        }
        return json::object();
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
                        session.playerLoadouts[playerId] = playerLoadout;
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
            json requestBody = parseRequestBody(request);

            std::string headers = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: application/json\r\n"
                                "Access-Control-Allow-Origin: *\r\n"
                                "Connection: keep-alive\r\n\r\n";

            if (endpoint == "/account/api/oauth/token") {
                json response = {
                    {"access_token", authToken},
                    {"expires_in", 28800},
                    {"expires_at", "9999-12-31T23:59:59.999Z"},
                    {"token_type", "bearer"},
                    {"refresh_token", authToken},
                    {"refresh_expires", 28800},
                    {"refresh_expires_at", "9999-12-31T23:59:59.999Z"},
                    {"account_id", accountId},
                    {"client_id", "ec684b8c687f479fadea3cb2ad83f5c6"},
                    {"internal_client", true},
                    {"client_service", "fortnite"},
                    {"displayName", displayName},
                    {"app", "fortnite"},
                    {"in_app_id", accountId}
                };
                
                sendResponse(clientSocket, headers + response.dump());
            }
            else if (endpoint == "/account/api/public/account") {
                json response = {
                    {"id", accountId},
                    {"displayName", displayName},
                    {"externalAuths", json::object()}
                };
                
                sendResponse(clientSocket, headers + response.dump());
            }
            else if (endpoint.find("/fortnite/api/game/v2/profile/" + accountId + "/client") != std::string::npos) {
                json response = {
                    {"profileId", "athena"},
                    {"profileChanges", {{
                        {"_type", "fullProfileUpdate"},
                        {"profile", {
                            {"_id", accountId},
                            {"accountId", accountId},
                            {"profileId", "athena"},
                            {"version", "1"},
                            {"items", playerInventory},
                            {"stats", {
                                {"attributes", {
                                    {"season_num", 0},
                                    {"loadout", playerLoadout}
                                }}
                            }},
                            {"commandRevision", 1}
                        }}
                    }},
                    {"profileCommandRevision", 1},
                    {"serverTime", "2023-12-31T23:59:59.999Z"},
                    {"responseVersion", 1}
                };

                if(endpoint.find("SetCosmeticLockerSlot") != std::string::npos) {
                    std::string category = requestBody["category"];
                    std::string itemId = requestBody["itemId"];
                    
                    if(playerInventory[category].contains(itemId)) {
                        playerLoadout[category] = itemId;
                        std::cout << "[COSMETICS] Player changed " << category << " to " << itemId << std::endl;
                    }
                }
                
                sendResponse(clientSocket, headers + response.dump());
            }
            else if (endpoint.find("/fortnite/api/matchmaking/session/findPlayer/") != std::string::npos) {
                std::lock_guard<std::mutex> lock(matchmakingMutex);
                matchmakingQueue.push_back(accountId);
                
                json response = {
                    {"status", "waiting"},
                    {"priority", 0},
                    {"ticket", "ticket_" + std::to_string(rand())},
                    {"queuedPlayers", matchmakingQueue.size()}
                };
                
                sendResponse(clientSocket, headers + response.dump());
            }
            else if (endpoint.find("/fortnite/api/matchmaking/session/matchMakingRequest") != std::string::npos) {
                json response;
                std::lock_guard<std::mutex> lock(matchmakingMutex);
                
                bool found = false;
                for(const auto& session : activeSessions) {
                    auto it = std::find(session.second.players.begin(), 
                                      session.second.players.end(), 
                                      accountId);
                    if(it != session.second.players.end()) {
                        response = {
                            {"status", "found"},
                            {"matchId", session.first},
                            {"sessionId", session.first},
                            {"playlistId", session.second.playlistId}
                        };
                        found = true;
                        break;
                    }
                }
                
                if(!found) {
                    response = {
                        {"status", "waiting"},
                        {"estimatedWaitSeconds", 5}
                    };
                }
                
                sendResponse(clientSocket, headers + response.dump());
            }
            else {
                json response = {{"status", "ok"}};
                sendResponse(clientSocket, headers + response.dump());
            }
        }

        closesocket(clientSocket);
    }

public:
    bool LivePatchFortnite() {
        std::cout << "\n[LIVE PATCHER] Starting live patching process...\n";

        DWORD processId = 0;
        int retryCount = 0;
        const int MAX_RETRIES = 15;
        
        while (processId == 0 && retryCount < MAX_RETRIES) {
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
                std::cout << "[LIVE PATCHER] Waiting for Fortnite process... Attempt " << (retryCount + 1) << "/" << MAX_RETRIES << "\n";
                Sleep(1000);
                retryCount++;
            }
        }

        if (processId == 0) {
            std::cout << "[LIVE PATCHER] Failed to find Fortnite process\n";
            return false;
        }

        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (!processHandle) {
            std::cout << "[LIVE PATCHER] Failed to open process\n";
            return false;
        }

        std::vector<std::pair<std::vector<BYTE>, std::vector<BYTE>>> patches = {
            {{0x75, 0x14, 0x48, 0x8B, 0x0D}, {0xEB, 0x14, 0x48, 0x8B, 0x0D}},
            {{0x74, 0x20, 0x48, 0x8B, 0x5C}, {0xEB, 0x20, 0x48, 0x8B, 0x5C}},
            {{0x0F, 0x84, 0x85, 0x00, 0x00, 0x00}, {0x90, 0xE9, 0x85, 0x00, 0x00, 0x00}},
            {{0x74, 0x23, 0x48, 0x8B, 0x4C}, {0xEB, 0x23, 0x48, 0x8B, 0x4C}},
            {{0x75, 0x08, 0x8B, 0x45, 0xE8}, {0xEB, 0x08, 0x8B, 0x45, 0xE8}},
            // Additional crash prevention patches
            {{0x75, 0x1F, 0x48, 0x8B, 0x45}, {0xEB, 0x1F, 0x48, 0x8B, 0x45}},
            {{0x74, 0x15, 0x48, 0x8B, 0x4D}, {0xEB, 0x15, 0x48, 0x8B, 0x4D}},
            {{0x0F, 0x85, 0x95, 0x00, 0x00}, {0xE9, 0x96, 0x00, 0x00, 0x00}}
        };

        MEMORY_BASIC_INFORMATION mbi;
        LPVOID address = 0;
        bool patchSuccess = false;

        while (VirtualQueryEx(processHandle, address, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && 
                (mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE)) {
                
                std::vector<BYTE> buffer(mbi.RegionSize);
                SIZE_T bytesRead;
                
                if (ReadProcessMemory(processHandle, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead)) {
                    for (const auto& patch : patches) {
                        for (size_t i = 0; i < buffer.size() - patch.first.size(); i++) {
                            if (memcmp(buffer.data() + i, patch.first.data(), patch.first.size()) == 0) {
                                LPVOID patchAddress = (LPVOID)((DWORD_PTR)mbi.BaseAddress + i);
                                SIZE_T bytesWritten;
                                DWORD oldProtect;

                                if (VirtualProtectEx(processHandle, patchAddress, patch.second.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
                                    if (WriteProcessMemory(processHandle, patchAddress, patch.second.data(), patch.second.size(), &bytesWritten)) {
                                        VirtualProtectEx(processHandle, patchAddress, patch.second.size(), oldProtect, &oldProtect);
                                        std::cout << "[LIVE PATCHER] Successfully applied patch at " 
                                                  << std::hex << patchAddress << std::dec << "\n";
                                        patchSuccess = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            address = (LPVOID)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize);
        }

        CloseHandle(processHandle);
        return patchSuccess;
    }

private:
    void startPatcher() {
        STARTUPINFO si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        
        std::string cmdLine = "cmd.exe /c title ZeroFN Patcher && echo Running patcher... && timeout /t 2 >nul";
        
        if (CreateProcess(NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE,
                         CREATE_NEW_CONSOLE | CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            patcherProcess = pi.hProcess;
            CloseHandle(pi.hThread);

            std::thread([this]() {
                while(running) {
                    LivePatchFortnite();
                    Sleep(5000);
                }
            }).detach();
        }
    }

public:
    FortniteServer() : running(false), serverSocket(INVALID_SOCKET), gameProcess(NULL), patcherProcess(NULL) {
        srand(static_cast<unsigned>(time(0)));
        
        system("cls");
        std::cout << "\nZeroFN Version 1.2.2\n";
        std::cout << "Developed by DevHarris\n\n";
        
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

        startPatcher();

        std::thread(&FortniteServer::handleMatchmaking, this).detach();

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

        std::string cmd = "\"" + installPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe\"";
        cmd += " -NOSSLPINNING -AUTH_TYPE=epic -AUTH_LOGIN=unused -AUTH_PASSWORD=" + authToken;
        cmd += " -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck";
        cmd += " -HTTP=127.0.0.1:7777 -AUTH_HOST=127.0.0.1:7777";
        cmd += " -NOTEXTURESTREAMING -USEALLAVAILABLECORES -PREFERREDPROCESSOR=0";
        cmd += " -NOSSLPINNING_V2 -ALLOWALLSSL -BYPASSSSL -NOENCRYPTION";
        cmd += " -DISABLEPATCHCHECK -DISABLELOGGEDOUT";

        std::string workingDir = installPath + "\\FortniteGame\\Binaries\\Win64";

        if (!CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                         CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS,
                         NULL, workingDir.c_str(), &si, &pi)) {
            std::cerr << "Failed to launch game" << std::endl;
            return;
        }

        gameProcess = pi.hProcess;
        SetPriorityClass(gameProcess, HIGH_PRIORITY_CLASS);
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);

        std::cout << "\nGame launched successfully!\n";
        
        std::thread([this]() {
            while (WaitForSingleObject(gameProcess, 100) == WAIT_TIMEOUT) {
                Sleep(1000);
            }
            std::cout << "Game process ended\n";
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

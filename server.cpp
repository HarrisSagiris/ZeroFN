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
#include <filesystem>
#include <direct.h>
#include <map>
#include <mutex>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <TlHelp32.h>
#include <Psapi.h>
#include <dwmapi.h>
#include <Richedit.h>

namespace fs = std::filesystem;

// Game session data
struct GameSession {
    std::string sessionId;
    std::vector<std::string> players;
    bool inProgress;
    std::string playlistId;
};

// Custom window procedure for patcher window
LRESULT CALLBACK PatcherWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
            LoadLibrary("Msftedit.dll");
            CreateWindowExA(0, "RICHEDIT50W", "",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
                10, 10, 780, 580, hwnd, NULL, hInstance, NULL);
            return 0;
        }
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

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
    HANDLE outputPipe;
    HWND patcherWindow;
    HWND logControl;

    // Matchmaking state
    std::map<std::string, GameSession> activeSessions;
    std::vector<std::string> matchmakingQueue;
    std::mutex matchmakingMutex;
    
    // Cosmetics and inventory
    std::map<std::string, std::string> playerLoadout;
    std::map<std::string, std::vector<std::string>> playerInventory;

    void initializePlayerData() {
        // Basic cosmetics loadout
        playerLoadout["character"] = "CID_001_Athena_Commando_F_Default";
        playerLoadout["backpack"] = "BID_001_Default";
        playerLoadout["pickaxe"] = "Pickaxe_Default";
        playerLoadout["glider"] = "Glider_Default";
        playerLoadout["contrail"] = "Trails_Default";

        // Basic inventory with some default items
        playerInventory["characters"] = {"CID_001_Athena_Commando_F_Default"};
        playerInventory["backpacks"] = {"BID_001_Default"};
        playerInventory["pickaxes"] = {"Pickaxe_Default"};
        playerInventory["gliders"] = {"Glider_Default"};
        playerInventory["contrails"] = {"Trails_Default"};
        playerInventory["emotes"] = {
            "EID_DanceDefault",
            "EID_DanceDefault2", 
            "EID_DanceDefault3",
            "EID_DanceDefault4",
            "EID_DanceDefault5",
            "EID_DanceDefault6"
        };
    }

    void sendResponse(SOCKET clientSocket, const std::string& response) {
        send(clientSocket, response.c_str(), response.length(), 0);
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

    std::string generateMatchId() {
        return "MATCH_" + std::to_string(rand());
    }

    void handleMatchmaking() {
        while(running) {
            std::lock_guard<std::mutex> lock(matchmakingMutex);
            
            if(matchmakingQueue.size() >= 2) {
                GameSession session;
                session.sessionId = generateMatchId();
                session.inProgress = true;
                session.playlistId = "Playlist_DefaultSolo";
                
                while(!matchmakingQueue.empty() && session.players.size() < 100) {
                    session.players.push_back(matchmakingQueue.back());
                    matchmakingQueue.pop_back();
                }
                
                activeSessions[session.sessionId] = session;
                
                LogMessage("[MATCHMAKING] Created session " + session.sessionId + 
                          " with " + std::to_string(session.players.size()) + " players");
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void handleClient(SOCKET clientSocket) {
        char buffer[8192];
        std::string request;
        
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            request = std::string(buffer, bytesReceived);
            std::string endpoint = parseRequest(request);

            std::string headers = "HTTP/1.1 200 OK\r\n";
            headers += "Content-Type: application/json\r\n";
            headers += "Access-Control-Allow-Origin: *\r\n";
            headers += "Connection: close\r\n\r\n";

            if (endpoint == "/account/api/oauth/token") {
                std::string response = "{";
                response += "\"access_token\":\"" + authToken + "\",";
                response += "\"expires_in\":28800,";
                response += "\"expires_at\":\"9999-12-31T23:59:59.999Z\",";
                response += "\"token_type\":\"bearer\",";
                response += "\"refresh_token\":\"" + authToken + "\",";
                response += "\"refresh_expires\":28800,";
                response += "\"refresh_expires_at\":\"9999-12-31T23:59:59.999Z\",";
                response += "\"account_id\":\"" + accountId + "\",";
                response += "\"client_id\":\"ec684b8c687f479fadea3cb2ad83f5c6\",";
                response += "\"internal_client\":true,";
                response += "\"client_service\":\"fortnite\",";
                response += "\"displayName\":\"" + displayName + "\",";
                response += "\"app\":\"fortnite\",";
                response += "\"in_app_id\":\"" + accountId + "\"";
                response += "}";
                
                sendResponse(clientSocket, headers + response);
            }
            else if (endpoint == "/account/api/public/account") {
                std::string response = "{";
                response += "\"id\":\"" + accountId + "\",";
                response += "\"displayName\":\"" + displayName + "\",";
                response += "\"externalAuths\":{}";
                response += "}";
                
                sendResponse(clientSocket, headers + response);
            }
            else if (endpoint == "/fortnite/api/game/v2/profile/" + accountId + "/client/QueryProfile") {
                std::string response = "{";
                response += "\"profileId\":\"athena\",";
                response += "\"profileChanges\":[{";
                response += "\"_type\":\"fullProfileUpdate\",";
                response += "\"profile\":{";
                response += "\"_id\":\"" + accountId + "\",";
                response += "\"accountId\":\"" + accountId + "\",";
                response += "\"profileId\":\"athena\",";
                response += "\"version\":\"1\",";
                response += "\"items\":{},";
                response += "\"stats\":{\"attributes\":{\"season_num\":0}},";
                response += "\"commandRevision\":1";
                response += "}}],";
                response += "\"profileCommandRevision\":1,";
                response += "\"serverTime\":\"2023-12-31T23:59:59.999Z\",";
                response += "\"responseVersion\":1";
                response += "}";
                
                sendResponse(clientSocket, headers + response);
            }
            else if (endpoint.find("/fortnite/api/matchmaking/session/findPlayer/") != std::string::npos) {
                std::lock_guard<std::mutex> lock(matchmakingMutex);
                matchmakingQueue.push_back(accountId);
                
                std::string response = "{";
                response += "\"status\":\"waiting\",";
                response += "\"priority\":0,";
                response += "\"ticket\":\"ticket_" + std::to_string(rand()) + "\",";
                response += "\"queuedPlayers\":" + std::to_string(matchmakingQueue.size());
                response += "}";
                
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
                        response = "{";
                        response += "\"status\":\"found\",";
                        response += "\"matchId\":\"" + session.first + "\",";
                        response += "\"sessionId\":\"" + session.first + "\",";
                        response += "\"playlistId\":\"" + session.second.playlistId + "\"";
                        response += "}";
                        found = true;
                        break;
                    }
                }
                
                if(!found) {
                    response = "{";
                    response += "\"status\":\"waiting\",";
                    response += "\"estimatedWaitSeconds\":10";
                    response += "}";
                }
                
                sendResponse(clientSocket, headers + response);
            }
            else {
                std::string response = "{\"status\":\"ok\"}";
                sendResponse(clientSocket, headers + response);
            }
        }

        closesocket(clientSocket);
    }

    bool PatchMemory(HANDLE process, LPVOID address, const std::vector<BYTE>& patch) {
        SIZE_T bytesWritten;
        DWORD oldProtect;

        Sleep(100);

        try {
            MEMORY_BASIC_INFORMATION mbi;
            if (!VirtualQueryEx(process, address, &mbi, sizeof(mbi)))
                return false;

            if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_GUARD)
                return false;

            std::vector<BYTE> currentBytes(patch.size());
            if (!ReadProcessMemory(process, address, currentBytes.data(), patch.size(), nullptr))
                return false;

            if (!VirtualProtectEx(process, address, patch.size(), PAGE_EXECUTE_READWRITE, &oldProtect))
                return false;

            if (!WriteProcessMemory(process, address, patch.data(), patch.size(), &bytesWritten))
                return false;

            std::vector<BYTE> verifyBytes(patch.size());
            if (!ReadProcessMemory(process, address, verifyBytes.data(), patch.size(), nullptr))
                return false;

            if (memcmp(verifyBytes.data(), patch.data(), patch.size()) != 0) {
                VirtualProtectEx(process, address, patch.size(), oldProtect, &oldProtect);
                return false;
            }

            if (!VirtualProtectEx(process, address, patch.size(), oldProtect, &oldProtect))
                return false;

            return bytesWritten == patch.size();
        }
        catch (...) {
            LogMessage("[LIVE PATCHER] Exception occurred while patching memory");
            return false;
        }
    }

    void LogMessage(const std::string& message) {
        std::cout << message << std::endl;
    }

    bool LivePatchFortnite() {
        LogMessage("\n[LIVE PATCHER] Starting live patching process...");

        DWORD processId = 0;
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);
        
        while (processId == 0 && running) {
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snapshot != INVALID_HANDLE_VALUE) {
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
                LogMessage("[LIVE PATCHER] Waiting for Fortnite process...");
                Sleep(1000);
            }
        }

        if (!running) return false;

        Sleep(5000);

        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (processHandle == NULL) {
            LogMessage("[LIVE PATCHER] Failed to open process");
            return false;
        }

        LogMessage("[LIVE PATCHER] Successfully attached to Fortnite process");

        struct Patch {
            std::string name;
            std::vector<BYTE> find;
            std::vector<BYTE> replace;
            bool critical;
            int retryCount;
        };

        std::vector<Patch> patches = {
            {"Login Bypass 1", 
             {0x75, 0x14, 0x48, 0x8B, 0x0D}, 
             {0xEB, 0x14, 0x48, 0x8B, 0x0D},
             false,
             2},
            
            {"Server Auth Bypass",
             {0x74, 0x20, 0x48, 0x8B, 0x5C},
             {0xEB, 0x20, 0x48, 0x8B, 0x5C},
             true,
             2},
             
            {"SSL Bypass",
             {0x0F, 0x84, 0x85, 0x00, 0x00, 0x00},
             {0x90, 0x90, 0x90, 0x90, 0x90, 0x90},
             false,
             2},
             
            {"Login Flow Bypass",
             {0x74, 0x23, 0x48, 0x8B, 0x4C},
             {0xEB, 0x23, 0x48, 0x8B, 0x4C},
             false,
             2},
             
            {"Server Validation",
             {0x75, 0x08, 0x48, 0x8B, 0x01},
             {0xEB, 0x08, 0x48, 0x8B, 0x01},
             true,
             2}
        };

        MEMORY_BASIC_INFORMATION mbi;
        LPVOID address = 0;
        bool criticalPatchesFailed = false;
        
        while (VirtualQueryEx(processHandle, address, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && 
                (mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE)) {
                
                std::vector<BYTE> buffer(mbi.RegionSize);
                SIZE_T bytesRead;
                
                try {
                    if (ReadProcessMemory(processHandle, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead)) {
                        for (auto& patch : patches) {
                            for (size_t i = 0; i < buffer.size() - patch.find.size(); i++) {
                                if (memcmp(buffer.data() + i, patch.find.data(), patch.find.size()) == 0) {
                                    LPVOID patchAddress = (LPVOID)((DWORD_PTR)mbi.BaseAddress + i);
                                    
                                    bool patchSuccess = false;
                                    for (int attempt = 0; attempt < patch.retryCount && !patchSuccess; attempt++) {
                                        if (attempt > 0) {
                                            Sleep(100 * (attempt + 1));
                                        }
                                        
                                        patchSuccess = PatchMemory(processHandle, patchAddress, patch.replace);
                                        
                                        if (patchSuccess) {
                                            std::stringstream ss;
                                            ss << "[LIVE PATCHER] Applied " << patch.name << " at 0x" 
                                               << std::hex << patchAddress;
                                            LogMessage(ss.str());
                                            break;
                                        }
                                    }
                                    
                                    if (!patchSuccess) {
                                        LogMessage("[LIVE PATCHER] Failed to apply " + patch.name);
                                        if (patch.critical) {
                                            criticalPatchesFailed = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                catch (...) {
                    std::stringstream ss;
                    ss << "[LIVE PATCHER] Exception while scanning memory region at 0x" 
                       << std::hex << mbi.BaseAddress;
                    LogMessage(ss.str());
                }
            }
            address = (LPVOID)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize);
        }

        CloseHandle(processHandle);
        
        if (criticalPatchesFailed) {
            LogMessage("[LIVE PATCHER] Critical patches failed, game may be unstable");
            return false;
        }
        
        return true;
    }

    bool patchGameExecutable() {
        LogMessage("\n[PATCHER] Starting game executable patching...");
        
        std::thread([this]() {
            int failedAttempts = 0;
            const int MAX_FAILED_ATTEMPTS = 2;
            
            while (running) {
                if (!LivePatchFortnite()) {
                    failedAttempts++;
                    LogMessage("[PATCHER] Patch attempt failed (" + 
                             std::to_string(failedAttempts) + "/" + 
                             std::to_string(MAX_FAILED_ATTEMPTS) + ")");
                    
                    if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
                        LogMessage("[PATCHER] Too many failed attempts, waiting longer before retry...");
                        Sleep(30000);
                        failedAttempts = 0;
                    }
                } else {
                    failedAttempts = 0;
                    LogMessage("[PATCHER] Live patches applied successfully");
                    Sleep(5000);
                }
            }
        }).detach();

        return true;
    }

public:
    FortniteServer() : running(false), serverSocket(INVALID_SOCKET), gameProcess(NULL) {
        srand(time(0));
        
        system("cls");
        SetConsoleTitle("ZeroFN Terminal");
        
        std::cout << "=====================================\n";
        std::cout << "    Welcome to ZeroFN Launcher v1.1\n";
        std::cout << "    Developed by DevHarris\n";
        std::cout << "=====================================\n\n";
        Sleep(1000);
        
        displayName = "ZeroFN_User_" + std::to_string(rand() % 10000);
        std::cout << "Using display name: " << displayName << std::endl;
        Sleep(1000);
        
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
                    std::cout << "Loaded installation path: " << installPath << "\n";
                    Sleep(1000);
                    return;
                }
            }
        }

        std::cout << "\nFortnite Installation Setup\n";
        std::cout << "===========================\n";
        Sleep(1000);
        
        std::string defaultPath = "C:\\FortniteOG";
        if(fs::exists(defaultPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe")) {
            std::cout << "Found existing installation at: " << defaultPath << "\n";
            installPath = defaultPath;
            Sleep(1000);
        } else {
            installFortniteOG();
        }

        std::ofstream path_out("path.json");
        path_out << "{\"path\":\"" << installPath << "\"}";
        path_out.close();
    }

    void installFortniteOG() {
        std::cout << "\nInstalling Fortnite OG...\n";
        Sleep(1000);
        installPath = "C:\\FortniteOG";
        
        _mkdir(installPath.c_str());
        
        std::cout << "Downloading Fortnite OG files...\n";
        Sleep(1000);
        std::cout << "Please place Fortnite files in: " << installPath << "\n";
        Sleep(5000);
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
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
            std::cerr << "setsockopt failed" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return false;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(PORT);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
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
        std::cout << "[SERVER] Username: " << displayName << std::endl;
        Sleep(1000);
        
        std::ofstream auth_file("auth_token.json");
        auth_file << "{\"access_token\":\"" << authToken << "\",\"displayName\":\"" << displayName << "\"}";
        auth_file.close();

        if(!patchGameExecutable()) {
            std::cerr << "Failed to patch game executable" << std::endl;
            return false;
        }

        std::thread(&FortniteServer::handleMatchmaking, this).detach();

        std::thread([this]() {
            while (running) {
                SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
                if (clientSocket != INVALID_SOCKET) {
                    LogMessage("[SERVER] Client connected");
                    std::thread(&FortniteServer::handleClient, this, clientSocket).detach();
                }
            }
        }).detach();

        return true;
    }

    void launchGame() {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(STARTUPINFO));
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
        si.cb = sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOW;

        std::string cmd = "\"" + installPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe\"";
        cmd += " -NOSSLPINNING -AUTH_TYPE=epic -AUTH_LOGIN=unused -AUTH_PASSWORD=" + authToken;
        cmd += " -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck";
        cmd += " -notexturestreaming -HTTP=127.0.0.1:7777 -AUTH_HOST=127.0.0.1:7777 -AUTH_SSL=0 -AUTH_VERIFY_SSL=0";
        cmd += " -AUTH_EPIC=0 -AUTH_EPIC_ONLY=0 -FORCECLIENT=127.0.0.1:7777 -NOEPICWEB -NOEPICFRIENDS -NOEAC -NOBE";
        cmd += " -FORCECLIENT_HOST=127.0.0.1:7777 -DISABLEFORTNITELOGIN -DISABLEEPICLOGIN -DISABLEEPICGAMESLOGIN";
        cmd += " -DISABLEEPICGAMESPORTAL -DISABLEEPICGAMESVERIFY -epicport=7777";
        cmd += " -NOSSLPINNING_V2 -ALLOWALLSSL -BYPASSSSL -NOENCRYPTION -NOSTEAM -NOEAC_V2 -NOBE_V2";
        cmd += " -DISABLEPATCHCHECK -DISABLELOGGEDOUT -USEALLAVAILABLECORES -PREFERREDPROCESSOR=0";
        cmd += " -DISABLEFORTNITELOGIN_V2 -DISABLEEPICLOGIN_V2 -DISABLEEPICGAMESLOGIN_V2";
        cmd += " -DISABLEEPICGAMESPORTAL_V2 -DISABLEEPICGAMESVERIFY_V2";
        cmd += " -NOENCRYPTION_V2 -NOSTEAM_V2";
        cmd += " -ALLOWALLSSL_V2 -BYPASSSSL_V2";

        std::string workingDir = installPath + "\\FortniteGame\\Binaries\\Win64";

        if (!CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                         CREATE_NEW_CONSOLE | HIGH_PRIORITY_CLASS,
                         NULL, workingDir.c_str(), &si, &pi)) {
            LogMessage("Failed to launch game. Error code: " + std::to_string(GetLastError()));
            return;
        }

        gameProcess = pi.hProcess;
        CloseHandle(pi.hThread);

        std::cout << "Game launched successfully with all patches and bypasses enabled!" << std::endl;
        Sleep(1000);
        
        std::thread([this]() {
            while (WaitForSingleObject(gameProcess, 100) == WAIT_TIMEOUT) {
                Sleep(1000);
            }
            std::cout << "Game process terminated" << std::endl;
        }).detach();
    }

    void stop() {
        if(gameProcess != NULL) {
            TerminateProcess(gameProcess, 0);
            CloseHandle(gameProcess);
            CloseHandle(outputPipe);
        }
        
        running = false;
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
        WSACleanup();
        std::cout << "[SERVER] Stopped" << std::endl;
    }
};

int main() {
    system("cls");
    SetConsoleTitle("ZeroFN Terminal");
    
    FortniteServer server;
    if(server.start()) {
        std::cout << "\nServer started successfully!\n";
        Sleep(1000);
        std::cout << "Starting Fortnite with all patches and bypasses...\n";
        Sleep(1000);
        server.launchGame();
        
        while(true) {
            Sleep(1000);
        }
    }
    
    return 0;
}

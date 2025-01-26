#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h> // For InetPton
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

// ZeroFN Version 1.2.1
// Developed by DevHarris
// A private server implementation for Fortnite with enhanced stability

namespace fs = std::experimental::filesystem;

// Game session data
struct GameSession {
    std::string sessionId;
    std::vector<std::string> players;
    bool inProgress;
    std::string playlistId;
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
    HANDLE outputPipe;
    HWND patcherWindow;

    // Matchmaking state with improved synchronization
    std::map<std::string, GameSession> activeSessions;
    std::vector<std::string> matchmakingQueue;
    std::mutex matchmakingMutex;
    
    // Cosmetics and inventory with optimized loading
    std::map<std::string, std::string> playerLoadout;
    std::map<std::string, std::vector<std::string>> playerInventory;

    void initializePlayerData() {
        // Basic cosmetics loadout with reduced memory footprint
        playerLoadout["character"] = "CID_001_Athena_Commando_F_Default";
        playerLoadout["backpack"] = "BID_001_Default";
        playerLoadout["pickaxe"] = "Pickaxe_Default";
        playerLoadout["glider"] = "Glider_Default";
        playerLoadout["contrail"] = "Trails_Default";

        // Optimized inventory initialization
        playerInventory["characters"] = {"CID_001_Athena_Commando_F_Default"};
        playerInventory["backpacks"] = {"BID_001_Default"};
        playerInventory["pickaxes"] = {"Pickaxe_Default"};
        playerInventory["gliders"] = {"Glider_Default"};
        playerInventory["contrails"] = {"Trails_Default"};
        playerInventory["emotes"] = {
            "EID_DanceDefault",
            "EID_DanceDefault2", 
            "EID_DanceDefault3"
        };
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
                    
                    // Limit players per session for better stability
                    const size_t maxPlayers = 16;
                    while(!matchmakingQueue.empty() && session.players.size() < maxPlayers) {
                        session.players.push_back(matchmakingQueue.back());
                        matchmakingQueue.pop_back();
                    }
                    
                    activeSessions[session.sessionId] = session;
                    
                    std::cout << "[MATCHMAKING] Created session " << session.sessionId 
                             << " with " << session.players.size() << " players" << std::endl;
                }
            }
            
            // Reduced sleep time for more responsive matchmaking
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    void handleClient(SOCKET clientSocket) {
        const int bufferSize = 16384; // Increased buffer size
        std::vector<char> buffer(bufferSize);
        
        int bytesReceived = recv(clientSocket, buffer.data(), bufferSize, 0);
        if (bytesReceived > 0) {
            std::string request(buffer.data(), bytesReceived);
            std::string endpoint = parseRequest(request);

            std::string headers = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: application/json\r\n"
                                "Access-Control-Allow-Origin: *\r\n"
                                "Connection: keep-alive\r\n\r\n";

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
                // Simplified profile response for better stability
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
                    response += "\"estimatedWaitSeconds\":5";
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

        if (!VirtualProtectEx(process, address, patch.size(), PAGE_EXECUTE_READWRITE, &oldProtect)) {
            return false;
        }

        bool success = WriteProcessMemory(process, address, patch.data(), patch.size(), &bytesWritten);

        VirtualProtectEx(process, address, patch.size(), oldProtect, &oldProtect);

        return success && (bytesWritten == patch.size());
    }

public:
    bool LivePatchFortnite() {
        std::cout << "\n[LIVE PATCHER] Starting live patching process...\n";

        DWORD processId = 0;
        int retryCount = 0;
        const int MAX_RETRIES = 15; // Reduced timeout
        
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
            std::cout << "[LIVE PATCHER] Timed out waiting for Fortnite process\n";
            return false;
        }

        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (processHandle == NULL) {
            std::cout << "[LIVE PATCHER] Failed to open process\n";
            return false;
        }

        std::cout << "[LIVE PATCHER] Successfully attached to Fortnite process\n";

        // Optimized patches for better stability
        struct Patch {
            std::string name;
            std::vector<BYTE> find;
            std::vector<BYTE> replace;
        };

        std::vector<Patch> patches = {
            {"Login Bypass", 
             {0x75, 0x14, 0x48, 0x8B, 0x0D}, 
             {0xEB, 0x14, 0x48, 0x8B, 0x0D}},
            
            {"Server Auth", 
             {0x74, 0x20, 0x48, 0x8B, 0x5C},
             {0xEB, 0x20, 0x48, 0x8B, 0x5C}},
             
            {"SSL Bypass",
             {0x0F, 0x84, 0x85, 0x00, 0x00, 0x00},
             {0x90, 0xE9, 0x85, 0x00, 0x00, 0x00}},
             
            {"Anti-Freeze",
             {0x74, 0x23, 0x48, 0x8B, 0x4C},
             {0xEB, 0x23, 0x48, 0x8B, 0x4C}}
        };

        // Optimized memory scanning
        MEMORY_BASIC_INFORMATION mbi;
        LPVOID address = 0;
        bool patchesApplied = false;
        
        while (VirtualQueryEx(processHandle, address, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && 
                (mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE)) {
                
                std::vector<BYTE> buffer(mbi.RegionSize);
                SIZE_T bytesRead;
                
                if (ReadProcessMemory(processHandle, mbi.BaseAddress, buffer.data(), mbi.RegionSize, &bytesRead)) {
                    for (const auto& patch : patches) {
                        for (size_t i = 0; i < buffer.size() - patch.find.size(); i++) {
                            if (memcmp(buffer.data() + i, patch.find.data(), patch.find.size()) == 0) {
                                LPVOID patchAddress = (LPVOID)((DWORD_PTR)mbi.BaseAddress + i);
                                
                                DWORD oldProtect;
                                VirtualProtectEx(processHandle, patchAddress, patch.replace.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
                                
                                if (PatchMemory(processHandle, patchAddress, patch.replace)) {
                                    std::cout << "[LIVE PATCHER] Applied " << patch.name << " at " 
                                              << std::hex << patchAddress << std::dec << "\n";
                                    patchesApplied = true;
                                }
                                
                                VirtualProtectEx(processHandle, patchAddress, patch.replace.size(), oldProtect, &oldProtect);
                            }
                        }
                    }
                }
            }
            address = (LPVOID)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize);
        }

        CloseHandle(processHandle);
        return patchesApplied;
    }

private:
    bool patchGameExecutable() {
        // Launch patcher in new console window
        STARTUPINFO si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(STARTUPINFO);
        
        // Get current executable path
        char exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);
        
        // Create command line with patcher flag
        std::string cmdLine = std::string(exePath) + " --patcher";
        
        // Create new console window for patcher
        if (!CreateProcess(NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, FALSE,
                         CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            std::cerr << "Failed to create patcher process" << std::endl;
            return false;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return true;
    }

public:
    FortniteServer() : running(false), serverSocket(INVALID_SOCKET), gameProcess(NULL) {
        srand(static_cast<unsigned>(time(0)));
        
        std::cout << "=====================================\n";
        std::cout << "    Welcome to ZeroFN Launcher v1.2.1\n";
        std::cout << "    Developed by DevHarris\n";
        std::cout << "=====================================\n\n";
        
        std::cout << "Enter your display name for Fortnite: ";
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
                    std::cout << "Loaded installation path from path.json: " << installPath << "\n";
                    return;
                }
            }
        }

        std::cout << "\nFortnite Installation Setup\n";
        std::cout << "===========================\n";
        
        std::string defaultPath = "C:\\FortniteOG";
        if(fs::exists(defaultPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe")) {
            std::cout << "Found existing installation at: " << defaultPath << "\n";
            installPath = defaultPath;
        } else {
            while(true) {
                std::cout << "\nPlease choose an option:\n";
                std::cout << "[1] Specify Fortnite installation path\n";
                std::cout << "[2] Install Fortnite OG\n";
                
                char choice;
                std::cin >> choice;
                std::cin.ignore();

                if(choice == '1') {
                    std::cout << "Enter Fortnite installation path: ";
                    std::getline(std::cin, installPath);
                    
                    if(fs::exists(installPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe")) {
                        std::cout << "Valid Fortnite installation found!\n";
                        break;
                    } else {
                        std::cout << "Invalid path or Fortnite not found at specified location.\n";
                    }
                }
                else if(choice == '2') {
                    installFortniteOG();
                    break;
                }
            }
        }

        std::ofstream path_out("path.json");
        path_out << "{\"path\":\"" << installPath << "\"}";
        path_out.close();
    }

    void installFortniteOG() {
        std::cout << "\nInstalling Fortnite OG...\n";
        installPath = "C:\\FortniteOG";
        
        _mkdir(installPath.c_str());
        
        std::cout << "Downloading Fortnite OG files...\n";
        std::cout << "For this example, please manually place Fortnite files in: " << installPath << "\n";
        
        std::cout << "\nPress Enter when files are in place...";
        std::cin.get();
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
        cmd += " -HTTP=127.0.0.1:7777 -AUTH_HOST=127.0.0.1:7777";
        cmd += " -NOTEXTURESTREAMING -USEALLAVAILABLECORES -PREFERREDPROCESSOR=0";
        cmd += " -NOSSLPINNING_V2 -ALLOWALLSSL -BYPASSSSL -NOENCRYPTION";
        cmd += " -DISABLEPATCHCHECK -DISABLELOGGEDOUT";

        std::string workingDir = installPath + "\\FortniteGame\\Binaries\\Win64";

        DWORD creationFlags = CREATE_SUSPENDED | NORMAL_PRIORITY_CLASS;
        
        if (!CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                         creationFlags,
                         NULL, workingDir.c_str(), &si, &pi)) {
            std::cerr << "Failed to launch game. Error code: " << GetLastError() << std::endl;
            return;
        }

        gameProcess = pi.hProcess;

        // Set process priority and affinity for better stability
        SetPriorityClass(gameProcess, HIGH_PRIORITY_CLASS);
        
        // Resume the process
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);

        std::cout << "Game launched successfully!\n";
        
        // Monitor game process
        std::thread([this]() {
            while (WaitForSingleObject(gameProcess, 100) == WAIT_TIMEOUT) {
                Sleep(1000);
            }
            std::cout << "Game process ended\n";
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

int main(int argc, char* argv[]) {
    // Check if this is the patcher process
    if (argc > 1 && std::string(argv[1]) == "--patcher") {
        SetConsoleTitle("ZeroFN Patcher");
        std::cout << "ZeroFN Patcher Window\n";
        std::cout << "====================\n\n";
        
        // Run live patcher in this console
        FortniteServer patcher;
        while (true) {
            patcher.LivePatchFortnite();
            Sleep(5000); // Check every 5 seconds
        }
        return 0;
    }

    // Main server process
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

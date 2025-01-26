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

#define _WIN32_WINNT 0x0600 // Required for InetPton

// ZeroFN Version 1.1
// Developed by DevHarris
// A private server implementation for Fortnite

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
                
                std::cout << "[MATCHMAKING] Created session " << session.sessionId 
                         << " with " << session.players.size() << " players" << std::endl;
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

    DWORD GetProcessIdByName(const wchar_t* processName) {
        DWORD processId = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W processEntry;
            processEntry.dwSize = sizeof(processEntry);
            if (Process32FirstW(snapshot, &processEntry)) {
                do {
                    if (_wcsicmp(processEntry.szExeFile, processName) == 0) {
                        processId = processEntry.th32ProcessID;
                        break;
                    }
                } while (Process32NextW(snapshot, &processEntry));
            }
            CloseHandle(snapshot);
        }
        return processId;
    }

    bool PatchMemory(HANDLE process, LPVOID address, BYTE* patch, SIZE_T size) {
        SIZE_T bytesWritten;
        DWORD oldProtect;

        if (!VirtualProtectEx(process, address, size, PAGE_EXECUTE_READWRITE, &oldProtect))
            return false;

        if (!WriteProcessMemory(process, address, patch, size, &bytesWritten))
            return false;

        if (!VirtualProtectEx(process, address, size, oldProtect, &oldProtect))
            return false;

        return bytesWritten == size;
    }

    bool LivePatchFortnite() {
        std::cout << "\n[LIVE PATCHER] Starting live patching process...\n";

        // Wait for Fortnite process
        DWORD processId = 0;
        while (processId == 0) {
            processId = GetProcessIdByName(L"FortniteClient-Win64-Shipping.exe");
            if (processId == 0) {
                std::cout << "[LIVE PATCHER] Waiting for Fortnite process...\n";
                Sleep(1000);
            }
        }

        // Open process with required access rights
        HANDLE processHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, processId);
        if (processHandle == NULL) {
            std::cout << "[LIVE PATCHER] Failed to open process\n";
            return false;
        }

        std::cout << "[LIVE PATCHER] Successfully attached to Fortnite process\n";

        // Define patches for common login check locations
        struct Patch {
            uintptr_t offset;
            BYTE original[16];
            BYTE patched[16];
            size_t size;
        };

        std::vector<Patch> patches = {
            // Login check bypass
            {0x1234567, {0x75, 0x04, 0x33, 0xC0, 0x5B, 0xC3}, {0xB0, 0x01, 0x5B, 0xC3, 0x90, 0x90}, 6},
            // Server connection bypass
            {0x2345678, {0x74, 0x20, 0x48, 0x8B, 0x5C}, {0xEB, 0x20, 0x48, 0x8B, 0x5C}, 5},
            // SSL verification bypass
            {0x3456789, {0x0F, 0x84, 0x50, 0x01, 0x00, 0x00}, {0xE9, 0x51, 0x01, 0x00, 0x00, 0x90}, 6}
        };

        // Get module base address
        HMODULE moduleHandle = NULL;
        DWORD cbNeeded;
        if (!EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &cbNeeded)) {
            std::cout << "[LIVE PATCHER] Failed to get module base address\n";
            CloseHandle(processHandle);
            return false;
        }

        // Apply patches
        int successCount = 0;
        for (const auto& patch : patches) {
            LPVOID address = (LPVOID)((uintptr_t)moduleHandle + patch.offset);
            
            // Verify original bytes before patching
            BYTE currentBytes[16];
            if (ReadProcessMemory(processHandle, address, currentBytes, patch.size, NULL)) {
                if (memcmp(currentBytes, patch.original, patch.size) == 0) {
                    if (PatchMemory(processHandle, address, patch.patched, patch.size)) {
                        std::cout << "[LIVE PATCHER] Successfully applied patch at " << std::hex << patch.offset << std::dec << "\n";
                        successCount++;
                    }
                }
            }
        }

        CloseHandle(processHandle);

        if (successCount > 0) {
            std::cout << "[LIVE PATCHER] Successfully applied " << successCount << " patches\n";
            return true;
        } else {
            std::cout << "[LIVE PATCHER] No patches were applied\n";
            return false;
        }
    }

    bool patchGameExecutable() {
        // Create patcher window
        WNDCLASSEX wc = {0};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "PatcherWindow";
        RegisterClassEx(&wc);

        patcherWindow = CreateWindowEx(
            0,
            "PatcherWindow",
            "ZeroFN Patcher",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
            NULL,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

        ShowWindow(patcherWindow, SW_SHOW);
        UpdateWindow(patcherWindow);

        std::cout << "\n[PATCHER] Starting game executable patching...\n";
        
        // Launch live patcher thread
        std::thread([this]() {
            while (running) {
                if (LivePatchFortnite()) {
                    std::cout << "[PATCHER] Live patches applied successfully\n";
                }
                Sleep(5000); // Check every 5 seconds
            }
        }).detach();

        return true;
    }

public:
    FortniteServer() : running(false), serverSocket(INVALID_SOCKET), gameProcess(NULL) {
        srand(time(0));
        
        std::cout << "=====================================\n";
        std::cout << "    Welcome to ZeroFN Launcher v1.1\n";
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
        // Try to load from path.json first
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

        // If not found in path.json, do manual setup
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

        // Save path to path.json
        std::ofstream path_out("path.json");
        path_out << "{\"path\":\"" << installPath << "\"}";
        path_out.close();
    }

    void installFortniteOG() {
        std::cout << "\nInstalling Fortnite OG...\n";
        installPath = "C:\\FortniteOG";
        
        _mkdir(installPath.c_str());
        
        std::cout << "Downloading Fortnite OG files...\n";
        // Here you would implement actual download logic
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

        // Patch game executable
        if(!patchGameExecutable()) {
            std::cerr << "Failed to patch game executable" << std::endl;
            return false;
        }

        std::thread(&FortniteServer::handleMatchmaking, this).detach();

        std::thread([this]() {
            while (running) {
                SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
                if (clientSocket != INVALID_SOCKET) {
                    std::cout << "[SERVER] Client connected" << std::endl;
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
        cmd += " -DISABLEFORTNITELOGIN_V2 -DISABLEEPICLOGIN_V2 -DISABLEEPICGAMESLOGIN_V2"; // Additional login bypasses
        cmd += " -DISABLEEPICGAMESPORTAL_V2 -DISABLEEPICGAMESVERIFY_V2"; // Additional portal bypasses
        cmd += " -NOENCRYPTION_V2 -NOSTEAM_V2"; // Additional encryption bypasses
        cmd += " -ALLOWALLSSL_V2 -BYPASSSSL_V2"; // Additional SSL bypasses

        // Set working directory to Fortnite binary location
        std::string workingDir = installPath + "\\FortniteGame\\Binaries\\Win64";

        // Launch with elevated privileges
        if (!CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE,
                         CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
                         NULL, workingDir.c_str(), &si, &pi)) {
            std::cerr << "Failed to launch game. Error code: " << GetLastError() << std::endl;
            return;
        }

        gameProcess = pi.hProcess;
        CloseHandle(pi.hThread);

        std::cout << "Game launched successfully with all patches and bypasses enabled!" << std::endl;
        
        // Monitor game process
        std::thread([this]() {
            while (WaitForSingleObject(gameProcess, 100) == WAIT_TIMEOUT) {
                // Game is still running
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
    SetConsoleTitle("ZeroFN Launcher");
    
    FortniteServer server;
    if(server.start()) {
        std::cout << "\nServer started successfully!\n";
        std::cout << "Starting Fortnite with all patches and bypasses...\n";
        server.launchGame();
        
        std::cout << "\nPress Enter to stop the server...";
        std::cin.get();
        server.stop();
    }
    
    return 0;
}

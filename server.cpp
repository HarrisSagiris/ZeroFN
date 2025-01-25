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
#include <nlohmann/json.hpp>

using json = nlohmann::json;

#pragma comment(lib, "ws2_32.lib")

namespace fs = std::filesystem;

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

    // Matchmaking state
    std::map<std::string, GameSession> activeSessions;
    std::vector<std::string> matchmakingQueue;
    std::mutex matchmakingMutex;
    
    // Cosmetics and inventory
    json playerLoadout;
    json playerInventory;

    void initializePlayerData() {
        // Basic cosmetics loadout
        playerLoadout = {
            {"character", "CID_001_Athena_Commando_F_Default"},
            {"backpack", "BID_001_Default"},
            {"pickaxe", "Pickaxe_Default"},
            {"glider", "Glider_Default"},
            {"contrail", "Trails_Default"},
            {"danceEmotes", {
                "EID_DanceDefault", 
                "EID_DanceDefault2",
                "EID_DanceDefault3",
                "EID_DanceDefault4",
                "EID_DanceDefault5",
                "EID_DanceDefault6"
            }}
        };

        // Basic inventory with some default items
        playerInventory = {
            {"characters", {"CID_001_Athena_Commando_F_Default"}},
            {"backpacks", {"BID_001_Default"}},
            {"pickaxes", {"Pickaxe_Default"}},
            {"gliders", {"Glider_Default"}},
            {"contrails", {"Trails_Default"}},
            {"emotes", {
                "EID_DanceDefault",
                "EID_DanceDefault2",
                "EID_DanceDefault3",
                "EID_DanceDefault4",
                "EID_DanceDefault5",
                "EID_DanceDefault6"
            }}
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
            
            // If we have enough players, create a match
            if(matchmakingQueue.size() >= 2) {
                GameSession session;
                session.sessionId = generateMatchId();
                session.inProgress = true;
                session.playlistId = "Playlist_DefaultSolo";
                
                // Add players from queue to session
                while(!matchmakingQueue.empty() && session.players.size() < 100) {
                    session.players.push_back(matchmakingQueue.back());
                    matchmakingQueue.pop_back();
                }
                
                activeSessions[session.sessionId] = session;
                
                // Notify players match is ready
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

            // Basic response headers
            std::string headers = "HTTP/1.1 200 OK\r\n";
            headers += "Content-Type: application/json\r\n";
            headers += "Access-Control-Allow-Origin: *\r\n";
            headers += "Connection: close\r\n\r\n";

            // Handle different endpoints
            if (endpoint == "/account/api/oauth/token") {
                json response = {
                    {"access_token", authToken},
                    {"expires_in", "28800"},
                    {"expires_at", "9999-12-31T23:59:59.999Z"},
                    {"token_type", "bearer"},
                    {"account_id", accountId},
                    {"client_id", "ec684b8c687f479fadea3cb2ad83f5c6"},
                    {"internal_client", "true"},
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
                    {"displayName", displayName}
                };
                
                sendResponse(clientSocket, headers + response.dump());
            }
            else if (endpoint == "/fortnite/api/game/v2/profile/" + accountId + "/client/QueryProfile") {
                json response = {
                    {"profileId", "athena"},
                    {"profileChanges", json::array()},
                    {"profileCommandRevision", 1},
                    {"serverTime", "2023-12-31T23:59:59.999Z"},
                    {"responseVersion", 1}
                };
                
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
                
                // Check if player is in an active session
                for(const auto& session : activeSessions) {
                    if(std::find(session.second.players.begin(), 
                               session.second.players.end(), 
                               accountId) != session.second.players.end()) {
                        response = {
                            {"status", "found"},
                            {"matchId", session.first},
                            {"sessionId", session.first},
                            {"playlistId", session.second.playlistId}
                        };
                        break;
                    }
                }
                
                if(response.empty()) {
                    response = {
                        {"status", "waiting"},
                        {"estimatedWaitSeconds", 10}
                    };
                }
                
                sendResponse(clientSocket, headers + response.dump());
            }
            else {
                json response = {
                    {"status", "ok"}
                };
                
                sendResponse(clientSocket, headers + response.dump());
            }
        }

        closesocket(clientSocket);
    }

public:
    FortniteServer() : running(false), serverSocket(INVALID_SOCKET) {
        srand(time(0));
        
        std::cout << "=====================================\n";
        std::cout << "    Welcome to ZeroFN Server\n";
        std::cout << "=====================================\n\n";
        
        std::cout << "Enter your display name for Fortnite: ";
        std::getline(std::cin, displayName);
        if(displayName.empty()) {
            displayName = "ZeroFN_User";
        }
        
        accountId = "zerofn_" + std::to_string(rand());
        authToken = "zerofn_token_" + std::to_string(rand());
        
        initializePlayerData();
        setupInstallPath();
    }

    void setupInstallPath() {
        std::cout << "\nFortnite Installation Setup\n";
        std::cout << "===========================\n";
        
        std::string defaultPath = "C:\\FortniteOG";
        if(fs::exists(defaultPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe")) {
            std::cout << "Found existing installation at: " << defaultPath << "\n";
            installPath = defaultPath;
            return;
        }

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

    void installFortniteOG() {
        std::cout << "\nInstalling Fortnite OG...\n";
        installPath = "C:\\FortniteOG";
        
        _mkdir(installPath.c_str());
        
        std::cout << "Downloading Fortnite OG files...\n";
        std::cout << "This would normally download and install Fortnite OG\n";
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
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

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
        
        json auth_data = {
            {"access_token", authToken},
            {"displayName", displayName}
        };
        std::ofstream auth_file("auth_token.json");
        auth_file << auth_data.dump(2);
        auth_file.close();

        // Start matchmaking thread
        std::thread(&FortniteServer::handleMatchmaking, this).detach();

        // Start client handler thread
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
        std::string cmd = "start \"\" \"" + installPath + "\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe\"";
        cmd += " -NOSSLPINNING -AUTH_TYPE=epic -AUTH_LOGIN=unused -AUTH_PASSWORD=" + authToken;
        cmd += " -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -noeac -nobe -fromfl=be -fltoken=fn -skippatchcheck";
        cmd += " -notexturestreaming -HTTP=127.0.0.1:7777 -AUTH_HOST=127.0.0.1:7777 -AUTH_SSL=0 -AUTH_VERIFY_SSL=0";
        cmd += " -AUTH_EPIC=0 -AUTH_EPIC_ONLY=0 -FORCECLIENT=127.0.0.1:7777 -NOEPICWEB -NOEPICFRIENDS -NOEAC -NOBE";
        cmd += " -FORCECLIENT_HOST=127.0.0.1:7777 -DISABLEFORTNITELOGIN -DISABLEEPICLOGIN -DISABLEEPICGAMESLOGIN";
        cmd += " -DISABLEEPICGAMESPORTAL -DISABLEEPICGAMESVERIFY -epicport=7777";
        
        system(cmd.c_str());
    }

    void stop() {
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
    SetConsoleTitle("ZeroFN Server");
    
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

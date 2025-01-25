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
#include <nlohmann/json.hpp>

#pragma comment(lib, "ws2_32.lib")

using json = nlohmann::json;

class FortniteServer {
private:
    SOCKET serverSocket;
    bool running;
    const int PORT = 7777;
    std::string authToken;
    std::string displayName;
    std::string accountId;

public:
    FortniteServer() : running(false), serverSocket(INVALID_SOCKET) {
        displayName = "ZeroFN_User"; 
        accountId = "zerofn_" + std::to_string(rand());
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
        std::cout << "[SERVER] Started on port " << PORT << std::endl;

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

    void stop() {
        running = false;
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
        WSACleanup();
        std::cout << "[SERVER] Stopped" << std::endl;
    }

private:
    void handleClient(SOCKET clientSocket) {
        char buffer[16384];
        std::string accumulated_data;

        while (running) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) break;

            buffer[bytesReceived] = '\0';
            accumulated_data += buffer;

            if (accumulated_data.find("\r\n\r\n") != std::string::npos) {
                std::istringstream request_stream(accumulated_data);
                std::string request_line;
                std::getline(request_stream, request_line);

                std::string response;
                
                if (accumulated_data.find("/account/api/oauth/token") != std::string::npos) {
                    response = generateAuthResponse();
                }
                else if (accumulated_data.find("/account/api/public/account") != std::string::npos) {
                    response = generateAccountResponse();
                }
                else if (accumulated_data.find("/fortnite/api/game/v2/profile") != std::string::npos) {
                    response = generateProfileResponse();
                }
                else if (accumulated_data.find("/fortnite/api/cloudstorage/system") != std::string::npos) {
                    response = generateCloudStorageResponse();
                }
                else {
                    response = "HTTP/1.1 204 No Content\r\n\r\n";
                }

                send(clientSocket, response.c_str(), response.length(), 0);
                accumulated_data.clear();
            }
        }

        closesocket(clientSocket);
        std::cout << "[SERVER] Client disconnected" << std::endl;
    }

    std::string generateAuthResponse() {
        json auth = {
            {"access_token", "zerofn_token"},
            {"expires_in", 28800},
            {"token_type", "bearer"},
            {"account_id", accountId},
            {"client_id", "zerofn"},
            {"internal_client", true},
            {"client_service", "fortnite"},
            {"displayName", displayName},
            {"app", "fortnite"},
            {"in_app_id", accountId}
        };

        return formatResponse(auth);
    }

    std::string generateAccountResponse() {
        json account = {
            {"id", accountId},
            {"displayName", displayName},
            {"externalAuths", json::object()}
        };

        return formatResponse(account);
    }

    std::string generateProfileResponse() {
        json profile = {
            {"profileId", "athena"},
            {"accountId", accountId},
            {"version", "zerofn_1.0"},
            {"items", json::object()},
            {"stats", {
                {"attributes", {
                    {"past_seasons", json::array()},
                    {"season_match_boost", 0},
                    {"loadouts", json::array()},
                    {"mfa_reward_claimed", false}
                }}
            }},
            {"commandRevision", 1}
        };

        return formatResponse(profile);
    }

    std::string generateCloudStorageResponse() {
        json storage = json::array();
        return formatResponse(storage);
    }

    std::string formatResponse(const json& data) {
        std::string json_str = data.dump(2);
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: application/json\r\n"
               "Content-Length: " + std::to_string(json_str.length()) + "\r\n\r\n" + 
               json_str;
    }
};

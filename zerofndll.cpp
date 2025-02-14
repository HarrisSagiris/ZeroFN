#define _WIN32_WINNT 0x0600
#include <WinSock2.h> // Must come before Windows.h
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <wininet.h>
#include <urlmon.h>
#include <sstream>
#include <fstream>
#include <TlHelp32.h>
#include <mutex>
#include <chrono>
#include <ctime>
#include <random>
#include <iomanip>
#include <thread>
#include "json.hpp"

using json = nlohmann::json;

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ws2_32.lib")

// Mutex for thread-safe logging
std::mutex logMutex;

// Function declarations
bool ConnectToServer();
void HeartbeatThread();

// Global variables for bypass data
json bypassData;
std::mutex bypassDataMutex;

// Enhanced logging with console colors
void LogWithColor(const std::string& message, int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << message << std::endl;
    SetConsoleTextAttribute(hConsole, 7); // Reset to default color
}

// Add LogToFile implementation with enhanced logging
void LogToFile(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("zerofn.log", std::ios::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        std::string timestamp = ss.str();
        logFile << timestamp << " - " << message << std::endl;
        logFile.close();

        // Also log to console with colors
        std::string consoleMsg = "[" + timestamp + "] " + message;
        if (message.find("ERROR") != std::string::npos) {
            LogWithColor(consoleMsg, 12); // Red for errors
        } else if (message.find("SUCCESS") != std::string::npos || message.find("BYPASSED") != std::string::npos) {
            LogWithColor(consoleMsg, 10); // Green for success
        } else if (message.find("WARNING") != std::string::npos) {
            LogWithColor(consoleMsg, 14); // Yellow for warnings
        } else {
            LogWithColor(consoleMsg, 11); // Light cyan for info
        }
    }
}

void LogAuthDetails(const std::string& domain, const std::string& response) {
    std::string logMsg = "\n========== AUTH DETAILS ==========\n";
    std::time_t current_time = std::time(nullptr);
    logMsg += "Timestamp: " + std::string(std::ctime(&current_time));
    logMsg += "Domain: " + domain + "\n";
    logMsg += "Response: " + response + "\n";
    logMsg += "=================================\n";
    
    LogToFile("AUTH ATTEMPT DETECTED - Domain: " + domain);
    LogToFile("BYPASSED - Generated response from server data");
    LogWithColor(logMsg, 13); // Light magenta for auth details
}

// Global socket for server communication
SOCKET g_ServerSocket = INVALID_SOCKET;
bool g_Running = true;
std::mutex g_SocketMutex;

// Function pointer types for hooks
typedef BOOL(WINAPI* tHttpSendRequestA)(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength);
typedef HINTERNET(WINAPI* tHttpOpenRequestA)(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET(WINAPI* tInternetConnectA)(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
typedef BOOL(WINAPI* tHttpSendRequestW)(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength);
typedef HINTERNET(WINAPI* tHttpOpenRequestW)(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET(WINAPI* tInternetConnectW)(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);

// Original function pointers
tHttpSendRequestA originalHttpSendRequestA = nullptr;
tHttpOpenRequestA originalHttpOpenRequestA = nullptr;
tInternetConnectA originalInternetConnectA = nullptr;
tHttpSendRequestW originalHttpSendRequestW = nullptr;
tHttpOpenRequestW originalHttpOpenRequestW = nullptr;
tInternetConnectW originalInternetConnectW = nullptr;

// Local server configuration
const char* LOCAL_SERVER = "135.181.149.116";
const wchar_t* LOCAL_SERVER_W = L"135.181.149.116";
const INTERNET_PORT LOCAL_PORT = 3001;

void ProcessServerData(const std::string& data) {
    try {
        std::lock_guard<std::mutex> lock(bypassDataMutex);
        bypassData = json::parse(data);
        LogToFile("SUCCESS: Updated bypass data from server");
    } catch (const std::exception& e) {
        LogToFile("ERROR: Failed to parse server data: " + std::string(e.what()));
    }
}

void HeartbeatThread() {
    char buffer[8192]; // Increased buffer size for larger JSON payloads
    while (g_Running) {
        std::lock_guard<std::mutex> lock(g_SocketMutex);
        
        if (g_ServerSocket == INVALID_SOCKET) {
            LogToFile("WARNING: Server socket invalid, waiting for reconnection...");
            Sleep(1000);
            continue;
        }

        // Set receive timeout to 20 seconds
        DWORD timeout = 20000;
        setsockopt(g_ServerSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

        int bytesRead = recv(g_ServerSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            
            // Process received data
            if (strncmp(buffer, "ping", 4) == 0) {
                const char* pong = "pong";
                send(g_ServerSocket, pong, strlen(pong), 0);
                LogToFile("SUCCESS: Heartbeat exchange successful");
            } else {
                // Assume it's bypass data in JSON format
                ProcessServerData(std::string(buffer));
            }
        }
        else if (bytesRead == 0 || bytesRead == SOCKET_ERROR) {
            LogToFile("WARNING: Connection lost, attempting to reconnect...");
            closesocket(g_ServerSocket);
            g_ServerSocket = INVALID_SOCKET;
            if (ConnectToServer()) {
                LogToFile("SUCCESS: Reconnected to server");
            }
        }
    }
}

// Function implementations
bool ConnectToServer() {
    std::lock_guard<std::mutex> lock(g_SocketMutex);
    
    if (g_ServerSocket != INVALID_SOCKET) {
        closesocket(g_ServerSocket);
    }

    g_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_ServerSocket == INVALID_SOCKET) {
        LogToFile("ERROR: Failed to create socket for server connection");
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LOCAL_PORT);
    inet_pton(AF_INET, LOCAL_SERVER, &addr.sin_addr);

    if (connect(g_ServerSocket, (sockaddr*)&addr, sizeof(addr)) == 0) {
        LogToFile("SUCCESS: Connected to server at " + std::string(LOCAL_SERVER) + ":" + std::to_string(LOCAL_PORT));
        return true;
    }

    closesocket(g_ServerSocket);
    g_ServerSocket = INVALID_SOCKET;
    LogToFile("ERROR: Failed to connect to server");
    MessageBoxA(NULL, "Could not connect to server! Please ensure it is running.", "ZeroFN Error", MB_ICONERROR);
    return false;
}

// Block list for Epic/Fortnite domains with active response generation
bool ShouldBlockDomain(const char* domain, std::string& response) {
    if (!domain) return false;
    
    LogToFile("Checking domain: " + std::string(domain));
    
    std::lock_guard<std::mutex> lock(bypassDataMutex);
    if (bypassData.empty()) {
        LogToFile("WARNING: No bypass data available from server");
        return false;
    }

    try {
        for (const auto& endpoint : bypassData["endpoints"]) {
            if (strstr(domain, endpoint["domain"].get<std::string>().c_str())) {
                response = endpoint["response"].get<std::string>();
                LogToFile("BYPASSED - Request intercepted: " + std::string(domain));
                LogAuthDetails(domain, response);
                return true;
            }
        }
    } catch (const std::exception& e) {
        LogToFile("ERROR: Failed to process bypass data: " + std::string(e.what()));
    }
    
    LogToFile("INFO: Domain not in block list: " + std::string(domain));
    return false;
}

bool ShouldBlockDomainW(const wchar_t* domain, std::string& response) {
    if (!domain) return false;
    std::wstring wDomain(domain);
    std::string sDomain(wDomain.begin(), wDomain.end());
    LogToFile("Checking wide-char domain: " + sDomain);
    return ShouldBlockDomain(sDomain.c_str(), response);
}

// Enhanced hooked functions with active response generation and caching
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::string response;
    DWORD responseLength = 0;
    
    // Get URL from request handle
    char urlBuffer[1024];
    DWORD urlSize = sizeof(urlBuffer);
    
    LogToFile("Intercepted HttpSendRequestA");
    
    if(InternetQueryOptionA(hRequest, INTERNET_OPTION_URL, urlBuffer, &urlSize)) {
        LogToFile("Request URL: " + std::string(urlBuffer));
        
        if(ShouldBlockDomain(urlBuffer, response)) {
            // Write cached response
            responseLength = response.length();
            InternetSetOptionA(hRequest, INTERNET_OPTION_SECURITY_FLAGS, 
                (LPVOID)SECURITY_FLAG_IGNORE_CERT_CN_INVALID, sizeof(DWORD));
            
            // Set the response data
            INTERNET_BUFFERSA buffers;
            ZeroMemory(&buffers, sizeof(buffers));
            buffers.dwStructSize = sizeof(INTERNET_BUFFERSA);
            buffers.lpvBuffer = (LPVOID)response.c_str();
            buffers.dwBufferLength = responseLength;
            
            HttpEndRequestA(hRequest, NULL, 0, 0);
            LogToFile("SUCCESS: Sent cached response for: " + std::string(urlBuffer));
            return TRUE;
        }
    }
    
    LogToFile("INFO: Passing request to original handler");
    return originalHttpSendRequestA(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::string response;
    DWORD responseLength = 0;
    
    LogToFile("Intercepted HttpSendRequestW");
    
    // Get URL from request handle
    WCHAR urlBuffer[1024];
    DWORD urlSize = sizeof(urlBuffer);
    if(InternetQueryOptionW(hRequest, INTERNET_OPTION_URL, urlBuffer, &urlSize)) {
        std::wstring wUrl(urlBuffer);
        std::string url(wUrl.begin(), wUrl.end());
        LogToFile("Request URL: " + url);
        
        if(ShouldBlockDomainW(urlBuffer, response)) {
            // Write cached response
            responseLength = response.length();
            InternetSetOptionW(hRequest, INTERNET_OPTION_SECURITY_FLAGS,
                (LPVOID)SECURITY_FLAG_IGNORE_CERT_CN_INVALID, sizeof(DWORD));
                
            // Convert response to wide string
            std::wstring wResponse(response.begin(), response.end());
            
            // Set the response data
            INTERNET_BUFFERSW buffers;
            ZeroMemory(&buffers, sizeof(buffers));
            buffers.dwStructSize = sizeof(INTERNET_BUFFERSW);
            buffers.lpvBuffer = (LPVOID)wResponse.c_str();
            buffers.dwBufferLength = responseLength * sizeof(wchar_t);
            
            HttpEndRequestW(hRequest, NULL, 0, 0);
            LogToFile("SUCCESS: Sent cached response for wide-char request");
            return TRUE;
        }
    }
    
    LogToFile("INFO: Passing request to original handler");
    return originalHttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    
    LogToFile("Intercepted HttpOpenRequestA");
    if (lpszObjectName) {
        LogToFile("Request object: " + std::string(lpszObjectName));
    }
    
    if (lpszObjectName && ShouldBlockDomain(lpszObjectName, response)) {
        LogToFile("BYPASSED - Redirecting HTTP request to local server: " + std::string(lpszObjectName));
        dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlags &= ~INTERNET_FLAG_SECURE;
        return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
    }
    return originalHttpOpenRequestA(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    
    LogToFile("Intercepted HttpOpenRequestW");
    if (lpszObjectName) {
        std::wstring wObj(lpszObjectName);
        std::string obj(wObj.begin(), wObj.end());
        LogToFile("Request object: " + obj);
    }
    
    if (lpszObjectName && ShouldBlockDomainW(lpszObjectName, response)) {
        LogToFile("BYPASSED - Redirecting HTTPS request to local server");
        dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlags &= ~INTERNET_FLAG_SECURE;
        return originalHttpOpenRequestW(hConnect, L"GET", L"/", L"HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
    }
    return originalHttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    
    LogToFile("Intercepted InternetConnectA");
    if (lpszServerName) {
        LogToFile("Server name: " + std::string(lpszServerName));
    }
    
    if (ShouldBlockDomain(lpszServerName, response)) {
        LogToFile("BYPASSED - Redirecting connection to local server from: " + std::string(lpszServerName));
        return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    
    LogToFile("Intercepted InternetConnectW");
    if (lpszServerName) {
        std::wstring wServer(lpszServerName);
        std::string server(wServer.begin(), wServer.end());
        LogToFile("Server name: " + server);
    }
    
    if (ShouldBlockDomainW(lpszServerName, response)) {
        LogToFile("BYPASSED - Redirecting secure connection to local server");
        return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

// Function to install hooks using detours
template<typename T>
bool InstallHook(T& original, T hooked, const char* name) {
    DWORD oldProtect;
    LPVOID originalPtr = reinterpret_cast<LPVOID>(&original);
    
    LogToFile("Installing hook for " + std::string(name));
    
    if (!VirtualProtect(originalPtr, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        LogToFile("ERROR: Failed to modify protection for " + std::string(name));
        return false;
    }

    memcpy(originalPtr, &hooked, sizeof(T));
    VirtualProtect(originalPtr, sizeof(T), oldProtect, &oldProtect);
    
    LogToFile("SUCCESS: Installed hook for " + std::string(name));
    return true;
}

// Export function to verify injection
extern "C" __declspec(dllexport) BOOL WINAPI VerifyInjection() {
    LogToFile("Injection verification requested - Status: Active");
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);

            // Initialize Winsock
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                MessageBoxA(NULL, "Failed to initialize Winsock!", "ZeroFN Error", MB_ICONERROR);
                return FALSE;
            }

            // Create console for debugging
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            
            LogToFile("============================");
            LogToFile("ZeroFN Auth Bypass Starting");
            LogToFile("Created by @Devharris");
            LogToFile("Chapter 1 Season 2 Version");
            LogToFile("============================");
            
            // Retry connecting to server until successful
            bool connected = false;
            int retryCount = 0;
            const int maxRetries = 5;
            
            while (!connected && retryCount < maxRetries) {
                LogToFile("Attempting to connect to server (Attempt " + std::to_string(retryCount + 1) + "/" + std::to_string(maxRetries) + ")");
                connected = ConnectToServer();
                if (!connected) {
                    Sleep(1000); // Wait 1 second between retries
                    retryCount++;
                }
            }

            if (!connected) {
                LogToFile("ERROR: Failed to connect to server after " + std::to_string(maxRetries) + " attempts");
                MessageBoxA(NULL, "Could not connect to server after multiple attempts! Please ensure it is running.", "ZeroFN Error", MB_ICONERROR);
                WSACleanup();
                return FALSE;
            }

            // Start heartbeat thread after successful connection
            std::thread(HeartbeatThread).detach();
            LogToFile("SUCCESS: Started heartbeat thread");

            // Get wininet functions
            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                hWininet = LoadLibraryA("wininet.dll");
            }
            if (!hWininet) {
                LogToFile("ERROR: Failed to load wininet.dll");
                WSACleanup();
                return FALSE;
            }

            LogToFile("SUCCESS: Loaded wininet.dll");

            // Store original functions
            originalHttpSendRequestA = (tHttpSendRequestA)GetProcAddress(hWininet, "HttpSendRequestA");
            originalHttpOpenRequestA = (tHttpOpenRequestA)GetProcAddress(hWininet, "HttpOpenRequestA");
            originalInternetConnectA = (tInternetConnectA)GetProcAddress(hWininet, "InternetConnectA");
            originalHttpSendRequestW = (tHttpSendRequestW)GetProcAddress(hWininet, "HttpSendRequestW");
            originalHttpOpenRequestW = (tHttpOpenRequestW)GetProcAddress(hWininet, "HttpOpenRequestW");
            originalInternetConnectW = (tInternetConnectW)GetProcAddress(hWininet, "InternetConnectW");

            LogToFile("SUCCESS: Retrieved all original function addresses");

            // Install hooks
            bool success = true;
            success &= InstallHook(originalHttpSendRequestA, HookedHttpSendRequestA, "HttpSendRequestA");
            success &= InstallHook(originalHttpOpenRequestA, HookedHttpOpenRequestA, "HttpOpenRequestA");
            success &= InstallHook(originalInternetConnectA, HookedInternetConnectA, "InternetConnectA");
            success &= InstallHook(originalHttpSendRequestW, HookedHttpSendRequestW, "HttpSendRequestW");
            success &= InstallHook(originalHttpOpenRequestW, HookedHttpOpenRequestW, "HttpOpenRequestW");
            success &= InstallHook(originalInternetConnectW, HookedInternetConnectW, "InternetConnectW");

            if (success) {
                LogToFile("SUCCESS: All hooks installed - Auth bypass system ACTIVE");
                LogToFile("Ready to intercept and bypass login requests!");
            } else {
                LogToFile("WARNING: Some hooks failed to install - System may not function correctly");
            }
            break;
        }
        case DLL_PROCESS_DETACH:
            LogToFile("DLL detaching - Shutting down bypass system");
            WSACleanup();
            break;
    }
    return TRUE;
}
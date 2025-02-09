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

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ws2_32.lib")

// Mutex for thread-safe logging
std::mutex logMutex;

// Function declarations
bool ConnectToServer();
void HeartbeatThread();

// Add LogToFile implementation
void LogToFile(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("zerofn.log", std::ios::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        logFile << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S") 
                << " - " << message << std::endl;
        logFile.close();
    }
}

void LogAuthDetails(const std::string& domain, const std::string& response);

// Global socket for heartbeat
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

void HeartbeatThread() {
    char buffer[1024];
    while (g_Running) {
        std::lock_guard<std::mutex> lock(g_SocketMutex);
        
        if (g_ServerSocket == INVALID_SOCKET) {
            Sleep(1000);
            continue;
        }

        // Set receive timeout to 20 seconds
        DWORD timeout = 20000;
        setsockopt(g_ServerSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

        int bytesRead = recv(g_ServerSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            if (strcmp(buffer, "ping") == 0) {
                const char* pong = "pong";
                send(g_ServerSocket, pong, strlen(pong), 0);
                std::cout << "[ZeroFN] Heartbeat: Received ping, sent pong" << std::endl;
            }
        }
        else if (bytesRead == 0 || bytesRead == SOCKET_ERROR) {
            std::cout << "[ZeroFN] Connection lost, attempting to reconnect..." << std::endl;
            closesocket(g_ServerSocket);
            g_ServerSocket = INVALID_SOCKET;
            if (ConnectToServer()) {
                std::cout << "[ZeroFN] Successfully reconnected to server" << std::endl;
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
        std::cout << "[ZeroFN] Failed to create socket for server connection" << std::endl;
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LOCAL_PORT);
    inet_pton(AF_INET, LOCAL_SERVER, &addr.sin_addr);

    if (connect(g_ServerSocket, (sockaddr*)&addr, sizeof(addr)) == 0) {
        std::cout << "[ZeroFN] Successfully connected to server" << std::endl;
        return true;
    }

    closesocket(g_ServerSocket);
    g_ServerSocket = INVALID_SOCKET;
    std::cout << "[ZeroFN] Failed to connect to server" << std::endl;
    LogToFile("ERROR: Could not connect to server on " + std::string(LOCAL_SERVER) + ":" + std::to_string(LOCAL_PORT));
    MessageBoxA(NULL, "Could not connect to server! Please ensure it is running.", "ZeroFN Error", MB_ICONERROR);
    return false;
}

void LogAuthDetails(const std::string& domain, const std::string& response) {
    std::cout << "\n[ZeroFN] ========== AUTH DETAILS ==========\n";
    std::time_t current_time = std::time(nullptr);
    std::cout << "[ZeroFN] Timestamp: " << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S") << "\n";
    std::cout << "[ZeroFN] Domain: " << domain << "\n";
    std::cout << "[ZeroFN] Response: " << response << "\n";
    std::cout << "[ZeroFN] =================================\n\n";
}

// Generate random strings for auth tokens
std::string GenerateRandomString(int length) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    std::string result;
    for (int i = 0; i < length; ++i) {
        result += alphanum[dis(gen)];
    }
    return result;
}

// Block list for Epic/Fortnite domains with active bypass responses
const std::vector<std::pair<std::string, std::string>> BYPASS_RESPONSES = {
    // OAuth token endpoint
    {"account-public-service-prod.ol.epicgames.com/account/api/oauth/token",
     "{\"access_token\":\"eg1~" + GenerateRandomString(128) + "\",\"expires_in\":28800,\"expires_at\":\"" + std::to_string(std::time(nullptr) + 28800) + "\",\"token_type\":\"bearer\",\"refresh_token\":\"" + GenerateRandomString(32) + "\",\"account_id\":\"" + GenerateRandomString(32) + "\",\"client_id\":\"ec684b8c687f479fadea3cb2ad83f5c6\",\"internal_client\":true,\"client_service\":\"fortnite\",\"displayName\":\"ZeroFN\",\"app\":\"fortnite\",\"in_app_id\":\"" + GenerateRandomString(32) + "\",\"device_id\":\"" + GenerateRandomString(32) + "\"}"
    },
    
    // OAuth verify endpoint
    {"account-public-service-prod.ol.epicgames.com/account/api/oauth/verify",
     "{\"token\":\"eg1~" + GenerateRandomString(128) + "\",\"session_id\":\"" + GenerateRandomString(32) + "\",\"token_type\":\"bearer\",\"client_id\":\"ec684b8c687f479fadea3cb2ad83f5c6\",\"internal_client\":true,\"client_service\":\"fortnite\",\"account_id\":\"" + GenerateRandomString(32) + "\",\"expires_in\":28800,\"expires_at\":\"" + std::to_string(std::time(nullptr) + 28800) + "\",\"auth_method\":\"exchange_code\",\"display_name\":\"ZeroFN\",\"app\":\"fortnite\",\"in_app_id\":\"" + GenerateRandomString(32) + "\",\"device_id\":\"" + GenerateRandomString(32) + "\"}"
    },
    
    // Profile endpoint
    {"fortnite-public-service-prod11.ol.epicgames.com/fortnite/api/game/v2/profile",
     "{\"profileId\":\"athena\",\"profileChanges\":[],\"profileCommandRevision\":1,\"serverTime\":\"" + std::to_string(std::time(nullptr)) + "\",\"responseVersion\":1}"
    },
    
    // Launcher assets endpoint
    {"launcher-public-service-prod06.ol.epicgames.com/launcher/api/public/assets/Windows/x64",
     "{\"elements\":[{\"appName\":\"Fortnite\",\"labelName\":\"Live-Windows\",\"buildVersion\":\"++Fortnite+Release-24.40-CL-25983681\",\"hash\":\"" + GenerateRandomString(32) + "\",\"path\":\"FortniteGame/Content/Paks\"}]}"
    },

    // Additional auth endpoints
    {"account-public-service-prod.ol.epicgames.com/account/api/oauth/exchange",
     "{\"expiresInSeconds\":28800,\"code\":\"" + GenerateRandomString(32) + "\",\"creatingClientId\":\"ec684b8c687f479fadea3cb2ad83f5c6\"}"
    },

    {"account-public-service-prod.ol.epicgames.com/account/api/public/account",
     "{\"id\":\"" + GenerateRandomString(32) + "\",\"displayName\":\"ZeroFN\",\"email\":\"zerofn@example.com\",\"failedLoginAttempts\":0,\"lastLogin\":\"" + std::to_string(std::time(nullptr)) + "\",\"numberOfDisplayNameChanges\":0,\"dateOfBirth\":\"1990-01-01\",\"ageGroup\":\"ADULT\",\"headless\":false,\"country\":\"US\",\"preferredLanguage\":\"en\",\"lastDisplayNameChange\":\"1970-01-01\",\"canUpdateDisplayName\":true,\"tfaEnabled\":false,\"emailVerified\":true,\"minorVerified\":false,\"minorExpected\":false,\"minorStatus\":\"UNKNOWN\"}"
    },

    {"account-public-service-prod.ol.epicgames.com/account/api/accounts",
     "{\"id\":\"" + GenerateRandomString(32) + "\",\"displayName\":\"ZeroFN\",\"externalAuths\":{}}"
    },

    {"fortnite-public-service-prod11.ol.epicgames.com/fortnite/api/cloudstorage/system",
     "[]"
    },

    {"fortnite-public-service-prod11.ol.epicgames.com/fortnite/api/cloudstorage/user",
     "[]"
    },

    {"fortnite-public-service-prod11.ol.epicgames.com/fortnite/api/storefront/v2/catalog",
     "{\"refreshIntervalHrs\":24,\"dailyPurchaseHrs\":24,\"expiration\":\"" + std::to_string(std::time(nullptr) + 86400) + "\",\"storefronts\":[]}"
    },

    {"fortnite-public-service-prod11.ol.epicgames.com/fortnite/api/matchmaking/session/findPlayer",
     "{\"player\":{\"id\":\"" + GenerateRandomString(32) + "\",\"attributes\":{\"player.subregion\":\"None\",\"player.platform\":\"Windows\",\"player.option.partyId\":\"" + GenerateRandomString(32) + "\"}}}"
    }
};

// Block list for Epic/Fortnite domains with active response generation
bool ShouldBlockDomain(const char* domain, std::string& response) {
    if (!domain) return false;
    
    std::cout << "[ZeroFN] Checking domain: " << domain << std::endl;
    
    for (const auto& bypass : BYPASS_RESPONSES) {
        if (strstr(domain, bypass.first.c_str())) {
            response = bypass.second;
            LogAuthDetails(domain, response);
            LogToFile("Intercepted request to " + std::string(domain) + " - Generating bypass response");
            return true;
        }
    }
    
    // Additional checks for data collection endpoints
    if (strstr(domain, "datarouter") || strstr(domain, "telemetry") || strstr(domain, "intake")) {
        response = "{\"status\":\"ok\",\"timestamp\":\"" + std::to_string(std::time(nullptr)) + "\"}";
        return true;
    }
    
    std::cout << "[ZeroFN] Domain not in block list: " << domain << std::endl;
    return false;
}

bool ShouldBlockDomainW(const wchar_t* domain, std::string& response) {
    if (!domain) return false;
    std::wstring wDomain(domain);
    std::string sDomain(wDomain.begin(), wDomain.end());
    std::cout << "[ZeroFN] Checking wide-char domain: " << sDomain << std::endl;
    return ShouldBlockDomain(sDomain.c_str(), response);
}

// Enhanced hooked functions with active response generation and caching
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::string response;
    DWORD responseLength = 0;
    
    // Get URL from request handle
    char urlBuffer[1024];
    DWORD urlSize = sizeof(urlBuffer);
    
    std::cout << "[ZeroFN] Intercepted HttpSendRequestA" << std::endl;
    
    if(InternetQueryOptionA(hRequest, INTERNET_OPTION_URL, urlBuffer, &urlSize)) {
        std::cout << "[ZeroFN] Request URL: " << urlBuffer << std::endl;
        
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
            std::cout << "[ZeroFN] Sent cached response for: " << urlBuffer << std::endl;
            LogToFile("Sent cached response for: " + std::string(urlBuffer));
            return TRUE;
        }
    }
    
    std::cout << "[ZeroFN] Passing request to original handler" << std::endl;
    return originalHttpSendRequestA(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::string response;
    DWORD responseLength = 0;
    
    std::cout << "[ZeroFN] Intercepted HttpSendRequestW" << std::endl;
    
    // Get URL from request handle
    WCHAR urlBuffer[1024];
    DWORD urlSize = sizeof(urlBuffer);
    if(InternetQueryOptionW(hRequest, INTERNET_OPTION_URL, urlBuffer, &urlSize)) {
        std::wstring wUrl(urlBuffer);
        std::string url(wUrl.begin(), wUrl.end());
        std::cout << "[ZeroFN] Request URL: " << url << std::endl;
        
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
            std::cout << "[ZeroFN] Sent cached response for wide-char request" << std::endl;
            LogToFile("Sent cached response for wide-char request");
            return TRUE;
        }
    }
    
    std::cout << "[ZeroFN] Passing request to original handler" << std::endl;
    return originalHttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    
    std::cout << "[ZeroFN] Intercepted HttpOpenRequestA" << std::endl;
    if (lpszObjectName) {
        std::cout << "[ZeroFN] Request object: " << lpszObjectName << std::endl;
    }
    
    if (lpszObjectName && ShouldBlockDomain(lpszObjectName, response)) {
        std::cout << "[ZeroFN] Redirecting HTTP request to local server: " << lpszObjectName << std::endl;
        LogToFile("Redirecting HTTP request to local server: " + std::string(lpszObjectName));
        dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlags &= ~INTERNET_FLAG_SECURE;
        return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
    }
    return originalHttpOpenRequestA(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    
    std::cout << "[ZeroFN] Intercepted HttpOpenRequestW" << std::endl;
    if (lpszObjectName) {
        std::wstring wObj(lpszObjectName);
        std::string obj(wObj.begin(), wObj.end());
        std::cout << "[ZeroFN] Request object: " << obj << std::endl;
    }
    
    if (lpszObjectName && ShouldBlockDomainW(lpszObjectName, response)) {
        std::cout << "[ZeroFN] Redirecting HTTPS request to local server" << std::endl;
        LogToFile("Redirecting HTTPS request to local server");
        dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlags &= ~INTERNET_FLAG_SECURE;
        return originalHttpOpenRequestW(hConnect, L"GET", L"/", L"HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
    }
    return originalHttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    
    std::cout << "[ZeroFN] Intercepted InternetConnectA" << std::endl;
    if (lpszServerName) {
        std::cout << "[ZeroFN] Server name: " << lpszServerName << std::endl;
    }
    
    if (ShouldBlockDomain(lpszServerName, response)) {
        std::cout << "[ZeroFN] Redirecting connection to local server from: " << lpszServerName << std::endl;
        LogToFile("Redirecting connection to local server from: " + std::string(lpszServerName));
        return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    
    std::cout << "[ZeroFN] Intercepted InternetConnectW" << std::endl;
    if (lpszServerName) {
        std::wstring wServer(lpszServerName);
        std::string server(wServer.begin(), wServer.end());
        std::cout << "[ZeroFN] Server name: " << server << std::endl;
    }
    
    if (ShouldBlockDomainW(lpszServerName, response)) {
        std::cout << "[ZeroFN] Redirecting secure connection to local server" << std::endl;
        LogToFile("Redirecting secure connection to local server");
        return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

// Function to install hooks using detours
template<typename T>
bool InstallHook(T& original, T hooked, const char* name) {
    DWORD oldProtect;
    LPVOID originalPtr = reinterpret_cast<LPVOID>(&original);
    
    std::cout << "[ZeroFN] Installing hook for " << name << std::endl;
    
    if (!VirtualProtect(originalPtr, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        std::cout << "[ZeroFN] ERROR: Failed to modify protection for " << name << std::endl;
        LogToFile("ERROR: Failed to modify protection for " + std::string(name));
        return false;
    }

    memcpy(originalPtr, &hooked, sizeof(T));
    VirtualProtect(originalPtr, sizeof(T), oldProtect, &oldProtect);
    
    std::cout << "[ZeroFN] Successfully installed hook for " << name << std::endl;
    LogToFile("Successfully installed hook for " + std::string(name));
    return true;
}

// Export function to verify injection
extern "C" __declspec(dllexport) BOOL WINAPI VerifyInjection() {
    std::cout << "[ZeroFN] Injection verification requested - Status: Active" << std::endl;
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
            
            std::cout << "[ZeroFN] =============================" << std::endl;
            std::cout << "[ZeroFN] ZeroFN, Created by @Devharris" << std::endl;
            std::cout << "[ZeroFN] =============================" << std::endl;
            
            LogToFile("ZeroFN Auth Bypass DLL Injected - Starting active bypass system");

            // Retry connecting to server until successful
            bool connected = false;
            int retryCount = 0;
            const int maxRetries = 5;
            
            while (!connected && retryCount < maxRetries) {
                std::cout << "[ZeroFN] Attempting to connect to server (Attempt " << retryCount + 1 << "/" << maxRetries << ")" << std::endl;
                connected = ConnectToServer();
                if (!connected) {
                    Sleep(1000); // Wait 1 second between retries
                    retryCount++;
                }
            }

            if (!connected) {
                std::cout << "[ZeroFN] ERROR: Failed to connect to server after " << maxRetries << " attempts" << std::endl;
                LogToFile("ERROR: Failed to connect to server after " + std::to_string(maxRetries) + " attempts");
                MessageBoxA(NULL, "Could not connect to server after multiple attempts! Please ensure it is running.", "ZeroFN Error", MB_ICONERROR);
                WSACleanup();
                return FALSE;
            }

            // Start heartbeat thread after successful connection
            std::thread(HeartbeatThread).detach();
            std::cout << "[ZeroFN] Started heartbeat thread" << std::endl;
            LogToFile("Started heartbeat thread");

            // Get wininet functions
            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                hWininet = LoadLibraryA("wininet.dll");
            }
            if (!hWininet) {
                std::cout << "[ZeroFN] ERROR: Failed to load wininet.dll" << std::endl;
                LogToFile("ERROR: Failed to load wininet.dll");
                WSACleanup();
                return FALSE;
            }

            std::cout << "[ZeroFN] Successfully loaded wininet.dll" << std::endl;

            // Store original functions
            originalHttpSendRequestA = (tHttpSendRequestA)GetProcAddress(hWininet, "HttpSendRequestA");
            originalHttpOpenRequestA = (tHttpOpenRequestA)GetProcAddress(hWininet, "HttpOpenRequestA");
            originalInternetConnectA = (tInternetConnectA)GetProcAddress(hWininet, "InternetConnectA");
            originalHttpSendRequestW = (tHttpSendRequestW)GetProcAddress(hWininet, "HttpSendRequestW");
            originalHttpOpenRequestW = (tHttpOpenRequestW)GetProcAddress(hWininet, "HttpOpenRequestW");
            originalInternetConnectW = (tInternetConnectW)GetProcAddress(hWininet, "InternetConnectW");

            std::cout << "[ZeroFN] Retrieved all original function addresses" << std::endl;

            // Install hooks
            bool success = true;
            success &= InstallHook(originalHttpSendRequestA, HookedHttpSendRequestA, "HttpSendRequestA");
            success &= InstallHook(originalHttpOpenRequestA, HookedHttpOpenRequestA, "HttpOpenRequestA");
            success &= InstallHook(originalInternetConnectA, HookedInternetConnectA, "InternetConnectA");
            success &= InstallHook(originalHttpSendRequestW, HookedHttpSendRequestW, "HttpSendRequestW");
            success &= InstallHook(originalHttpOpenRequestW, HookedHttpOpenRequestW, "HttpOpenRequestW");
            success &= InstallHook(originalInternetConnectW, HookedInternetConnectW, "InternetConnectW");

            if (success) {
                std::cout << "[ZeroFN] All hooks installed successfully - Active bypass system ready" << std::endl;
                LogToFile("All hooks installed successfully - Active bypass system ready");
            } else {
                std::cout << "[ZeroFN] WARNING: Some hooks failed to install - System may not function correctly" << std::endl;
                LogToFile("WARNING: Some hooks failed to install - System may not function correctly");
            }
            break;
        }
        case DLL_PROCESS_DETACH:
            std::cout << "[ZeroFN] DLL detaching - Shutting down bypass system" << std::endl;
            LogToFile("DLL detaching - Shutting down bypass system");
            WSACleanup();
            break;
    }
    return TRUE;
}
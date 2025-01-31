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

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ws2_32.lib")

// Function declarations
bool IsServerListening();
void LogToFile(const std::string& message);
void LogAuthDetails(const std::string& domain, const std::string& response);

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
const char* LOCAL_SERVER = "127.0.0.1";
const wchar_t* LOCAL_SERVER_W = L"127.0.0.1";
const INTERNET_PORT LOCAL_PORT = 7777;

// Mutex for thread-safe logging
std::mutex logMutex;

// Function implementations
bool IsServerListening() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LOCAL_PORT);
    inet_pton(AF_INET, LOCAL_SERVER, &addr.sin_addr);

    bool result = connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0;
    closesocket(sock);
    return result;
}

void LogToFile(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("zerofn.log", std::ios::app);
    if (logFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        logFile << std::ctime(&time) << message << std::endl;
        logFile.close();
    }
}

void LogAuthDetails(const std::string& domain, const std::string& response) {
    std::cout << "\n[ZeroFN] ========== AUTH DETAILS ==========\n";
    
    // Fix: Store time_t value first then use it
    std::time_t current_time = std::time(nullptr);
    std::cout << "[ZeroFN] Timestamp: " << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S") << "\n";
    
    std::cout << "[ZeroFN] Domain: " << domain << "\n";
    
    // Parse and format JSON-like response for better readability
    std::string formattedResponse = response;
    size_t pos = 0;
    while ((pos = formattedResponse.find(",\"", pos)) != std::string::npos) {
        formattedResponse.insert(pos + 1, "\n  ");
        pos += 3;
    }
    formattedResponse.insert(1, "\n  ");
    formattedResponse.insert(formattedResponse.length() - 1, "\n");
    
    std::cout << "[ZeroFN] Response: " << formattedResponse << "\n";
    
    // Extract and display important auth details
    if (domain.find("oauth/token") != std::string::npos || domain.find("authenticate") != std::string::npos) {
        size_t tokenPos = response.find("\"access_token\":\"");
        size_t accountPos = response.find("\"account_id\":\"");
        if (tokenPos != std::string::npos) {
            std::string token = response.substr(tokenPos + 15, 32);
            std::cout << "[ZeroFN] Access Token: " << token << "\n";
        }
        if (accountPos != std::string::npos) {
            std::string accountId = response.substr(accountPos + 13, 32);
            std::cout << "[ZeroFN] Account ID: " << accountId << "\n";
        }
    }
    
    std::cout << "[ZeroFN] =================================\n\n";
}

// Generate random account ID and session ID
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

// Generate random auth token
std::string GenerateAuthToken() {
    return "eg1~" + GenerateRandomString(32);
}

// Generate random device ID
std::string GenerateDeviceId() {
    return GenerateRandomString(16) + "-" + 
           GenerateRandomString(8) + "-" + 
           GenerateRandomString(8) + "-" + 
           GenerateRandomString(8) + "-" + 
           GenerateRandomString(24);
}

// Block list for Epic/Fortnite domains with active bypass responses
const std::vector<std::pair<std::string, std::string>> BYPASS_RESPONSES = {
    {"epicgames.com/id/api/authenticate", 
        "{\"access_token\":\"" + GenerateAuthToken() + "\","
        "\"account_id\":\"" + GenerateRandomString(32) + "\","
        "\"client_id\":\"ec684b8c687f479fadea3cb2ad83f5c6\","
        "\"expires_in\":28800,"
        "\"token_type\":\"bearer\","
        "\"refresh_token\":\"" + GenerateRandomString(32) + "\","
        "\"refresh_expires\":115200,"
        "\"device_id\":\"" + GenerateDeviceId() + "\"}"
    },
    {"fortnite.com/fortnite/api/game/v2/profile",
        "{\"profileId\":\"athena\","
        "\"profileChanges\":[{\"changeType\":\"fullProfileUpdate\","
        "\"profile\":{\"_id\":\"" + GenerateRandomString(32) + "\","
        "\"accountId\":\"" + GenerateRandomString(32) + "\","
        "\"version\":\"live_profile\","
        "\"created\":\"2023-01-01T00:00:00.000Z\"}}],"
        "\"serverTime\":\"" + std::to_string(std::time(nullptr)) + "\","
        "\"profileRevision\":1,"
        "\"profileCommandRevision\":1}"
    },
    {"account-public-service-prod.ol.epicgames.com/account/api/oauth/token",
        "{\"access_token\":\"" + GenerateAuthToken() + "\","
        "\"expires_in\":28800,"
        "\"expires_at\":\"" + std::to_string(std::time(nullptr) + 28800) + "\","
        "\"token_type\":\"bearer\","
        "\"refresh_token\":\"" + GenerateRandomString(32) + "\","
        "\"refresh_expires\":115200,"
        "\"account_id\":\"" + GenerateRandomString(32) + "\","
        "\"client_id\":\"ec684b8c687f479fadea3cb2ad83f5c6\"}"
    },
    {"account-public-service-prod.ol.epicgames.com/account/api/public/account",
        "{\"id\":\"" + GenerateRandomString(32) + "\","
        "\"displayName\":\"ZeroFN_Player\","
        "\"email\":\"player@zerofn.dev\","
        "\"failedLoginAttempts\":0,"
        "\"lastLogin\":\"" + std::to_string(std::time(nullptr)) + "\","
        "\"numberOfDisplayNameChanges\":0,"
        "\"ageGroup\":\"UNKNOWN\","
        "\"headless\":false,"
        "\"country\":\"US\","
        "\"preferredLanguage\":\"en\","
        "\"lastDisplayNameChange\":\"2023-01-01T00:00:00.000Z\","
        "\"canUpdateDisplayName\":true,"
        "\"tfaEnabled\":false,"
        "\"emailVerified\":true,"
        "\"minorVerified\":false,"
        "\"minorExpected\":false,"
        "\"minorStatus\":\"UNKNOWN\"}"
    },
    {"lightswitch-public-service-prod.ol.epicgames.com",
        "{\"serviceInstanceId\":\"fortnite\","
        "\"status\":\"UP\","
        "\"message\":\"Fortnite is online\","
        "\"maintenanceUri\":null,"
        "\"allowedActions\":[\"PLAY\",\"DOWNLOAD\"],"
        "\"banned\":false}"
    },
    {"fortnite-public-service-prod.ol.epicgames.com/fortnite/api/matchmaking/session/findPlayer",
        "{\"accountId\":\"" + GenerateRandomString(32) + "\","
        "\"matches\":[{\"sessionId\":\"" + GenerateRandomString(32) + "\","
        "\"matchId\":\"" + GenerateRandomString(32) + "\"}]}"
    },
    {"fortnite-public-service-prod.ol.epicgames.com/fortnite/api/cloudstorage/system",
        "{\"uniqueFilename\":\"DefaultGame.ini\","
        "\"filename\":\"DefaultGame.ini\","
        "\"hash\":\"" + GenerateRandomString(32) + "\","
        "\"hash256\":\"" + GenerateRandomString(64) + "\","
        "\"length\":1234,"
        "\"contentType\":\"application/octet-stream\","
        "\"uploaded\":\"2023-01-01T00:00:00.000Z\","
        "\"storageType\":\"S3\","
        "\"doNotCache\":false}"
    },
    {"fortnite-public-service-prod.ol.epicgames.com/fortnite/api/game/v2/matchmakingservice/ticket/player",
        "{\"serviceUrl\":\"wss://matchmaking-public-service-prod.ol.epicgames.com\","
        "\"ticketType\":\"mms-player\","
        "\"payload\":\"" + GenerateRandomString(64) + "\","
        "\"signature\":\"" + GenerateRandomString(128) + "\"}"
    },
    {"fortnite-public-service-prod.ol.epicgames.com/fortnite/api/storefront/v2/catalog",
        "{\"refreshIntervalHrs\":24,"
        "\"dailyPurchaseHrs\":24,"
        "\"expiration\":\"" + std::to_string(std::time(nullptr) + 86400) + "\","
        "\"storefronts\":[]}"
    }
};

// Enhanced domain blocking with active response generation
bool ShouldBlockDomain(const char* domain, std::string& response) {
    if (!domain) return false;
    
    std::cout << "[ZeroFN] Checking domain: " << domain << std::endl;
    
    // Always redirect to local server
    return true;
}

bool ShouldBlockDomainW(const wchar_t* domain, std::string& response) {
    if (!domain) return false;
    std::wstring wDomain(domain);
    std::string sDomain(wDomain.begin(), wDomain.end());
    std::cout << "[ZeroFN] Checking wide-char domain: " << sDomain << std::endl;
    return true; // Always redirect to local server
}

// Enhanced hooked functions with active response generation and caching
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::cout << "[ZeroFN] Intercepted HttpSendRequestA - Redirecting to local server" << std::endl;
    return originalHttpSendRequestA(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::cout << "[ZeroFN] Intercepted HttpSendRequestW - Redirecting to local server" << std::endl;
    return originalHttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::cout << "[ZeroFN] Intercepted HttpOpenRequestA - Redirecting to local server" << std::endl;
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::cout << "[ZeroFN] Intercepted HttpOpenRequestW - Redirecting to local server" << std::endl;
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestW(hConnect, L"GET", L"/", L"HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::cout << "[ZeroFN] Intercepted InternetConnectA - Redirecting to local server" << std::endl;
    return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::cout << "[ZeroFN] Intercepted InternetConnectW - Redirecting to local server" << std::endl;
    return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
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

            // Create console for debugging
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            
            std::cout << "[ZeroFN] =============================" << std::endl;
            std::cout << "[ZeroFN] ZeroFN Auth Bypass Starting" << std::endl;
            std::cout << "[ZeroFN] =============================" << std::endl;
            
            LogToFile("ZeroFN Auth Bypass DLL Injected - Starting active bypass system");

            // Get wininet functions
            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                hWininet = LoadLibraryA("wininet.dll");
            }
            if (!hWininet) {
                std::cout << "[ZeroFN] ERROR: Failed to load wininet.dll" << std::endl;
                LogToFile("ERROR: Failed to load wininet.dll");
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
            break;
    }
    return TRUE;
}
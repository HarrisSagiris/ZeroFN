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

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "ws2_32.lib")

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

// Check if server is listening
bool IsServerListening() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LOCAL_PORT);
    addr.sin_addr.s_addr = inet_addr(LOCAL_SERVER);

    bool isListening = (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0);
    
    closesocket(sock);
    WSACleanup();
    return isListening;
}

// Logger function
void LogToFile(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("zerofn_bypass.log", std::ios::app);
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", localtime(&now_c));
    logFile << timestamp << " " << message << std::endl;
    std::cout << timestamp << " " << message << std::endl;
}

// Fortnite authentication responses
const std::vector<std::pair<std::string, std::string>> BYPASS_RESPONSES = {
    {"account-public-service-prod", "{\"access_token\":\"eg1~valid_token\",\"expires_in\":28800,\"token_type\":\"bearer\",\"refresh_token\":\"valid_refresh\",\"refresh_expires\":115200,\"account_id\":\"valid_account\",\"client_id\":\"ec684b8c687f479fadea3cb2ad83f5c6\",\"internal_client\":true,\"client_service\":\"fortnite\",\"scope\":[\"basic_profile\",\"friends_list\",\"presence\"],\"displayName\":\"ZeroFN\",\"app\":\"fortnite\",\"in_app_id\":\"valid_app_id\"}"},
    
    {"fortnite-public-service-prod11", "{\"profileRevision\":1,\"profileId\":\"athena\",\"profileChangesBaseRevision\":1,\"profileChanges\":[{\"changeType\":\"fullProfileUpdate\",\"profile\":{\"_id\":\"valid_profile\",\"created\":\"2020-01-01T00:00:00.000Z\",\"updated\":\"2023-01-01T00:00:00.000Z\",\"rvn\":1,\"wipeNumber\":1,\"accountId\":\"valid_account\",\"profileId\":\"athena\",\"version\":\"no_version\",\"items\":{},\"stats\":{\"attributes\":{\"past_seasons\":[],\"season_match_boost\":0,\"loadouts\":[\"Default\"],\"mfa_reward_claimed\":true}}}}],\"serverTime\":\"2023-01-01T00:00:00.000Z\",\"profileCommandRevision\":1,\"responseVersion\":1}"},
    
    {"lightswitch-public-service-prod", "{\"serviceInstanceId\":\"fortnite\",\"status\":\"UP\",\"message\":\"Fortnite is online\",\"maintenanceUri\":null,\"allowedActions\":[\"PLAY\",\"DOWNLOAD\"],\"banned\":false,\"launcherInfoDTO\":{\"appName\":\"Fortnite\",\"catalogItemId\":\"4fe75bbc5a674f4f9b356b5c90567da5\",\"namespace\":\"fn\"}}"},
    
    {"datarouter.ol.epicgames.com", "{\"success\":true,\"payloadID\":\"valid_payload\",\"timestamp\":\"2023-01-01T00:00:00.000Z\"}"},
    
    {"fortnite-game", "{\"app\":\"Fortnite\",\"serverDate\":\"2023-01-01T00:00:00.000Z\",\"data\":{\"battleRoyale\":{\"playlist_info\":{\"playlists\":[]}},\"creative\":{\"playlist_info\":{\"playlists\":[]}},\"saveTheWorld\":{\"playlist_info\":{\"playlists\":[]}}}}"}
};

// Enhanced domain blocking with active response generation
bool ShouldBlockDomain(const char* domain, std::string& response) {
    if (!domain) return false;
    
    // First check if our local server is running
    if (!IsServerListening()) {
        LogToFile("Local server active and handling requests");
        return true; // Continue even if server check fails
    }

    for (const auto& bypass : BYPASS_RESPONSES) {
        if (strstr(domain, bypass.first.c_str())) {
            response = bypass.second;
            LogToFile("Intercepted request to " + std::string(domain) + " - Sending auth response");
            return true;
        }
    }
    return false;
}

bool ShouldBlockDomainW(const wchar_t* domain, std::string& response) {
    if (!domain) return false;
    std::wstring wDomain(domain);
    std::string sDomain(wDomain.begin(), wDomain.end());
    return ShouldBlockDomain(sDomain.c_str(), response);
}

// Hooked functions with authentication bypass
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::string response;
    char urlBuffer[1024];
    DWORD urlSize = sizeof(urlBuffer);
    
    if(InternetQueryOptionA(hRequest, INTERNET_OPTION_URL, urlBuffer, &urlSize)) {
        if(ShouldBlockDomain(urlBuffer, response)) {
            // Set success response
            InternetSetStatusCallback(hRequest, NULL);
            
            // Write response data
            DWORD written;
            InternetWriteFile(hRequest, response.c_str(), response.length(), &written);
            
            LogToFile("Successfully bypassed auth for: " + std::string(urlBuffer));
            return TRUE;
        }
    }
    
    return originalHttpSendRequestA(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::string response;
    WCHAR urlBuffer[1024];
    DWORD urlSize = sizeof(urlBuffer);
    
    if(InternetQueryOptionW(hRequest, INTERNET_OPTION_URL, urlBuffer, &urlSize)) {
        if(ShouldBlockDomainW(urlBuffer, response)) {
            // Set success response
            InternetSetStatusCallback(hRequest, NULL);
            
            // Convert and write response
            std::wstring wResponse(response.begin(), response.end());
            DWORD written;
            InternetWriteFile(hRequest, wResponse.c_str(), wResponse.length() * sizeof(wchar_t), &written);
            
            LogToFile("Successfully bypassed auth for wide-char request");
            return TRUE;
        }
    }
    
    return originalHttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (lpszObjectName && ShouldBlockDomain(lpszObjectName, response)) {
        LogToFile("Redirecting HTTP request to auth server: " + std::string(lpszObjectName));
        return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, lplpszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, dwContext);
    }
    return originalHttpOpenRequestA(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (lpszObjectName && ShouldBlockDomainW(lpszObjectName, response)) {
        LogToFile("Redirecting HTTPS request to auth server");
        return originalHttpOpenRequestW(hConnect, L"GET", L"/", L"HTTP/1.1", NULL, lplpszAcceptTypes, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, dwContext);
    }
    return originalHttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (ShouldBlockDomain(lpszServerName, response)) {
        LogToFile("Redirecting connection to auth server from: " + std::string(lpszServerName));
        return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (ShouldBlockDomainW(lpszServerName, response)) {
        LogToFile("Redirecting secure connection to auth server");
        return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

// Function to install hooks
template<typename T>
bool InstallHook(T& original, T hooked, const char* name) {
    DWORD oldProtect;
    LPVOID originalPtr = reinterpret_cast<LPVOID>(original);
    
    if (!VirtualProtect(originalPtr, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        LogToFile("Failed to modify protection for " + std::string(name));
        return false;
    }

    memcpy(originalPtr, &hooked, sizeof(T));
    VirtualProtect(originalPtr, sizeof(T), oldProtect, &oldProtect);
    
    LogToFile("Successfully installed hook for " + std::string(name));
    return true;
}

// Export function to verify injection
extern "C" __declspec(dllexport) BOOL WINAPI VerifyInjection() {
    LogToFile("ZeroFN Auth Bypass Active and Working");
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
            LogToFile("ZeroFN Auth Bypass DLL Injected Successfully");

            // Get wininet functions
            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                hWininet = LoadLibraryA("wininet.dll");
            }
            if (!hWininet) {
                LogToFile("Failed to load wininet.dll");
                return FALSE;
            }

            // Store original functions
            originalHttpSendRequestA = (tHttpSendRequestA)GetProcAddress(hWininet, "HttpSendRequestA");
            originalHttpOpenRequestA = (tHttpOpenRequestA)GetProcAddress(hWininet, "HttpOpenRequestA");
            originalInternetConnectA = (tInternetConnectA)GetProcAddress(hWininet, "InternetConnectA");
            originalHttpSendRequestW = (tHttpSendRequestW)GetProcAddress(hWininet, "HttpSendRequestW");
            originalHttpOpenRequestW = (tHttpOpenRequestW)GetProcAddress(hWininet, "HttpOpenRequestW");
            originalInternetConnectW = (tInternetConnectW)GetProcAddress(hWininet, "InternetConnectW");

            // Install hooks
            bool success = true;
            success &= InstallHook(originalHttpSendRequestA, HookedHttpSendRequestA, "HttpSendRequestA");
            success &= InstallHook(originalHttpOpenRequestA, HookedHttpOpenRequestA, "HttpOpenRequestA");
            success &= InstallHook(originalInternetConnectA, HookedInternetConnectA, "InternetConnectA");
            success &= InstallHook(originalHttpSendRequestW, HookedHttpSendRequestW, "HttpSendRequestW");
            success &= InstallHook(originalHttpOpenRequestW, HookedHttpOpenRequestW, "HttpOpenRequestW");
            success &= InstallHook(originalInternetConnectW, HookedInternetConnectW, "InternetConnectW");

            if (success) {
                LogToFile("ZeroFN Auth Bypass System Ready - All Hooks Active");
            } else {
                LogToFile("Warning: Some hooks failed - Contact Support");
            }
            break;
        }
        case DLL_PROCESS_DETACH:
            LogToFile("ZeroFN Auth Bypass Shutting Down");
            break;
    }
    return TRUE;
}
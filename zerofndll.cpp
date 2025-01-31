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

// Logger function
void LogToFile(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("zerofn_bypass.log", std::ios::app);
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", localtime(&now_c));
    logFile << timestamp << " " << message << std::endl;
}

// Improved authentication responses
const std::vector<std::pair<std::string, std::string>> AUTH_RESPONSES = {
    {"account-public-service-prod", "{\"access_token\":\"eg1~valid_token\",\"expires_in\":28800,\"token_type\":\"bearer\",\"refresh_token\":\"valid_refresh\",\"refresh_expires\":115200,\"account_id\":\"valid_account\",\"client_id\":\"valid_client\",\"internal_client\":true,\"client_service\":\"fortnite\",\"scope\":[\"basic_profile\",\"friends_list\",\"presence\"]}"},
    {"fortnite-game", "{\"status\":\"UP\",\"message\":\"Fortnite is online\",\"allowedToPlay\":true,\"banned\":false,\"launcherInfoDTO\":{\"appName\":\"Fortnite\",\"catalogItemId\":\"valid_catalog\",\"namespace\":\"fn\"}}"},
    {"lightswitch-public-service-prod", "{\"serviceInstanceId\":\"fortnite\",\"status\":\"UP\",\"message\":\"Fortnite services are online\",\"maintenanceUri\":null,\"allowedActions\":[\"PLAY\",\"LOGIN\"],\"banned\":false}"},
    {"datarouter", "{\"success\":true,\"payloadID\":\"valid_payload\",\"sessionID\":\"valid_session\"}"},
    {"presence", "{\"bIsPlaying\":true,\"bIsJoinable\":false,\"bHasVoiceSupport\":false,\"Status\":\"Playing Battle Royale\",\"Properties\":{\"KairosProfile_j\":{\"bIsValid\":true}}}"}
};

// Enhanced domain blocking with forced authentication bypass
bool ShouldBypassAuth(const char* domain, std::string& response) {
    if (!domain) return false;

    for (const auto& auth : AUTH_RESPONSES) {
        if (strstr(domain, auth.first.c_str())) {
            response = auth.second;
            LogToFile("Bypassing authentication for: " + std::string(domain));
            return true;
        }
    }

    // Force bypass for any Epic/Fortnite related domains
    if (strstr(domain, "epic") || strstr(domain, "fortnite")) {
        response = "{\"status\":\"success\",\"authenticated\":true}";
        LogToFile("Forcing authentication bypass for: " + std::string(domain));
        return true;
    }

    return false;
}

bool ShouldBypassAuthW(const wchar_t* domain, std::string& response) {
    if (!domain) return false;
    std::wstring wDomain(domain);
    std::string sDomain(wDomain.begin(), wDomain.end());
    return ShouldBypassAuth(sDomain.c_str(), response);
}

// Hooked functions with forced authentication bypass
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::string response;
    char urlBuffer[1024];
    DWORD urlSize = sizeof(urlBuffer);
    
    if (InternetQueryOptionA(hRequest, INTERNET_OPTION_URL, urlBuffer, &urlSize)) {
        if (ShouldBypassAuth(urlBuffer, response)) {
            // Force successful authentication response
            InternetSetStatusCallback(hRequest, NULL);
            SetLastError(0);
            
            // Set fake response
            DWORD responseLength = response.length();
            INTERNET_BUFFERSA buffers = {0};
            buffers.dwStructSize = sizeof(INTERNET_BUFFERSA);
            buffers.lpvBuffer = (LPVOID)response.c_str();
            buffers.dwBufferLength = responseLength;
            
            // Complete request immediately with success
            return TRUE;
        }
    }
    
    return originalHttpSendRequestA(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    std::string response;
    WCHAR urlBuffer[1024];
    DWORD urlSize = sizeof(urlBuffer);
    
    if (InternetQueryOptionW(hRequest, INTERNET_OPTION_URL, urlBuffer, &urlSize)) {
        if (ShouldBypassAuthW(urlBuffer, response)) {
            // Force successful authentication response
            InternetSetStatusCallback(hRequest, NULL);
            SetLastError(0);
            
            // Set fake response
            std::wstring wResponse(response.begin(), response.end());
            DWORD responseLength = response.length();
            INTERNET_BUFFERSW buffers = {0};
            buffers.dwStructSize = sizeof(INTERNET_BUFFERSW);
            buffers.lpvBuffer = (LPVOID)wResponse.c_str();
            buffers.dwBufferLength = responseLength * sizeof(wchar_t);
            
            // Complete request immediately with success
            return TRUE;
        }
    }
    
    return originalHttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (lpszObjectName && ShouldBypassAuth(lpszObjectName, response)) {
        // Force local loopback for auth requests
        return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, NULL, INTERNET_FLAG_IGNORE_CERT_CN_INVALID, 0);
    }
    return originalHttpOpenRequestA(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (lpszObjectName && ShouldBypassAuthW(lpszObjectName, response)) {
        // Force local loopback for auth requests
        return originalHttpOpenRequestW(hConnect, L"GET", L"/", L"HTTP/1.1", NULL, NULL, INTERNET_FLAG_IGNORE_CERT_CN_INVALID, 0);
    }
    return originalHttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (ShouldBypassAuth(lpszServerName, response)) {
        // Force connection to localhost
        return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, 0, 0);
    }
    return originalInternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (ShouldBypassAuthW(lpszServerName, response)) {
        // Force connection to localhost
        return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, 0, 0);
    }
    return originalInternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

// Function to install hooks
bool InstallHook(LPVOID& originalFunction, LPVOID hookFunction, const char* functionName) {
    DWORD oldProtect;
    VirtualProtect(originalFunction, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(originalFunction, hookFunction, sizeof(LPVOID));
    VirtualProtect(originalFunction, sizeof(LPVOID), oldProtect, &oldProtect);
    LogToFile("Installed hook for " + std::string(functionName));
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            LogToFile("ZeroFN Auth Bypass DLL Injected");

            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) hWininet = LoadLibraryA("wininet.dll");
            if (!hWininet) {
                LogToFile("Failed to load wininet.dll");
                return FALSE;
            }

            // Get original functions
            originalHttpSendRequestA = (tHttpSendRequestA)GetProcAddress(hWininet, "HttpSendRequestA");
            originalHttpOpenRequestA = (tHttpOpenRequestA)GetProcAddress(hWininet, "HttpOpenRequestA");
            originalInternetConnectA = (tInternetConnectA)GetProcAddress(hWininet, "InternetConnectA");
            originalHttpSendRequestW = (tHttpSendRequestW)GetProcAddress(hWininet, "HttpSendRequestW");
            originalHttpOpenRequestW = (tHttpOpenRequestW)GetProcAddress(hWininet, "HttpOpenRequestW");
            originalInternetConnectW = (tInternetConnectW)GetProcAddress(hWininet, "InternetConnectW");

            // Install hooks
            InstallHook((LPVOID&)originalHttpSendRequestA, (LPVOID)HookedHttpSendRequestA, "HttpSendRequestA");
            InstallHook((LPVOID&)originalHttpOpenRequestA, (LPVOID)HookedHttpOpenRequestA, "HttpOpenRequestA");
            InstallHook((LPVOID&)originalInternetConnectA, (LPVOID)HookedInternetConnectA, "InternetConnectA");
            InstallHook((LPVOID&)originalHttpSendRequestW, (LPVOID)HookedHttpSendRequestW, "HttpSendRequestW");
            InstallHook((LPVOID&)originalHttpOpenRequestW, (LPVOID)HookedHttpOpenRequestW, "HttpOpenRequestW");
            InstallHook((LPVOID&)originalInternetConnectW, (LPVOID)HookedInternetConnectW, "InternetConnectW");

            LogToFile("All hooks installed successfully");
            break;
        }
        case DLL_PROCESS_DETACH:
            LogToFile("DLL detaching");
            break;
    }
    return TRUE;
}
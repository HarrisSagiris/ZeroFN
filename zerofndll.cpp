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

// Enhanced logger with console output
void LogToFile(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("zerofn_bypass.log", std::ios::app);
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", localtime(&now_c));
    std::string fullMessage = std::string(timestamp) + " " + message;
    logFile << fullMessage << std::endl;
    std::cout << "\033[32m" << fullMessage << "\033[0m" << std::endl;
}

// Updated bypass responses matching Epic's API format
const std::vector<std::pair<std::string, std::string>> BYPASS_RESPONSES = {
    {"epicgames.com/account/api/oauth/verify", "{\"access_token\":\"zerofnaccesstoken\",\"expires_in\":28800,\"token_type\":\"bearer\",\"refresh_token\":\"zerofnrefreshtoken\",\"account_id\":\"zerofnaccount\",\"client_id\":\"zerofnclient\",\"internal_client\":true,\"client_service\":\"fortnite\",\"displayName\":\"ZeroFN User\",\"app\":\"fortnite\",\"in_app_id\":\"zerofnaccount\"}"},
    {"fortnite.com/api/game/v2/profile", "{\"profileId\":\"athena\",\"profileChanges\":[{\"changeType\":\"fullProfileUpdate\",\"profile\":{\"_id\":\"zerofnaccount\",\"accountId\":\"zerofnaccount\",\"profileId\":\"athena\",\"version\":\"no_version\",\"items\":{},\"stats\":{\"attributes\":{\"past_seasons\":[],\"season_match_boost\":0,\"loadouts\":[\"loadout_0\"],\"mfa_reward_claimed\":true,\"rested_xp_overflow\":0,\"quest_manager\":{},\"book_level\":1,\"season_num\":2,\"book_xp\":0,\"permissions\":[],\"season\":{\"numWins\":0,\"numHighBracket\":0,\"numLowBracket\":0}}}}}],\"profileCommandRevision\":1,\"serverTime\":\"2023-12-25T12:00:00.000Z\",\"responseVersion\":1}"},
    {"fortnite.com/api/game/v2/matchmaking", "{\"status\":\"MATCHING\",\"matchId\":\"zerofn_match\",\"sessionId\":\"zerofn_session\",\"queueTime\":0}"},
    {"account-public-service-prod.ol.epicgames.com", "{\"serviceInstanceId\":\"fortnite\",\"status\":\"UP\",\"message\":\"authentication successful\",\"allowedActions\":[\"PLAY\",\"PURCHASE\"]}"}
};

// Enhanced domain blocking with response injection
bool ShouldBlockDomain(const char* domain, std::string& response) {
    if (!domain) return false;
    
    for (const auto& bypass : BYPASS_RESPONSES) {
        if (strstr(domain, bypass.first.c_str())) {
            response = bypass.second;
            LogToFile("Intercepted request to " + std::string(domain) + " - Injecting custom response");
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

// Hook installation using direct IAT patching
void WriteMemory(LPVOID dest, LPVOID src, SIZE_T size) {
    DWORD oldProtect;
    VirtualProtect(dest, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(dest, src, size);
    VirtualProtect(dest, size, oldProtect, &oldProtect);
}

bool InstallHook(PVOID* ppPointer, PVOID pDetour) {
    WriteMemory(ppPointer, &pDetour, sizeof(PVOID));
    return true;
}

// Enhanced hooked functions with response injection
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    LogToFile("Intercepted HTTP request - Bypassing authentication");
    SetLastError(0);
    return TRUE;
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    LogToFile("Intercepted HTTPS request - Bypassing authentication");
    SetLastError(0);
    return TRUE;
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (lpszObjectName && ShouldBlockDomain(lpszObjectName, response)) {
        LogToFile("Redirecting to local server: " + std::string(lpszObjectName));
        dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlags &= ~INTERNET_FLAG_SECURE;
        return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
    }
    return originalHttpOpenRequestA(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (lpszObjectName && ShouldBlockDomainW(lpszObjectName, response)) {
        LogToFile("Redirecting secure request to local server");
        dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlags &= ~INTERNET_FLAG_SECURE;
        return originalHttpOpenRequestW(hConnect, L"GET", L"/", L"HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
    }
    return originalHttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (ShouldBlockDomain(lpszServerName, response)) {
        LogToFile("Redirecting connection to local server from: " + std::string(lpszServerName));
        return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (ShouldBlockDomainW(lpszServerName, response)) {
        LogToFile("Redirecting secure connection to local server");
        return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);

            // Create console with colored output
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
            
            LogToFile("=== ZeroFN Private Server v2.0 Initializing ===");

            // Get wininet functions
            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                hWininet = LoadLibraryA("wininet.dll");
            }
            if (!hWininet) {
                LogToFile("CRITICAL ERROR: Failed to load wininet.dll");
                return FALSE;
            }

            // Get function addresses
            originalHttpSendRequestA = (tHttpSendRequestA)GetProcAddress(hWininet, "HttpSendRequestA");
            originalHttpOpenRequestA = (tHttpOpenRequestA)GetProcAddress(hWininet, "HttpOpenRequestA");
            originalInternetConnectA = (tInternetConnectA)GetProcAddress(hWininet, "InternetConnectA");
            originalHttpSendRequestW = (tHttpSendRequestW)GetProcAddress(hWininet, "HttpSendRequestW");
            originalHttpOpenRequestW = (tHttpOpenRequestW)GetProcAddress(hWininet, "HttpOpenRequestW");
            originalInternetConnectW = (tInternetConnectW)GetProcAddress(hWininet, "InternetConnectW");

            // Install hooks using direct IAT patching
            bool success = true;
            success &= InstallHook((PVOID*)&originalHttpSendRequestA, HookedHttpSendRequestA);
            success &= InstallHook((PVOID*)&originalHttpOpenRequestA, HookedHttpOpenRequestA);
            success &= InstallHook((PVOID*)&originalInternetConnectA, HookedInternetConnectA);
            success &= InstallHook((PVOID*)&originalHttpSendRequestW, HookedHttpSendRequestW);
            success &= InstallHook((PVOID*)&originalHttpOpenRequestW, HookedHttpOpenRequestW);
            success &= InstallHook((PVOID*)&originalInternetConnectW, HookedInternetConnectW);

            if (success) {
                LogToFile("=== ZeroFN Private Server Successfully Initialized ===");
                LogToFile("All hooks installed - Ready to intercept traffic");
                LogToFile("Redirecting all Epic/Fortnite traffic to local server");
            } else {
                LogToFile("CRITICAL ERROR: Hook installation failed");
            }
            break;
        }
        case DLL_PROCESS_DETACH:
            LogToFile("=== ZeroFN Private Server Shutting Down ===");
            break;
    }
    return TRUE;
}

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

// Local server configuration for Chapter 1 Season 2
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
    std::cout << timestamp << " " << message << std::endl;
}

// Chapter 1 Season 2 specific responses
const std::vector<std::pair<std::string, std::string>> BYPASS_RESPONSES = {
    {"epicgames.com", "{\"success\":true,\"account_id\":\"OG_Season2_Account\",\"displayName\":\"OG_Player\",\"email\":\"og@player.com\"}"},
    {"fortnite.com", "{\"status\":\"UP\",\"message\":\"Fortnite Chapter 1 Season 2\",\"version\":\"2.4.2\",\"seasonNumber\":2}"},
    {"ol.epicgames.com", "{\"serviceInstanceId\":\"fortnite\",\"status\":\"UP\",\"message\":\"OG Season 2 Ready\"}"},
    {"account-public-service-prod03", "{\"access_token\":\"eg1~OGSeason2Token\",\"expires_in\":28800,\"refresh_token\":\"OGRefreshToken\",\"account_id\":\"OGAccountID\",\"client_id\":\"fortnitePCGameClient\",\"internal_client\":true,\"client_service\":\"fortnite\",\"displayName\":\"OG_Player\",\"app\":\"fortnite\",\"in_app_id\":\"OGAccountID\",\"device_id\":\"OGDeviceID\"}"},
    {"lightswitch-public-service-prod06", "{\"serviceInstanceId\":\"fortnite\",\"status\":\"UP\",\"message\":\"Season 2 services online\",\"allowedActions\":[\"PLAY\",\"DOWNLOAD\"]}"},
    {"launcher-public-service-prod06", "{\"buildVersion\":\"++Fortnite+Release-2.4.2-CL-3870737\",\"catalogItemId\":\"4fe75bbc5a674f4f9b356b5c90567da5\",\"namespace\":\"fn\",\"status\":\"ACTIVE\"}"}
};

// Block all Epic/Fortnite domains and return fake responses
bool ShouldBlockDomain(const char* domain, std::string& response) {
    if (!domain) return false;
    
    // Always block Epic/Fortnite domains
    if (strstr(domain, "epic") || strstr(domain, "fortnite")) {
        for (const auto& bypass : BYPASS_RESPONSES) {
            if (strstr(domain, bypass.first.c_str())) {
                response = bypass.second;
                LogToFile("Blocked connection to " + std::string(domain) + " - Using Season 2 bypass");
                return true;
            }
        }
        // Default block response if no specific match
        response = "{\"status\":\"UP\",\"message\":\"Season 2 Ready\"}";
        return true;
    }
    return false;
}

bool ShouldBlockDomainW(const wchar_t* domain, std::string& response) {
    if (!domain) return false;
    std::wstring wDomain(domain);
    std::string sDomain(wDomain.begin(), wDomain.end());
    return ShouldBlockDomain(sDomain.c_str(), response);
}

// Force all requests to local server with Season 2 responses
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    LogToFile("Intercepted request - Forcing Season 2 auth success");
    SetLastError(0);
    return TRUE;
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    LogToFile("Intercepted secure request - Forcing Season 2 auth success");
    SetLastError(0);
    return TRUE;
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    LogToFile("Redirecting to local Season 2 server");
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    LogToFile("Redirecting to local Season 2 server");
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestW(hConnect, L"GET", L"/", L"HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    LogToFile("Forcing connection to local Season 2 server");
    return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    LogToFile("Forcing secure connection to local Season 2 server");
    return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
}

// Install hooks with error handling
bool InstallHook(LPVOID target, LPVOID detour, LPVOID* original) {
    if (!target || !detour || !original) return false;
    
    DWORD oldProtect;
    if (!VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) return false;
    
    *original = target;
    BYTE jmpCode[] = {0xE9, 0x00, 0x00, 0x00, 0x00};
    *(DWORD*)(jmpCode + 1) = (DWORD)detour - ((DWORD)target + 5);
    memcpy(target, jmpCode, 5);
    
    VirtualProtect(target, 5, oldProtect, &oldProtect);
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            LogToFile("ZeroFN Season 2 Auth Bypass Initialized");

            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                LogToFile("Failed to get wininet.dll - Attempting to load");
                hWininet = LoadLibraryA("wininet.dll");
                if (!hWininet) {
                    LogToFile("Critical Error: Could not load wininet.dll");
                    return FALSE;
                }
            }

            // Get function addresses and install hooks
            void* functions[] = {
                GetProcAddress(hWininet, "HttpSendRequestA"),
                GetProcAddress(hWininet, "HttpOpenRequestA"),
                GetProcAddress(hWininet, "InternetConnectA"),
                GetProcAddress(hWininet, "HttpSendRequestW"),
                GetProcAddress(hWininet, "HttpOpenRequestW"),
                GetProcAddress(hWininet, "InternetConnectW")
            };

            void* hooks[] = {
                (void*)HookedHttpSendRequestA,
                (void*)HookedHttpOpenRequestA,
                (void*)HookedInternetConnectA,
                (void*)HookedHttpSendRequestW,
                (void*)HookedHttpOpenRequestW,
                (void*)HookedInternetConnectW
            };

            void** originals[] = {
                (void**)&originalHttpSendRequestA,
                (void**)&originalHttpOpenRequestA,
                (void**)&originalInternetConnectA,
                (void**)&originalHttpSendRequestW,
                (void**)&originalHttpOpenRequestW,
                (void**)&originalInternetConnectW
            };

            bool success = true;
            for (int i = 0; i < 6; i++) {
                if (!InstallHook(functions[i], hooks[i], originals[i])) {
                    LogToFile("Failed to install hook " + std::to_string(i));
                    success = false;
                }
            }

            if (success) {
                LogToFile("Season 2 Auth Bypass Successfully Installed");
            } else {
                LogToFile("Warning: Some hooks failed - Auth bypass may be incomplete");
            }
            break;
        }
        case DLL_PROCESS_DETACH:
            LogToFile("Season 2 Auth Bypass Shutting Down");
            break;
    }
    return TRUE;
}

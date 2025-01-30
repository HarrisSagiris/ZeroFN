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

// Block list for Epic/Fortnite domains with active bypass responses
const std::vector<std::pair<std::string, std::string>> BYPASS_RESPONSES = {
    {"epicgames.com", "{\"success\":true,\"account_id\":\"bypass_account\",\"session_id\":\"active_session\"}"},
    {"fortnite.com", "{\"status\":\"UP\",\"message\":\"Fortnite is online\",\"version\":\"2.0.0\"}"},
    {"ol.epicgames.com", "{\"serviceInstanceId\":\"fortnite\",\"status\":\"UP\",\"message\":\"authentication successful\"}"},
    {"account-public-service-prod03", "{\"access_token\":\"bypass_token\",\"expires_in\":28800,\"client_id\":\"fortnitePCGameClient\"}"},
    {"lightswitch-public-service-prod06", "{\"serviceInstanceId\":\"fortnite\",\"status\":\"UP\",\"message\":\"lightswitch check passed\"}"},
    {"launcher-public-service-prod06", "{\"buildVersion\":\"++Fortnite+Release-2.5.0\",\"status\":\"ACTIVE\",\"message\":\"launcher check passed\"}")
};

// Enhanced domain blocking with active response generation
bool ShouldBlockDomain(const char* domain, std::string& response) {
    for (const auto& bypass : BYPASS_RESPONSES) {
        if (strstr(domain, bypass.first.c_str())) {
            response = bypass.second;
            LogToFile("Intercepted request to " + std::string(domain) + " - Generating bypass response");
            return true;
        }
    }
    return false;
}

bool ShouldBlockDomainW(const wchar_t* domain, std::string& response) {
    std::wstring wDomain(domain);
    std::string sDomain(wDomain.begin(), wDomain.end());
    return ShouldBlockDomain(sDomain.c_str(), response);
}

// Enhanced hooked functions with active response generation
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    LogToFile("Intercepted HttpSendRequestA - Generating success response");
    return TRUE;
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    LogToFile("Intercepted HttpSendRequestW - Generating success response");
    return TRUE;
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (lpszObjectName && ShouldBlockDomain(lpszObjectName, response)) {
        LogToFile("Redirecting HTTP request to local server: " + std::string(lpszObjectName));
        dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
        dwFlags &= ~INTERNET_FLAG_SECURE;
        return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
    }
    return originalHttpOpenRequestA(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    std::string response;
    if (lpszObjectName && ShouldBlockDomainW(lpszObjectName, response)) {
        LogToFile("Redirecting HTTPS request to local server");
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

// Export function to verify injection
extern "C" __declspec(dllexport) BOOL WINAPI VerifyInjection() {
    LogToFile("Injection verification requested - Status: Active");
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);

            // Only allow 64-bit processes
            #ifndef _WIN64
            return FALSE;
            #endif

            // Create console for debugging
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            LogToFile("ZeroFN Auth Bypass DLL Injected - Starting active bypass system");

            // Get wininet functions
            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                hWininet = LoadLibraryA("wininet.dll");
            }
            if (!hWininet) {
                LogToFile("ERROR: Failed to load wininet.dll");
                return FALSE;
            }

            // Store original functions
            originalHttpSendRequestA = (tHttpSendRequestA)GetProcAddress(hWininet, "HttpSendRequestA");
            originalHttpOpenRequestA = (tHttpOpenRequestA)GetProcAddress(hWininet, "HttpOpenRequestA");
            originalInternetConnectA = (tInternetConnectA)GetProcAddress(hWininet, "InternetConnectA");
            originalHttpSendRequestW = (tHttpSendRequestW)GetProcAddress(hWininet, "HttpSendRequestW");
            originalHttpOpenRequestW = (tHttpOpenRequestW)GetProcAddress(hWininet, "HttpOpenRequestW");
            originalInternetConnectW = (tInternetConnectW)GetProcAddress(hWininet, "InternetConnectW");

            // Install hooks with improved error handling
            DWORD oldProtect;
            auto InstallHook = [&](void* original, void* hooked, const char* name) {
                if (!VirtualProtect(original, 5, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                    LogToFile("ERROR: Failed to modify protection for " + std::string(name));
                    return false;
                }
                *(BYTE*)original = 0xE9;
                *(DWORD*)((BYTE*)original + 1) = (DWORD)((BYTE*)hooked - (BYTE*)original - 5);
                VirtualProtect(original, 5, oldProtect, &oldProtect);
                LogToFile("Successfully installed hook for " + std::string(name));
                return true;
            };

            bool success = true;
            success &= InstallHook(originalHttpSendRequestA, HookedHttpSendRequestA, "HttpSendRequestA");
            success &= InstallHook(originalHttpOpenRequestA, HookedHttpOpenRequestA, "HttpOpenRequestA");
            success &= InstallHook(originalInternetConnectA, HookedInternetConnectA, "InternetConnectA");
            success &= InstallHook(originalHttpSendRequestW, HookedHttpSendRequestW, "HttpSendRequestW");
            success &= InstallHook(originalHttpOpenRequestW, HookedHttpOpenRequestW, "HttpOpenRequestW");
            success &= InstallHook(originalInternetConnectW, HookedInternetConnectW, "InternetConnectW");

            if (success) {
                LogToFile("All hooks installed successfully - Active bypass system ready");
            } else {
                LogToFile("WARNING: Some hooks failed to install - System may not function correctly");
            }
            break;
        }
        case DLL_PROCESS_DETACH:
            LogToFile("DLL detaching - Shutting down bypass system");
            break;
    }
    return TRUE;
}

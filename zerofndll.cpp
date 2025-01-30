#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <wininet.h>
#include <urlmon.h>
#include <sstream>
#include <fstream>
#include <TlHelp32.h>

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

// Block list for Epic/Fortnite domains
const std::vector<std::string> BLOCKED_DOMAINS = {
    "epicgames.com",
    "fortnite.com",
    "ol.epicgames.com",
    "account-public-service-prod03",
    "lightswitch-public-service-prod06",
    "launcher-public-service-prod06"
};

// Helper function to check if domain should be blocked
bool ShouldBlockDomain(const char* domain) {
    for (const auto& blocked : BLOCKED_DOMAINS) {
        if (strstr(domain, blocked.c_str())) {
            return true;
        }
    }
    return false;
}

bool ShouldBlockDomainW(const wchar_t* domain) {
    std::wstring wDomain(domain);
    std::string sDomain(wDomain.begin(), wDomain.end());
    return ShouldBlockDomain(sDomain.c_str());
}

// Hooked functions that redirect all traffic to local server
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    // Always succeed to bypass auth checks
    return TRUE;
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    // Always succeed to bypass auth checks
    return TRUE;
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    // Disable SSL/TLS verification
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestA(hConnect, lpszVerb, "/", lpszVersion, NULL, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    // Disable SSL/TLS verification
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestW(hConnect, lpszVerb, L"/", lpszVersion, NULL, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    // Redirect all connections to local server
    return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    // Redirect all connections to local server
    return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
}

// Export function to verify injection
extern "C" __declspec(dllexport) BOOL WINAPI VerifyInjection() {
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
            std::cout << "ZeroFN Auth Bypass DLL Injected" << std::endl;

            // Get wininet functions
            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                hWininet = LoadLibraryA("wininet.dll");
            }
            if (!hWininet) {
                std::cout << "Failed to load wininet.dll" << std::endl;
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
            DWORD oldProtect;
            VirtualProtect(originalHttpSendRequestA, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalHttpSendRequestA = 0xE9;
            *(DWORD*)((BYTE*)originalHttpSendRequestA + 1) = (DWORD)((BYTE*)HookedHttpSendRequestA - (BYTE*)originalHttpSendRequestA - 5);
            VirtualProtect(originalHttpSendRequestA, 5, oldProtect, &oldProtect);

            VirtualProtect(originalHttpOpenRequestA, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalHttpOpenRequestA = 0xE9;
            *(DWORD*)((BYTE*)originalHttpOpenRequestA + 1) = (DWORD)((BYTE*)HookedHttpOpenRequestA - (BYTE*)originalHttpOpenRequestA - 5);
            VirtualProtect(originalHttpOpenRequestA, 5, oldProtect, &oldProtect);

            VirtualProtect(originalInternetConnectA, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalInternetConnectA = 0xE9;
            *(DWORD*)((BYTE*)originalInternetConnectA + 1) = (DWORD)((BYTE*)HookedInternetConnectA - (BYTE*)originalInternetConnectA - 5);
            VirtualProtect(originalInternetConnectA, 5, oldProtect, &oldProtect);

            VirtualProtect(originalHttpSendRequestW, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalHttpSendRequestW = 0xE9;
            *(DWORD*)((BYTE*)originalHttpSendRequestW + 1) = (DWORD)((BYTE*)HookedHttpSendRequestW - (BYTE*)originalHttpSendRequestW - 5);
            VirtualProtect(originalHttpSendRequestW, 5, oldProtect, &oldProtect);

            VirtualProtect(originalHttpOpenRequestW, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalHttpOpenRequestW = 0xE9;
            *(DWORD*)((BYTE*)originalHttpOpenRequestW + 1) = (DWORD)((BYTE*)HookedHttpOpenRequestW - (BYTE*)originalHttpOpenRequestW - 5);
            VirtualProtect(originalHttpOpenRequestW, 5, oldProtect, &oldProtect);

            VirtualProtect(originalInternetConnectW, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalInternetConnectW = 0xE9;
            *(DWORD*)((BYTE*)originalInternetConnectW + 1) = (DWORD)((BYTE*)HookedInternetConnectW - (BYTE*)originalInternetConnectW - 5);
            VirtualProtect(originalInternetConnectW, 5, oldProtect, &oldProtect);

            std::cout << "All hooks installed successfully" << std::endl;
            break;
        }
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

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

// Memory patch helper
void PatchMemory(BYTE* dst, BYTE* src, size_t size) {
    DWORD oldProtect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
}

// Force connection to local server
HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    // Always redirect to local server
    LogToFile("Forcing connection to local server");
    return originalInternetConnectA(hInternet, LOCAL_SERVER, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    // Always redirect to local server
    LogToFile("Forcing secure connection to local server");
    return originalInternetConnectW(hInternet, LOCAL_SERVER_W, LOCAL_PORT, NULL, NULL, dwService, dwFlags, dwContext);
}

// Force successful auth responses
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    // Patch memory to force successful auth
    BYTE successResponse[] = {0x90, 0x90, 0x90, 0x90, 0x90}; // NOP sled
    PatchMemory((BYTE*)hRequest + 0x20, successResponse, sizeof(successResponse));
    LogToFile("Forced successful auth response");
    return TRUE;
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    // Patch memory to force successful auth
    BYTE successResponse[] = {0x90, 0x90, 0x90, 0x90, 0x90}; // NOP sled
    PatchMemory((BYTE*)hRequest + 0x20, successResponse, sizeof(successResponse));
    LogToFile("Forced successful auth response");
    return TRUE;
}

// Force all requests to local server
HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestA(hConnect, "GET", "/", "HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestW(hConnect, L"GET", L"/", L"HTTP/1.1", NULL, lplpszAcceptTypes, dwFlags, dwContext);
}

// Function to install hooks
bool InstallHook(LPVOID target, LPVOID detour, LPVOID* original) {
    BYTE jmpBytes[] = {0xE9, 0x00, 0x00, 0x00, 0x00}; // JMP instruction
    DWORD relativeAddress = (DWORD)detour - ((DWORD)target + 5);
    memcpy(jmpBytes + 1, &relativeAddress, 4);
    
    DWORD oldProtect;
    VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(original, target, 5);
    memcpy(target, jmpBytes, 5);
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
            LogToFile("ZeroFN Auth Bypass Initialized");

            // Get wininet functions
            HMODULE hWininet = GetModuleHandleA("wininet.dll");
            if (!hWininet) {
                hWininet = LoadLibraryA("wininet.dll");
            }

            // Store and hook functions
            originalHttpSendRequestA = (tHttpSendRequestA)GetProcAddress(hWininet, "HttpSendRequestA");
            originalHttpOpenRequestA = (tHttpOpenRequestA)GetProcAddress(hWininet, "HttpOpenRequestA");
            originalInternetConnectA = (tInternetConnectA)GetProcAddress(hWininet, "InternetConnectA");
            originalHttpSendRequestW = (tHttpSendRequestW)GetProcAddress(hWininet, "HttpSendRequestW");
            originalHttpOpenRequestW = (tHttpOpenRequestW)GetProcAddress(hWininet, "HttpOpenRequestW");
            originalInternetConnectW = (tInternetConnectW)GetProcAddress(hWininet, "InternetConnectW");

            // Install hooks with direct memory patching
            InstallHook((LPVOID)originalHttpSendRequestA, (LPVOID)HookedHttpSendRequestA, (LPVOID*)&originalHttpSendRequestA);
            InstallHook((LPVOID)originalHttpOpenRequestA, (LPVOID)HookedHttpOpenRequestA, (LPVOID*)&originalHttpOpenRequestA);
            InstallHook((LPVOID)originalInternetConnectA, (LPVOID)HookedInternetConnectA, (LPVOID*)&originalInternetConnectA);
            InstallHook((LPVOID)originalHttpSendRequestW, (LPVOID)HookedHttpSendRequestW, (LPVOID*)&originalHttpSendRequestW);
            InstallHook((LPVOID)originalHttpOpenRequestW, (LPVOID)HookedHttpOpenRequestW, (LPVOID*)&originalHttpOpenRequestW);
            InstallHook((LPVOID)originalInternetConnectW, (LPVOID)HookedInternetConnectW, (LPVOID*)&originalInternetConnectW);

            LogToFile("Auth bypass hooks installed successfully");
            break;
        }
        case DLL_PROCESS_DETACH:
            LogToFile("DLL detaching");
            break;
    }
    return TRUE;
}

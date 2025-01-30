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

// Custom backend URL
const char* BACKEND_URL = "127.0.0.1";
const wchar_t* BACKEND_URL_W = L"127.0.0.1";
const INTERNET_PORT BACKEND_PORT = 8080;

// Helper function to log requests
void LogRequest(const char* type, const char* url) {
    std::ofstream log("zerofn_requests.log", std::ios::app);
    if (log.is_open()) {
        log << type << ": " << url << std::endl;
        log.close();
    }
}

// Hooked functions
BOOL WINAPI HookedHttpSendRequestA(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    LogRequest("SendRequestA", lpszHeaders ? lpszHeaders : "No Headers");
    return originalHttpSendRequestA(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

BOOL WINAPI HookedHttpSendRequestW(HINTERNET hRequest, LPCWSTR lpszHeaders, DWORD dwHeadersLength, LPVOID lpOptional, DWORD dwOptionalLength) {
    if (lpszHeaders) {
        std::wstring headers(lpszHeaders);
        LogRequest("SendRequestW", std::string(headers.begin(), headers.end()).c_str());
    }
    return originalHttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
}

HINTERNET WINAPI HookedHttpOpenRequestA(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestA(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedHttpOpenRequestW(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR* lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext) {
    dwFlags |= INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
    dwFlags &= ~INTERNET_FLAG_SECURE;
    return originalHttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    if (strstr(lpszServerName, "epicgames.com") || 
        strstr(lpszServerName, "fortnite.com") || 
        strstr(lpszServerName, "ol.epicgames.com")) {
        LogRequest("Redirecting", lpszServerName);
        return originalInternetConnectA(hInternet, BACKEND_URL, BACKEND_PORT, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

HINTERNET WINAPI HookedInternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) {
    std::wstring serverName(lpszServerName);
    if (serverName.find(L"epicgames.com") != std::wstring::npos || 
        serverName.find(L"fortnite.com") != std::wstring::npos || 
        serverName.find(L"ol.epicgames.com") != std::wstring::npos) {
        return originalInternetConnectW(hInternet, BACKEND_URL_W, BACKEND_PORT, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
    }
    return originalInternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
}

// Export a function that can be called from the launcher to verify injection
extern "C" __declspec(dllexport) BOOL WINAPI VerifyInjection() {
    return TRUE;
}

// Helper function to attach to parent Fortnite process
void AttachToFortniteProcess() {
    DWORD parentPID = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(pe32);
        if (Process32FirstW(hSnapshot, &pe32)) {
            do {
                if (_wcsicmp(pe32.szExeFile, L"FortniteClient-Win64-Shipping.exe") == 0) {
                    parentPID = pe32.th32ProcessID;
                    break;
                }
            } while (Process32NextW(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }

    if (parentPID) {
        HANDLE hParent = OpenProcess(PROCESS_ALL_ACCESS, FALSE, parentPID);
        if (hParent) {
            // Attach to parent process
            if (!AttachConsole(parentPID)) {
                // If attach fails, allocate new console
                AllocConsole();
            }
            CloseHandle(hParent);
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);

            // Only allow 64-bit processes
            #ifndef _WIN64
            MessageBoxA(NULL, "This DLL is for 64-bit Fortnite only", "Architecture Error", MB_OK | MB_ICONERROR);
            return FALSE;
            #endif

            // Attach to parent Fortnite process
            AttachToFortniteProcess();

            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            std::cout << "ZeroFN 64-bit DLL Injection Started..." << std::endl;

            // Load wininet.dll
            HMODULE hWininet = LoadLibraryA("wininet.dll");
            if (!hWininet) {
                std::cout << "Failed to load wininet.dll" << std::endl;
                return FALSE;
            }

            // Get function addresses
            originalHttpSendRequestA = (tHttpSendRequestA)GetProcAddress(hWininet, "HttpSendRequestA");
            originalHttpOpenRequestA = (tHttpOpenRequestA)GetProcAddress(hWininet, "HttpOpenRequestA");
            originalInternetConnectA = (tInternetConnectA)GetProcAddress(hWininet, "InternetConnectA");
            originalHttpSendRequestW = (tHttpSendRequestW)GetProcAddress(hWininet, "HttpSendRequestW");
            originalHttpOpenRequestW = (tHttpOpenRequestW)GetProcAddress(hWininet, "HttpOpenRequestW");
            originalInternetConnectW = (tInternetConnectW)GetProcAddress(hWininet, "InternetConnectW");

            if (!originalHttpSendRequestA || !originalHttpOpenRequestA || !originalInternetConnectA ||
                !originalHttpSendRequestW || !originalHttpOpenRequestW || !originalInternetConnectW) {
                std::cout << "Failed to get function addresses" << std::endl;
                return FALSE;
            }

            // Hook functions by writing jump instructions
            DWORD oldProtect;
            VirtualProtect((LPVOID)originalHttpSendRequestA, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalHttpSendRequestA = 0xE9;
            *(DWORD*)((BYTE*)originalHttpSendRequestA + 1) = (DWORD)((BYTE*)HookedHttpSendRequestA - (BYTE*)originalHttpSendRequestA - 5);
            VirtualProtect((LPVOID)originalHttpSendRequestA, 5, oldProtect, &oldProtect);

            VirtualProtect((LPVOID)originalHttpOpenRequestA, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalHttpOpenRequestA = 0xE9;
            *(DWORD*)((BYTE*)originalHttpOpenRequestA + 1) = (DWORD)((BYTE*)HookedHttpOpenRequestA - (BYTE*)originalHttpOpenRequestA - 5);
            VirtualProtect((LPVOID)originalHttpOpenRequestA, 5, oldProtect, &oldProtect);

            VirtualProtect((LPVOID)originalInternetConnectA, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalInternetConnectA = 0xE9;
            *(DWORD*)((BYTE*)originalInternetConnectA + 1) = (DWORD)((BYTE*)HookedInternetConnectA - (BYTE*)originalInternetConnectA - 5);
            VirtualProtect((LPVOID)originalInternetConnectA, 5, oldProtect, &oldProtect);

            VirtualProtect((LPVOID)originalHttpSendRequestW, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalHttpSendRequestW = 0xE9;
            *(DWORD*)((BYTE*)originalHttpSendRequestW + 1) = (DWORD)((BYTE*)HookedHttpSendRequestW - (BYTE*)originalHttpSendRequestW - 5);
            VirtualProtect((LPVOID)originalHttpSendRequestW, 5, oldProtect, &oldProtect);

            VirtualProtect((LPVOID)originalHttpOpenRequestW, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalHttpOpenRequestW = 0xE9;
            *(DWORD*)((BYTE*)originalHttpOpenRequestW + 1) = (DWORD)((BYTE*)HookedHttpOpenRequestW - (BYTE*)originalHttpOpenRequestW - 5);
            VirtualProtect((LPVOID)originalHttpOpenRequestW, 5, oldProtect, &oldProtect);

            VirtualProtect((LPVOID)originalInternetConnectW, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(BYTE*)originalInternetConnectW = 0xE9;
            *(DWORD*)((BYTE*)originalInternetConnectW + 1) = (DWORD)((BYTE*)HookedInternetConnectW - (BYTE*)originalInternetConnectW - 5);
            VirtualProtect((LPVOID)originalInternetConnectW, 5, oldProtect, &oldProtect);

            std::cout << "ZeroFN 64-bit DLL Injection Complete!" << std::endl;
            break;
        }

        case DLL_PROCESS_DETACH: {
            // Unhook functions (restore original bytes)
            // Note: In a real implementation you'd want to save and restore the original bytes
            break;
        }
    }
    return TRUE;
}

#define _WIN32_WINNT 0x0600
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

// Global variables for UI
HWND g_hwndProgress = NULL;
HWND g_hwndStatus = NULL;
const char* WINDOW_CLASS = "ZeroFNEACClass";
const char* WINDOW_TITLE = "ZeroFN Anti-Cheat";

// Function to check if process is a game process we want to protect
bool IsGameProcess(const wchar_t* processName) {
    return (wcscmp(processName, L"FortniteClient-Win64-Shipping.exe") == 0);
}

// Function to check if running with admin privileges
bool IsRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin;
}

// Window procedure for the UI
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Create progress bar
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_PROGRESS_CLASS;
            InitCommonControlsEx(&icex);

            g_hwndProgress = CreateWindowEx(0, PROGRESS_CLASS, NULL,
                WS_CHILD | WS_VISIBLE | PBS_MARQUEE,
                20, 60, 260, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);
            SendMessage(g_hwndProgress, PBM_SETMARQUEE, TRUE, 50);

            // Create status text
            g_hwndStatus = CreateWindowExA(0, "STATIC", "ZeroFN Anti-Cheat Service Running",
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                20, 20, 260, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Function to create the UI window
HWND CreateMainWindow() {
    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS;
    RegisterClassExA(&wc);

    // Create centered window
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 300;
    int windowHeight = 120;
    int posX = (screenWidth - windowWidth) / 2;
    int posY = (screenHeight - windowHeight) / 2;

    return CreateWindowExA(
        0,
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        posX, posY, windowWidth, windowHeight,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );
}

// Function to terminate EAC processes
bool TerminateEACProcesses() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(processEntry);

    if (!Process32FirstW(snapshot, &processEntry)) {
        CloseHandle(snapshot);
        return false;
    }

    bool terminated = false;
    do {
        if (wcscmp(processEntry.szExeFile, L"EasyAntiCheat.exe") == 0 ||
            wcscmp(processEntry.szExeFile, L"EasyAntiCheat_Setup.exe") == 0) {
            
            HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, processEntry.th32ProcessID);
            if (processHandle != NULL) {
                if (TerminateProcess(processHandle, 0)) {
                    terminated = true;
                }
                CloseHandle(processHandle);
            }
        }
    } while (Process32NextW(snapshot, &processEntry));

    CloseHandle(snapshot);
    return terminated;
}

// Function to bypass memory protection
void DisableMemoryProtection(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (processHandle != NULL) {
        DWORD oldProtection;
        SetProcessDEPPolicy(0);
        VirtualProtectEx(processHandle, (LPVOID)0x0, 0x7FFFFFFF, PAGE_EXECUTE_READWRITE, &oldProtection);
        CloseHandle(processHandle);
    }
}

// Function to allow DLL injection
void AllowDLLInjection(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (processHandle != NULL) {
        DWORD oldProtection;
        VirtualProtectEx(processHandle, (LPVOID)0x0, 0x7FFFFFFF, PAGE_EXECUTE_READWRITE, &oldProtection);
        CloseHandle(processHandle);
    }
}

// Function declarations
extern "C" __declspec(dllexport) bool StartZeroFNEAC();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return StartZeroFNEAC();
}

bool StartZeroFNEAC() {
    // Check for admin privileges
    if (!IsRunningAsAdmin()) {
        MessageBoxA(NULL, "ZeroFN Anti-Cheat requires administrator privileges to run.", 
                  "ZeroFN Error", MB_ICONERROR);
        return false;
    }

    // Create UI window
    HWND hwnd = CreateMainWindow();
    if (!hwnd) {
        return false;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    
    // Terminate any existing EAC processes
    TerminateEACProcesses();

    // Start monitoring thread
    std::thread([&]() {
        while (true) {
            TerminateEACProcesses();
            
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snapshot != INVALID_HANDLE_VALUE) {
                PROCESSENTRY32W processEntry;
                processEntry.dwSize = sizeof(processEntry);
                
                if (Process32FirstW(snapshot, &processEntry)) {
                    do {
                        if (IsGameProcess(processEntry.szExeFile)) {
                            DisableMemoryProtection(processEntry.th32ProcessID);
                            AllowDLLInjection(processEntry.th32ProcessID);
                        }
                    } while (Process32NextW(snapshot, &processEntry));
                }
                CloseHandle(snapshot);
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }).detach();

    // Message loop for UI
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

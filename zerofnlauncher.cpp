#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <experimental/filesystem>
#include <shlobj.h>
#include <commdlg.h>
#include <wininet.h>
#include <tlhelp32.h>
#include <vector>
#include <thread>
#include <chrono>
#pragma comment(lib, "wininet.lib")

namespace fs = std::experimental::filesystem;

// Pattern scanning helper functions
bool CompareBytes(const BYTE* data, const BYTE* pattern, const char* mask) {
    for (; *mask; ++mask, ++data, ++pattern) {
        if (*mask == 'x' && *data != *pattern)
            return false;
    }
    return (*mask) == 0;
}

DWORD_PTR FindPattern(HANDLE process, DWORD_PTR start, DWORD_PTR end, const BYTE* pattern, const char* mask) {
    BYTE* buffer = new BYTE[4096];
    DWORD_PTR pos = start;
    SIZE_T bytesRead;

    while (pos < end) {
        if (!ReadProcessMemory(process, (LPCVOID)pos, buffer, 4096, &bytesRead)) {
            pos += 4096;
            continue;
        }

        for (DWORD_PTR i = 0; i < bytesRead; i++) {
            if (CompareBytes(buffer + i, pattern, mask)) {
                delete[] buffer;
                return pos + i;
            }
        }
        pos += bytesRead;
    }

    delete[] buffer;
    return 0;
}

class ZeroFNLauncher {
public:
    ZeroFNLauncher() {
        // Create main window
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"ZeroFNLauncher";
        RegisterClassExW(&wc);

        hwnd = CreateWindowExW(
            0,
            L"ZeroFNLauncher", 
            L"ZeroFN Launcher",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
            NULL,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

        // Create controls
        CreateWindowW(L"BUTTON", L"Browse", WS_VISIBLE | WS_CHILD,
            10, 10, 80, 25, hwnd, (HMENU)1, NULL, NULL);

        pathEdit = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            100, 10, 300, 25, hwnd, (HMENU)2, NULL, NULL);

        startButton = CreateWindowW(L"BUTTON", L"Launch Game", WS_VISIBLE | WS_CHILD,
            10, 45, 100, 30, hwnd, (HMENU)3, NULL, NULL);

        stopButton = CreateWindowW(L"BUTTON", L"Stop Services", WS_VISIBLE | WS_CHILD,
            120, 45, 100, 30, hwnd, (HMENU)4, NULL, NULL);
        EnableWindow(stopButton, FALSE);

        // Create console output with larger size and auto-scroll
        consoleOutput = CreateWindowW(L"EDIT", L"", 
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL,
            10, 85, 760, 460, hwnd, (HMENU)5, NULL, NULL);

        // Set console font for better readability
        HFONT hFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
        SendMessage(consoleOutput, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Load saved path
        LoadSavedPath();
        
        ShowWindow(hwnd, SW_SHOW);
        logMessage("ZeroFN Launcher initialized successfully");
        logMessage("Waiting for user input...");
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_COMMAND:
                switch (LOWORD(wParam)) {
                    case 1: // Browse button
                        BrowsePath();
                        break;
                    case 3: // Start button  
                        StartServer();
                        break;
                    case 4: // Stop button
                        StopServer();
                        break;
                }
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    void Run() {
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

private:
    static void BrowsePath() {
        BROWSEINFOW bi = {0};
        bi.hwndOwner = hwnd;
        bi.lpszTitle = L"Select Fortnite Installation Folder";
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        bi.lpfn = BrowseCallbackProc;

        LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
        if (pidl) {
            WCHAR path[MAX_PATH];
            if (SHGetPathFromIDListW(pidl, path)) {
                std::wstring baseDir(path);
                
                if (ValidateFortniteDirectory(baseDir)) {
                    SetWindowTextW(pathEdit, baseDir.c_str());
                    SavePath(baseDir.c_str());
                    logMessage("Valid Fortnite directory selected");
                } else {
                    MessageBoxW(NULL, L"Selected folder is not a valid Fortnite installation directory.", 
                        L"Invalid Directory", MB_OK | MB_ICONWARNING);
                    logMessage("Invalid Fortnite directory selected");
                }
            }
            CoTaskMemFree(pidl);
        }
    }

    static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
        if (uMsg == BFFM_INITIALIZED) {
            std::wstring savedPath = LoadLastPath();
            if (!savedPath.empty()) {
                SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)savedPath.c_str());
            }
        }
        return 0;
    }

    static bool ValidateFortniteDirectory(const std::wstring& path) {
        std::vector<std::wstring> checkPaths = {
            L"\\FortniteGame",
            L"\\Engine",
            L"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe"
        };

        for (const auto& checkPath : checkPaths) {
            if (!fs::exists(path + checkPath)) {
                logMessage("Missing required file/directory: " + std::string(checkPath.begin(), checkPath.end()));
                return false;
            }
        }
        return true;
    }

    static std::wstring LoadLastPath() {
        std::wifstream file(L"path.txt");
        if (file.is_open()) {
            std::wstring path;
            std::getline(file, path);
            file.close();
            return path;
        }
        return L"";
    }

    static void SetupProxy() {
        logMessage("Configuring system proxy settings...");
        INTERNET_PROXY_INFO proxyInfo;
        ZeroMemory(&proxyInfo, sizeof(proxyInfo));

        proxyInfo.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
        proxyInfo.lpszProxy = (LPCTSTR)"127.0.0.1:8080";
        proxyInfo.lpszProxyBypass = (LPCTSTR)"<local>";

        BOOL result = InternetSetOption(NULL, INTERNET_OPTION_PROXY, &proxyInfo, sizeof(proxyInfo));
        if(result) {
            logMessage("Proxy configuration set successfully to 127.0.0.1:8080");
        } else {
            logMessage("Failed to set proxy configuration");
        }
    }

    static void PatchFortnite(HANDLE hProcess) {
        logMessage("Starting auth bypass patching...");
        
        // Auth bypass patterns
        const BYTE authPattern[] = {0x74, 0x23, 0x48, 0x8B, 0x4B, 0x78};
        const BYTE authPatch[] = {0xEB, 0x23, 0x48, 0x8B, 0x4B, 0x78};

        // Find and patch auth check
        DWORD_PTR authAddr = FindPattern(hProcess, 0, (DWORD_PTR)-1, authPattern, "xxxxxx");
        if (authAddr) {
            DWORD oldProtect;
            VirtualProtectEx(hProcess, (LPVOID)authAddr, sizeof(authPatch), PAGE_EXECUTE_READWRITE, &oldProtect);
            WriteProcessMemory(hProcess, (LPVOID)authAddr, authPatch, sizeof(authPatch), NULL);
            VirtualProtectEx(hProcess, (LPVOID)authAddr, sizeof(authPatch), oldProtect, &oldProtect);
            logMessage("Auth bypass patch applied successfully");
        } else {
            logMessage("WARNING: Could not find auth pattern to patch");
        }
    }

    static void StartServer() {
        logMessage("Starting server initialization sequence...");
        
        // Check required files
        logMessage("Checking required server files...");
        if (!fs::exists("server.js") || !fs::exists("redirect.py")) {
            MessageBoxW(NULL, L"Required server files are missing!", L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Missing required server files!");
            return;
        }
        logMessage("All required files found");

        // Setup proxy configuration
        SetupProxy();

        // Start Node.js auth server
        STARTUPINFOW si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOW; // Show the server window

        logMessage("Starting auth server on port 7777...");
        WCHAR nodeCmd[] = L"node server.js";
        if (!CreateProcessW(NULL, nodeCmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            MessageBoxW(NULL, L"Failed to start auth server", L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Failed to start auth server!");
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        logMessage("Auth server started successfully");

        // Start proxy server for auth redirection
        logMessage("Starting proxy server on port 8080...");
        WCHAR proxyCmd[] = L"mitmdump -s redirect.py --listen-port 8080";
        if (!CreateProcessW(NULL, proxyCmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            MessageBoxW(NULL, L"Failed to start proxy server", L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Failed to start proxy server!");
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        logMessage("Proxy server started successfully");

        // Wait for servers to initialize
        Sleep(2000); // Give servers time to start

        // Get Fortnite path
        WCHAR path[MAX_PATH];
        GetWindowTextW(pathEdit, path, MAX_PATH);
        std::wstring basePath(path);
        std::wstring fortnitePath = basePath + L"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe";

        logMessage("Verifying Fortnite executable...");
        if (!fs::exists(fortnitePath)) {
            MessageBoxW(NULL, L"Fortnite executable not found!", L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Fortnite executable not found at: " + std::string(fortnitePath.begin(), fortnitePath.end()));
            return;
        }
        logMessage("Fortnite executable found");

        // Launch Fortnite with auth redirection
        logMessage("Launching Fortnite...");
        STARTUPINFOW siGame = {0};
        PROCESS_INFORMATION piGame = {0};
        siGame.cb = sizeof(siGame);
        siGame.dwFlags = STARTF_USESHOWWINDOW;
        siGame.wShowWindow = SW_SHOW;

        std::wstring cmdLine = L"\"" + fortnitePath + L"\" -NOSSLPINNING -noeac -fromfl=be -fltoken=7d41f3c07b724575892f0def64c57569 "
            L"-skippatchcheck -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -nobe -fromfl=eac -fltoken=none "
            L"-nosound -AUTH_TYPE=epic -AUTH_LOGIN=127.0.0.1:7777 -AUTH_PASSWORD=test -http-proxy=127.0.0.1:8080 "
            L"-FORCECONSOLE -notexturestreaming -dx11 -windowed -NOFORCECONNECT";

        WCHAR* cmdLinePtr = new WCHAR[cmdLine.length() + 1];
        wcscpy_s(cmdLinePtr, cmdLine.length() + 1, cmdLine.c_str());

        BOOL success = CreateProcessW(
            fortnitePath.c_str(),
            cmdLinePtr,
            NULL,
            NULL,
            FALSE,
            CREATE_SUSPENDED,  // Create suspended so we can patch before it runs
            NULL,
            basePath.c_str(),
            &siGame,
            &piGame
        );

        delete[] cmdLinePtr;

        if (!success) {
            DWORD error = GetLastError();
            WCHAR errorMsg[256];
            swprintf_s(errorMsg, L"Failed to launch Fortnite. Error code: %d", error);
            MessageBoxW(NULL, errorMsg, L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Failed to launch Fortnite. Error code: " + std::to_string(error));
            return;
        }

        logMessage("Fortnite process created successfully (PID: " + std::to_string(piGame.dwProcessId) + ")");

        // Patch the process immediately
        PatchFortnite(piGame.hProcess);
        
        // Resume the process
        logMessage("Starting Fortnite...");
        ResumeThread(piGame.hThread);
        logMessage("Fortnite process started");

        CloseHandle(piGame.hProcess);
        CloseHandle(piGame.hThread);

        EnableWindow(startButton, FALSE);
        EnableWindow(stopButton, TRUE);
        EnableWindow(pathEdit, FALSE);
        
        logMessage("Fortnite launched successfully with auth bypass enabled");
    }

    static void StopServer() {
        logMessage("Initiating shutdown sequence...");

        // Kill processes
        logMessage("Terminating auth server process...");
        system("taskkill /F /IM node.exe");
        logMessage("Auth server terminated");
        
        logMessage("Terminating proxy server process...");
        system("taskkill /F /IM mitmdump.exe");
        logMessage("Proxy server terminated");
        
        logMessage("Terminating Fortnite client...");
        system("taskkill /F /IM FortniteClient-Win64-Shipping.exe");
        logMessage("Fortnite client terminated");

        EnableWindow(startButton, TRUE);
        EnableWindow(stopButton, FALSE);
        EnableWindow(pathEdit, TRUE);
        
        logMessage("All services stopped successfully");
        logMessage("System ready for next session");
    }

    static void LoadSavedPath() {
        std::ifstream file("path.txt");
        if (file.is_open()) {
            std::string path;
            std::getline(file, path);
            SetWindowTextA(pathEdit, path.c_str());
            file.close();
            logMessage("Loaded saved Fortnite path: " + path);
        } else {
            logMessage("No saved path found");
        }
    }

    static void SavePath(const WCHAR* path) {
        std::wofstream file("path.txt");
        if (file.is_open()) {
            file << path;
            file.close();
            logMessage("Path saved successfully");
        } else {
            logMessage("ERROR: Failed to save path");
        }
    }

    static void logMessage(const std::string& message) {
        time_t now = time(0);
        char timestamp[32];
        strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", localtime(&now));
        
        std::string logLine = timestamp + message + "\r\n";
        
        int length = GetWindowTextLength(consoleOutput);
        SendMessageA(consoleOutput, EM_SETSEL, length, length);
        SendMessageA(consoleOutput, EM_REPLACESEL, FALSE, (LPARAM)logLine.c_str());
        SendMessage(consoleOutput, EM_SCROLLCARET, 0, 0);
    }

    static HWND hwnd;
    static HWND pathEdit;
    static HWND startButton;
    static HWND stopButton;
    static HWND consoleOutput;
};

HWND ZeroFNLauncher::hwnd = NULL;
HWND ZeroFNLauncher::pathEdit = NULL;
HWND ZeroFNLauncher::startButton = NULL;
HWND ZeroFNLauncher::stopButton = NULL;
HWND ZeroFNLauncher::consoleOutput = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ZeroFNLauncher launcher;
    launcher.Run();
    return 0;
}

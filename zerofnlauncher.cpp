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
        // Initialize COM for folder browser
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

        // Create main window
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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

        // Create controls with improved styling
        CreateWindowW(L"BUTTON", L"Browse", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 10, 80, 25, hwnd, (HMENU)1, NULL, NULL);

        pathEdit = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            100, 10, 580, 25, hwnd, (HMENU)2, NULL, NULL);

        startButton = CreateWindowW(L"BUTTON", L"Launch Game", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 45, 100, 30, hwnd, (HMENU)3, NULL, NULL);

        stopButton = CreateWindowW(L"BUTTON", L"Stop Services", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            120, 45, 100, 30, hwnd, (HMENU)4, NULL, NULL);
        EnableWindow(stopButton, FALSE);

        // Create console output with improved styling
        consoleOutput = CreateWindowW(L"EDIT", L"", 
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL,
            10, 85, 760, 460, hwnd, (HMENU)5, NULL, NULL);

        // Set modern font
        HFONT hFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        
        SendMessage(pathEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(startButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(stopButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(consoleOutput, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Load saved path
        LoadSavedPath();
        
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        
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
                CoUninitialize();
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
        
        // Reset any existing proxy settings first
        INTERNET_PER_CONN_OPTION_LIST list;
        INTERNET_PER_CONN_OPTION options[1];
        unsigned long listSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

        options[0].dwOption = INTERNET_PER_CONN_FLAGS;
        options[0].Value.dwValue = PROXY_TYPE_DIRECT;

        list.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
        list.pszConnection = NULL;
        list.dwOptionCount = 1;
        list.dwOptionError = 0;
        list.pOptions = options;

        InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, listSize);
        
        // Now set our proxy
        INTERNET_PROXY_INFO proxyInfo;
        ZeroMemory(&proxyInfo, sizeof(proxyInfo));

        proxyInfo.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
        proxyInfo.lpszProxy = (LPCTSTR)"127.0.0.1:8080";
        proxyInfo.lpszProxyBypass = (LPCTSTR)"<local>";

        BOOL result = InternetSetOption(NULL, INTERNET_OPTION_PROXY, &proxyInfo, sizeof(proxyInfo));
        
        // Notify WinINET that settings have changed
        InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
        InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);

        if(result) {
            logMessage("Proxy configuration set successfully to 127.0.0.1:8080");
        } else {
            logMessage("Failed to set proxy configuration");
        }
    }

    static void PatchFortnite(HANDLE hProcess) {
        logMessage("Starting auth bypass patching...");
        
        // Multiple auth bypass patterns for different versions
        const std::vector<std::pair<std::vector<BYTE>, std::vector<BYTE>>> patterns = {
            // Pattern 1
            {{0x74, 0x23, 0x48, 0x8B, 0x4B, 0x78}, {0xEB, 0x23, 0x48, 0x8B, 0x4B, 0x78}},
            // Pattern 2  
            {{0x75, 0x1A, 0x48, 0x8B, 0x45, 0x70}, {0xEB, 0x1A, 0x48, 0x8B, 0x45, 0x70}},
            // Pattern 3
            {{0x74, 0x20, 0x48, 0x8B, 0x4B, 0x78}, {0xEB, 0x20, 0x48, 0x8B, 0x4B, 0x78}}
        };

        bool patchSuccess = false;
        for (const auto& pattern : patterns) {
            DWORD_PTR authAddr = FindPattern(hProcess, 0, (DWORD_PTR)-1, 
                pattern.first.data(), std::string(pattern.first.size(), 'x').c_str());
                
            if (authAddr) {
                DWORD oldProtect;
                if (VirtualProtectEx(hProcess, (LPVOID)authAddr, pattern.second.size(), 
                    PAGE_EXECUTE_READWRITE, &oldProtect)) {
                    
                    SIZE_T bytesWritten;
                    if (WriteProcessMemory(hProcess, (LPVOID)authAddr, pattern.second.data(), 
                        pattern.second.size(), &bytesWritten)) {
                        
                        VirtualProtectEx(hProcess, (LPVOID)authAddr, pattern.second.size(), 
                            oldProtect, &oldProtect);
                            
                        patchSuccess = true;
                        logMessage("Auth bypass patch applied successfully");
                        break;
                    }
                }
            }
        }

        if (!patchSuccess) {
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

        // Kill any existing processes first
        StopServer();
        
        // Setup proxy configuration
        SetupProxy();

        // Start Node.js auth server
        STARTUPINFOW si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOW;

        // Create server process with proper working directory
        WCHAR currentDir[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, currentDir);
        
        logMessage("Starting auth server on port 7777...");
        WCHAR nodeCmd[] = L"node server.js";
        if (!CreateProcessW(NULL, nodeCmd, NULL, NULL, FALSE, 
            CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP, 
            NULL, currentDir, &si, &pi)) {
            MessageBoxW(NULL, L"Failed to start auth server", L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Failed to start auth server!");
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        logMessage("Auth server started successfully");

        // Start proxy server for auth redirection
        logMessage("Starting proxy server on port 8080...");
        WCHAR proxyCmd[] = L"mitmdump -s redirect.py --listen-port 8080 --set block_global=false";
        if (!CreateProcessW(NULL, proxyCmd, NULL, NULL, FALSE, 
            CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP,
            NULL, currentDir, &si, &pi)) {
            MessageBoxW(NULL, L"Failed to start proxy server", L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Failed to start proxy server!");
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        logMessage("Proxy server started successfully");

        // Wait for servers to initialize
        logMessage("Waiting for services to initialize...");
        Sleep(5000); // Increased wait time to ensure services are ready

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

        // Create process with environment block
        LPVOID envBlock = NULL;
        if (!CreateEnvironmentBlock(&envBlock, NULL, FALSE)) {
            logMessage("WARNING: Failed to create environment block");
        }

        BOOL success = CreateProcessW(
            fortnitePath.c_str(),
            (LPWSTR)cmdLine.c_str(),
            NULL,
            NULL,
            FALSE,
            CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP | CREATE_UNICODE_ENVIRONMENT,
            envBlock,
            basePath.c_str(),
            &siGame,
            &piGame
        );

        if (envBlock) {
            DestroyEnvironmentBlock(envBlock);
        }

        if (!success) {
            DWORD error = GetLastError();
            WCHAR errorMsg[256];
            swprintf_s(errorMsg, L"Failed to launch Fortnite. Error code: %d", error);
            MessageBoxW(NULL, errorMsg, L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Failed to launch Fortnite. Error code: " + std::to_string(error));
            return;
        }

        logMessage("Fortnite process created successfully (PID: " + std::to_string(piGame.dwProcessId) + ")");

        // Wait a moment before patching
        Sleep(2000);

        // Patch the process
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

        // Kill processes more gracefully
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe32;
            pe32.dwSize = sizeof(pe32);

            if (Process32FirstW(snapshot, &pe32)) {
                do {
                    if (wcscmp(pe32.szExeFile, L"node.exe") == 0 ||
                        wcscmp(pe32.szExeFile, L"mitmdump.exe") == 0 ||
                        wcscmp(pe32.szExeFile, L"FortniteClient-Win64-Shipping.exe") == 0) {
                        
                        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                        if (hProcess != NULL) {
                            TerminateProcess(hProcess, 0);
                            CloseHandle(hProcess);
                            std::wstring wExeFile = pe32.szExeFile;
                            logMessage("Terminated process: " + std::string(wExeFile.begin(), wExeFile.end()));
                        }
                    }
                } while (Process32NextW(snapshot, &pe32));
            }
            CloseHandle(snapshot);
        }

        // Reset proxy settings
        INTERNET_PER_CONN_OPTION_LIST list;
        INTERNET_PER_CONN_OPTION options[1];
        unsigned long listSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

        options[0].dwOption = INTERNET_PER_CONN_FLAGS;
        options[0].Value.dwValue = PROXY_TYPE_DIRECT;

        list.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
        list.pszConnection = NULL;
        list.dwOptionCount = 1;
        list.dwOptionError = 0;
        list.pOptions = options;

        InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, listSize);
        InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
        InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);

        EnableWindow(startButton, TRUE);
        EnableWindow(stopButton, FALSE);
        EnableWindow(pathEdit, TRUE);
        
        logMessage("All services stopped successfully");
        logMessage("System ready for next session");
    }

    static void LoadSavedPath() {
        std::wifstream file(L"path.txt");
        if (file.is_open()) {
            std::wstring path;
            std::getline(file, path);
            SetWindowTextW(pathEdit, path.c_str());
            file.close();
            logMessage("Loaded saved Fortnite path");
        } else {
            logMessage("No saved path found");
        }
    }

    static void SavePath(const WCHAR* path) {
        std::wofstream file(L"path.txt");
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

        // Also write to log file
        std::ofstream logFile("launcher.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << logLine;
            logFile.close();
        }
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

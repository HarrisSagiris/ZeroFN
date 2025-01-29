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
#pragma comment(lib, "wininet.lib")

namespace fs = std::experimental::filesystem;

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

        consoleOutput = CreateWindowW(L"EDIT", L"", 
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
            10, 85, 760, 400, hwnd, (HMENU)5, NULL, NULL);

        // Load saved path
        LoadSavedPath();
        
        ShowWindow(hwnd, SW_SHOW);
        logMessage("ZeroFN Launcher initialized successfully");
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

    static void StartServer() {
        // Check required files
        if (!fs::exists("server.js") || !fs::exists("redirect.py")) {
            MessageBoxW(NULL, L"Required server files are missing!", L"Error", MB_OK | MB_ICONERROR);
            return;
        }

        // Start Node.js server
        STARTUPINFOW si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        // Start local server
        WCHAR nodeCmd[] = L"node server.js";
        if (!CreateProcessW(NULL, nodeCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            MessageBoxW(NULL, L"Failed to start local server", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        // Configure proxy settings to redirect to local server
        INTERNET_PER_CONN_OPTION_LIST options;
        INTERNET_PER_CONN_OPTION option[3];
        unsigned long listSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

        option[0].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
        option[0].Value.pszValue = const_cast<LPWSTR>(L"127.0.0.1:8080");
        
        option[1].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
        option[1].Value.pszValue = const_cast<LPWSTR>(L"localhost");
        
        option[2].dwOption = INTERNET_PER_CONN_FLAGS;
        option[2].Value.dwValue = PROXY_TYPE_PROXY;

        options.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
        options.pszConnection = NULL;
        options.dwOptionCount = 3;
        options.dwOptionError = 0;
        options.pOptions = option;

        InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &options, listSize);
        InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
        InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);

        // Get Fortnite path
        WCHAR path[MAX_PATH];
        GetWindowTextW(pathEdit, path, MAX_PATH);
        std::wstring basePath(path);
        std::wstring fortnitePath = basePath + L"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe";

        if (!fs::exists(fortnitePath)) {
            MessageBoxW(NULL, L"Fortnite executable not found!", L"Error", MB_OK | MB_ICONERROR);
            return;
        }

        // Launch Fortnite directly with custom parameters
        STARTUPINFOW siGame = {0};
        PROCESS_INFORMATION piGame = {0};
        siGame.cb = sizeof(siGame);

        // Custom launch parameters to bypass Epic launcher and connect to local server
        std::wstring cmdLine = L"\"" + fortnitePath + L"\" -NOSSLPINNING -noeac -fromfl=be -fltoken=7d41f3c07b724575892f0def64c57569 -skippatchcheck -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -nobe -fromfl=eac -fltoken=none -nosound -AUTH_TYPE=epic -AUTH_LOGIN=localhost:3000 -AUTH_PASSWORD=test";
        
        WCHAR* cmdLinePtr = new WCHAR[cmdLine.length() + 1];
        wcscpy_s(cmdLinePtr, cmdLine.length() + 1, cmdLine.c_str());

        BOOL success = CreateProcessW(
            fortnitePath.c_str(),
            cmdLinePtr,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE | CREATE_SUSPENDED,  // Start suspended to inject necessary modifications
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
            return;
        }

        // Resume the process
        ResumeThread(piGame.hThread);

        CloseHandle(piGame.hProcess);
        CloseHandle(piGame.hThread);

        EnableWindow(startButton, FALSE);
        EnableWindow(stopButton, TRUE);
        EnableWindow(pathEdit, FALSE);
        
        logMessage("Fortnite launched successfully with custom configuration");
    }

    static void StopServer() {
        // Reset proxy settings
        INTERNET_PER_CONN_OPTION_LIST options;
        INTERNET_PER_CONN_OPTION option[1];
        unsigned long listSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);

        option[0].dwOption = INTERNET_PER_CONN_FLAGS;
        option[0].Value.dwValue = PROXY_TYPE_DIRECT;

        options.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
        options.pszConnection = NULL;
        options.dwOptionCount = 1;
        options.dwOptionError = 0;
        options.pOptions = option;

        InternetSetOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &options, listSize);
        InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0);
        InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0);

        // Kill processes
        system("taskkill /F /IM node.exe");
        system("taskkill /F /IM FortniteClient-Win64-Shipping.exe");

        EnableWindow(startButton, TRUE);
        EnableWindow(stopButton, FALSE);
        EnableWindow(pathEdit, TRUE);
        
        logMessage("All services stopped and settings restored");
    }

    static void LoadSavedPath() {
        std::ifstream file("path.txt");
        if (file.is_open()) {
            std::string path;
            std::getline(file, path);
            SetWindowTextA(pathEdit, path.c_str());
            file.close();
        }
    }

    static void SavePath(const WCHAR* path) {
        std::wofstream file("path.txt");
        if (file.is_open()) {
            file << path;
            file.close();
            logMessage("Path saved successfully");
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

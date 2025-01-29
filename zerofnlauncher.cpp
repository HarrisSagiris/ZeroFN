#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <sstream>
#include <experimental/filesystem>
#include <shlobj.h>

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
        // Show folder browser dialog with improved UI
        BROWSEINFOW bi = {0};
        bi.lpszTitle = L"Select Fortnite Installation Directory";
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        bi.lpfn = BrowseCallbackProc;
        
        LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
        if (pidl != 0) {
            WCHAR path[MAX_PATH];
            SHGetPathFromIDListW(pidl, path);
            
            // Validate that this is a Fortnite directory
            std::wstring selectedPath(path);
            if (ValidateFortniteDirectory(selectedPath)) {
                SetWindowTextW(pathEdit, path);
                SavePath(path);
                logMessage("Valid Fortnite directory selected");
            } else {
                MessageBoxW(NULL, L"Selected folder does not contain Fortnite installation.\nPlease select the main Fortnite folder.", 
                    L"Invalid Directory", MB_OK | MB_ICONWARNING);
            }

            IMalloc * imalloc = 0;
            if (SUCCEEDED(SHGetMalloc(&imalloc))) {
                imalloc->Free(pidl);
                imalloc->Release();
            }
        }
    }

    static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
        if (uMsg == BFFM_INITIALIZED) {
            // Set initial directory if previously saved
            std::wstring savedPath = LoadLastPath();
            if (!savedPath.empty()) {
                SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)savedPath.c_str());
            }
        }
        return 0;
    }

    static bool ValidateFortniteDirectory(const std::wstring& path) {
        // Check for common Fortnite folders/files to validate directory
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

        WCHAR nodeCmd[] = L"node server.js";
        if (!CreateProcessW(NULL, nodeCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            MessageBoxW(NULL, L"Failed to start Node.js server", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        // Start mitmproxy
        WCHAR mitmCmd[] = L"mitmdump -s redirect.py";
        if (!CreateProcessW(NULL, mitmCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            MessageBoxW(NULL, L"Failed to start mitmproxy", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        // Get Fortnite path and validate
        WCHAR path[MAX_PATH];
        GetWindowTextW(pathEdit, path, MAX_PATH);
        std::wstring basePath(path);
        std::wstring fortnitePath = basePath + L"\\FortniteGame\\Binaries\\Win64\\FortniteClient-Win64-Shipping.exe";

        if (!fs::exists(fortnitePath)) {
            MessageBoxW(NULL, L"Fortnite executable not found! Please verify the installation directory.", L"Error", MB_OK | MB_ICONERROR);
            return;
        }

        // Launch Fortnite with improved process creation
        STARTUPINFOW siGame = {0};
        PROCESS_INFORMATION piGame = {0};
        siGame.cb = sizeof(siGame);
        
        std::wstring cmdLine = L"\"" + fortnitePath + L"\" -noeac -fromfl=be -fltoken=h1h4370717422124b232377 -skippatchcheck";
        
        WCHAR* cmdLinePtr = new WCHAR[cmdLine.length() + 1];
        wcscpy_s(cmdLinePtr, cmdLine.length() + 1, cmdLine.c_str());

        BOOL success = CreateProcessW(
            NULL,
            cmdLinePtr,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE | DETACHED_PROCESS,
            NULL,
            basePath.c_str(), // Set working directory to Fortnite base path
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

        CloseHandle(piGame.hProcess);
        CloseHandle(piGame.hThread);

        EnableWindow(startButton, FALSE);
        EnableWindow(stopButton, TRUE);
        EnableWindow(pathEdit, FALSE);
        
        logMessage("Fortnite launched successfully");
    }

    static void StopServer() {
        // Kill processes
        system("taskkill /F /IM node.exe");
        system("taskkill /F /IM mitmdump.exe");
        system("taskkill /F /IM FortniteClient-Win64-Shipping.exe");

        EnableWindow(startButton, TRUE); 
        EnableWindow(stopButton, FALSE);
        EnableWindow(pathEdit, TRUE);
        
        logMessage("All services stopped");
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

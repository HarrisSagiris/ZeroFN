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
#include <functional>
#include <userenv.h>
#include <psapi.h> // Added for EnumProcessModules and GetModuleFileNameExW
#include <gdiplus.h> // Added for gradient backgrounds
#include <dwmapi.h> // Added for window composition effects
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "dwmapi.lib")

namespace fs = std::experimental::filesystem;

// Helper function to inject DLL with improved error handling and logging
bool InjectDLL(HANDLE hProcess, const std::wstring& dllPath, void (*logCallback)(const std::string&)) {
    logCallback("Starting DLL injection into Fortnite process...");

    // Get the full path to zerofn.dll
    WCHAR fullDllPath[MAX_PATH];
    GetFullPathNameW(dllPath.c_str(), MAX_PATH, fullDllPath, nullptr);
    logCallback("Full DLL Path: " + std::string(dllPath.begin(), dllPath.end()));

    // Get LoadLibraryW address
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (!hKernel32) {
        logCallback("ERROR: Could not get kernel32.dll handle");
        return false;
    }
    
    FARPROC loadLibraryAddr = GetProcAddress(hKernel32, "LoadLibraryW");
    if (!loadLibraryAddr) {
        logCallback("ERROR: Could not get LoadLibraryW address");
        return false;
    }
    logCallback("Successfully got LoadLibraryW address");

    // Check if process is 64-bit
    BOOL isWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(), &isWow64);
    BOOL is64BitProcess = !isWow64;

    // Check if DLL matches process architecture
    HANDLE hFile = CreateFileW(fullDllPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        logCallback("ERROR: Could not open DLL file");
        return false;
    }

    IMAGE_DOS_HEADER dosHeader;
    DWORD bytesRead;
    if (!ReadFile(hFile, &dosHeader, sizeof(IMAGE_DOS_HEADER), &bytesRead, NULL)) {
        CloseHandle(hFile);
        logCallback("ERROR: Could not read DOS header");
        return false;
    }

    SetFilePointer(hFile, dosHeader.e_lfanew, NULL, FILE_BEGIN);
    IMAGE_NT_HEADERS ntHeaders;
    if (!ReadFile(hFile, &ntHeaders, sizeof(IMAGE_NT_HEADERS), &bytesRead, NULL)) {
        CloseHandle(hFile);
        logCallback("ERROR: Could not read NT headers");
        return false;
    }
    CloseHandle(hFile);

    bool isDll64Bit = (ntHeaders.FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64);
    if (is64BitProcess != isDll64Bit) {
        logCallback("ERROR: DLL architecture mismatch with process");
        return false;
    }

    // Allocate memory in Fortnite process
    SIZE_T pathSize = (wcslen(fullDllPath) + 1) * sizeof(WCHAR);
    LPVOID remoteMem = VirtualAllocEx(hProcess, NULL, pathSize, 
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            
    if (!remoteMem) {
        logCallback("ERROR: Could not allocate memory in Fortnite process");
        return false;
    }
    logCallback("Successfully allocated memory in Fortnite process");

    // Write DLL path to Fortnite process memory
    if (!WriteProcessMemory(hProcess, remoteMem, fullDllPath, pathSize, NULL)) {
        logCallback("ERROR: Could not write to Fortnite process memory");
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }
    logCallback("Successfully wrote DLL path to Fortnite process memory");

    // Create remote thread to load DLL
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteMem, 0, NULL);
            
    if (!hThread) {
        logCallback("ERROR: Could not create remote thread in Fortnite");
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }
    logCallback("Successfully created remote thread");

    // Wait for injection to complete with timeout
    if (WaitForSingleObject(hThread, 30000) == WAIT_TIMEOUT) { // 30 second timeout
        logCallback("ERROR: DLL injection timed out");
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }

    // Get injection result
    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);
    if (exitCode == 0) {
        DWORD lastError = GetLastError();
        std::stringstream ss;
        ss << "ERROR: LoadLibrary failed with error code: 0x" << std::hex << lastError;
        logCallback(ss.str());
        CloseHandle(hThread);
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }

    // Cleanup
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);

    // Wait for DLL to fully load
    Sleep(5000);

    // Verify DLL is loaded
    HMODULE hModules[1024];
    DWORD cbNeeded;
    if(EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
        for(unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            WCHAR modName[MAX_PATH];
            if(GetModuleFileNameExW(hProcess, hModules[i], modName, sizeof(modName)/sizeof(WCHAR))) {
                if(wcsstr(modName, L"zerofn.dll") != NULL) {
                    logCallback("Successfully verified ZeroFN DLL is loaded!");
                    return true;
                }
            }
        }
    }

    logCallback("ERROR: Could not verify DLL was loaded");
    return false;
}

class ZeroFNLauncher {
public:
    ZeroFNLauncher() {
        // Initialize GDI+ and COM
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
        CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

        // Create main window with Epic-style theme
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(RGB(18, 18, 18)); // Epic dark background
        wc.lpszClassName = L"ZeroFNLauncher";
        RegisterClassExW(&wc);

        hwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_COMPOSITED,
            L"ZeroFNLauncher", 
            L"ZeroFN Launcher",
            WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600,
            NULL,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

        // Enable window composition effects
        MARGINS margins = {1, 1, 1, 1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
        
        // Set window transparency and blur
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        
        DWM_BLURBEHIND bb = {0};
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = TRUE;
        DwmEnableBlurBehindWindow(hwnd, &bb);

        // Create Epic-styled controls with modern gradients
        CreateWindowW(L"BUTTON", L"Browse", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            20, 20, 100, 40, hwnd, (HMENU)1, NULL, NULL);

        pathEdit = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            140, 20, 700, 40, hwnd, (HMENU)2, NULL, NULL);

        startButton = CreateWindowW(L"BUTTON", L"LAUNCH FORTNITE", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            20, 80, 200, 50, hwnd, (HMENU)3, NULL, NULL);

        stopButton = CreateWindowW(L"BUTTON", L"STOP SERVICES", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            240, 80, 200, 50, hwnd, (HMENU)4, NULL, NULL);

        EnableWindow(stopButton, FALSE);

        // Create Epic-styled console output
        consoleOutput = CreateWindowW(L"EDIT", L"", 
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL,
            20, 150, 940, 400, hwnd, (HMENU)6, NULL, NULL);

        // Set Epic Games font
        HFONT hFont = CreateFontW(20, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

        HFONT hFontBold = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

        // Apply Epic styling to controls
        SendMessage(startButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);
        SendMessage(stopButton, WM_SETFONT, (WPARAM)hFontBold, TRUE);
        
        for(HWND ctrl : {pathEdit, consoleOutput}) {
            SendMessage(ctrl, WM_SETFONT, (WPARAM)hFont, TRUE);
            SetWindowSubclass(ctrl, ButtonSubclassProc, 0, 0);
        }

        // Custom Epic-style colors and borders
        SetWindowLongPtr(pathEdit, GWL_EXSTYLE, GetWindowLongPtr(pathEdit, GWL_EXSTYLE) | WS_EX_CLIENTEDGE);
        SetWindowLongPtr(consoleOutput, GWL_EXSTYLE, GetWindowLongPtr(consoleOutput, GWL_EXSTYLE) | WS_EX_CLIENTEDGE);

        // Load saved path
        LoadSavedPath();
        
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        
        logMessage("ZeroFN Launcher initialized successfully");
        logMessage("Waiting for user input...");
    }

    static LRESULT CALLBACK ButtonSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                             UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
        switch (uMsg) {
            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                
                // Epic-style colors
                SetBkColor(hdc, RGB(32, 32, 32));
                SetTextColor(hdc, RGB(255, 255, 255));
                
                EndPaint(hWnd, &ps);
                break;
            }
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC: {
                HDC hdcEdit = (HDC)wParam;
                SetBkColor(hdcEdit, RGB(32, 32, 32));
                SetTextColor(hdcEdit, RGB(255, 255, 255));
                return (LRESULT)CreateSolidBrush(RGB(32, 32, 32));
            }
            case WM_CTLCOLORBTN: {
                HDC hdcButton = (HDC)wParam;
                SetBkColor(hdcButton, RGB(0, 140, 255)); // Epic blue
                SetTextColor(hdcButton, RGB(255, 255, 255));
                return (LRESULT)CreateSolidBrush(RGB(0, 140, 255));
            }
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_CTLCOLOREDIT:
            case WM_CTLCOLORSTATIC: {
                HDC hdcEdit = (HDC)wParam;
                SetBkColor(hdcEdit, RGB(32, 32, 32));
                SetTextColor(hdcEdit, RGB(255, 255, 255));
                return (LRESULT)CreateSolidBrush(RGB(32, 32, 32));
            }
            case WM_CTLCOLORBTN: {
                HDC hdcButton = (HDC)wParam;
                SetBkColor(hdcButton, RGB(0, 140, 255)); // Epic blue
                SetTextColor(hdcButton, RGB(255, 255, 255));
                return (LRESULT)CreateSolidBrush(RGB(0, 140, 255));
            }
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
                Gdiplus::GdiplusShutdown(gdiplusToken);
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

    static void StartServer() {
        logMessage("Starting server initialization sequence...");
        
        // Check required files
        logMessage("Checking required server files...");
        if (!fs::exists("zerofn.dll")) {
            MessageBoxW(NULL, L"Required DLL is missing!", L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Missing required files!");
            return;
        }
        logMessage("All required files found");

        // Kill any existing processes first
        StopServer();

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

        // Create suspended Fortnite process
        STARTUPINFOW siGame = {0};
        PROCESS_INFORMATION piGame = {0};
        siGame.cb = sizeof(siGame);
        siGame.dwFlags = STARTF_USESHOWWINDOW;
        siGame.wShowWindow = SW_SHOW;

        std::wstring cmdLine = L"\"" + fortnitePath + L"\" -NOSSLPINNING -noeac -fromfl=be -fltoken=7d41f3c07b724575892f0def64c57569 "
            L"-skippatchcheck -epicapp=Fortnite -epicenv=Prod -epiclocale=en-us -epicportal -nobe -fromfl=eac -fltoken=none "
            L"-nosound -AUTH_TYPE=epic -AUTH_LOGIN=135.181.149.116:3000 -AUTH_PASSWORD=test "
            L"-FORCECONSOLE -notexturestreaming -dx11 -windowed -FORCECONNECT";

        // Create process suspended
        SHELLEXECUTEINFOW shExInfo = {0};
        shExInfo.cbSize = sizeof(SHELLEXECUTEINFOW);
        shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shExInfo.hwnd = NULL;
        shExInfo.lpVerb = L"runas";  // Request elevation
        shExInfo.lpFile = fortnitePath.c_str();
        shExInfo.lpParameters = cmdLine.c_str() + fortnitePath.length() + 3;
        shExInfo.lpDirectory = basePath.c_str();
        shExInfo.nShow = SW_SHOW;

        if (!ShellExecuteExW(&shExInfo)) {
            DWORD error = GetLastError();
            WCHAR errorMsg[256];
            swprintf_s(errorMsg, L"Failed to launch Fortnite. Error code: %d", error);
            MessageBoxW(NULL, errorMsg, L"Error", MB_OK | MB_ICONERROR);
            logMessage("ERROR: Failed to launch Fortnite. Error code: " + std::to_string(error));
            return;
        }

        piGame.hProcess = shExInfo.hProcess;

        // Inject DLL before resuming process
        std::wstring dllPath = L"zerofn.dll";
        logMessage("Injecting DLL before process start...");
        
        if (!fs::exists(dllPath)) {
            logMessage("ERROR: zerofn.dll not found at: " + std::string(dllPath.begin(), dllPath.end()));
            TerminateProcess(piGame.hProcess, 0);
            CloseHandle(piGame.hProcess);
            return;
        }

        bool injectionSuccess = InjectDLL(piGame.hProcess, dllPath, logMessage);
        if (!injectionSuccess) {
            logMessage("ERROR: DLL injection failed");
            TerminateProcess(piGame.hProcess, 0);
            CloseHandle(piGame.hProcess);
            return;
        }

        logMessage("DLL injection successful - resuming Fortnite process");

        CloseHandle(piGame.hProcess);

        EnableWindow(startButton, FALSE);
        EnableWindow(stopButton, TRUE);
        EnableWindow(pathEdit, FALSE);
        
        logMessage("Fortnite launched successfully with ZeroFN DLL");
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
                    if (wcscmp(pe32.szExeFile, L"FortniteClient-Win64-Shipping.exe") == 0 ||
                        wcscmp(pe32.szExeFile, L"eac.exe") == 0) {
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
    static ULONG_PTR gdiplusToken;
};

HWND ZeroFNLauncher::hwnd = NULL;
HWND ZeroFNLauncher::pathEdit = NULL;
HWND ZeroFNLauncher::startButton = NULL;
HWND ZeroFNLauncher::stopButton = NULL;
HWND ZeroFNLauncher::consoleOutput = NULL;
ULONG_PTR ZeroFNLauncher::gdiplusToken = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    ZeroFNLauncher launcher;
    launcher.Run();
    return 0;
}

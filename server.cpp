#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h> // For InetPton
#include <windows.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <chrono>
#include <filesystem> // Changed from experimental/filesystem
#include <direct.h>
#include <map>
#include <mutex>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <TlHelp32.h>
#include <Psapi.h>
#include <dwmapi.h>
#include <richedit.h> // Added for MSFTEDIT_CLASS

// ZeroFN Version 1.1
// Developed by DevHarris
// A private server implementation for Fortnite

namespace fs = std::filesystem;

// Game session data
struct GameSession {
    std::string sessionId;
    std::vector<std::string> players;
    bool inProgress;
    std::string playlistId;
};

// Custom window procedure for patcher window
LRESULT CALLBACK PatcherWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Create a rich edit control for logs
            HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
            LoadLibrary("Msftedit.dll");
            CreateWindowEx(0, L"RICHEDIT50W", L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
                10, 10, 780, 580, hwnd, NULL, hInstance, NULL);
            return 0;
        }
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

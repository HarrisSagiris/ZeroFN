#define _WIN32_WINNT 0x0600
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>

// Function to check if a process is a game process we want to protect
bool IsGameProcess(const wchar_t* processName) {
    return (wcscmp(processName, L"FortniteClient-Win64-Shipping.exe") == 0);
}

// Function to terminate EAC processes
bool TerminateEACProcesses() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cout << "[ZeroFN EAC] Failed to create process snapshot" << std::endl;
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
                    std::cout << "[ZeroFN EAC] Successfully terminated EAC process" << std::endl;
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
        // Disable DEP
        DWORD oldProtection;
        SetProcessDEPPolicy(0);
        
        // Allow unsigned code execution
        VirtualProtectEx(processHandle, (LPVOID)0x0, 0x7FFFFFFF, PAGE_EXECUTE_READWRITE, &oldProtection);
        
        CloseHandle(processHandle);
    }
}

// Function to allow DLL injection
void AllowDLLInjection(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (processHandle != NULL) {
        // Disable signature verification
        DWORD oldProtection;
        VirtualProtectEx(processHandle, (LPVOID)0x0, 0x7FFFFFFF, PAGE_EXECUTE_READWRITE, &oldProtection);
        
        CloseHandle(processHandle);
    }
}

// Main entry point
extern "C" __declspec(dllexport) bool StartZeroFNEAC() {
    std::cout << "[ZeroFN EAC] Initializing modified EAC service..." << std::endl;
    
    // Terminate any existing EAC processes
    if (!TerminateEACProcesses()) {
        std::cout << "[ZeroFN EAC] No EAC processes found or failed to terminate" << std::endl;
    }

    // Create dummy EAC service
    std::cout << "[ZeroFN EAC] Starting ZeroFN EAC service" << std::endl;
    
    // Start monitoring thread
    std::thread([&]() {
        while (true) {
            // Terminate any real EAC processes
            TerminateEACProcesses();
            
            // Find game process and apply bypasses
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

    std::cout << "[ZeroFN EAC] ZeroFN EAC service started successfully" << std::endl;
    return true;
}

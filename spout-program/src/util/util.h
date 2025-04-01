#pragma once
#include <string>
#include <iostream>
#include <windows.h>

std::string ReadFile(const std::string& filepath);

inline int RegisterSoftcam() {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    char command[31] = "cmd.exe /C RegisterSoftcam.bat";
    if (!CreateProcessA(
        NULL, // Path to the executable
        command,           // Command line arguments
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        NULL,// No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)            // Pointer to PROCESS_INFORMATION structure
        ) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ")\n";
        return 1;
    }

    std::cout << "Process created with ID: " << pi.dwProcessId << std::endl;

    // Optionally wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
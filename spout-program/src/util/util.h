#pragma once
#include <string>
#include <iostream>
#include <windows.h>

std::string ReadFile(const std::string& filepath);

static inline int _RegisterSoftcam(const std::string& args) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessA(
        "C:\\Windows\\system32\\cmd.exe", // Path to the executable
        const_cast<LPSTR>(args.c_str()),           // Command line arguments
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        NULL/*CREATE_NO_WINDOW*/,// No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)            // Pointer to PROCESS_INFORMATION structure
        ) 
    {
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

inline int RegisterSoftcam() {
    return _RegisterSoftcam("cmd /c SoftcamInstaller.exe register softcam.dll");
}

inline int DeregisterSoftcam() {
    return _RegisterSoftcam("cmd /c SoftcamInstaller.exe unregister softcam.dll");
}

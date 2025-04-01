#include <windows.h>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>
#include "CamInstall.h"

/*
MIT License

Copyright (c) 2020 tshino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#if defined(DEBUG)
#define DLLPATH "softcamd.dll"
#else
#define DLLPATH "softcam.dll"
#endif

void Message(const std::string& message)
{
    MessageBoxA(NULL, message.c_str(), "Softcam Installer", MB_OK);
}

std::string ToHex(long x)
{
    char buff[128];
    std::snprintf(buff, sizeof(buff), "%08lx\n", x);
    return buff;
}

HMODULE LoadDLL(const std::string& path)
{
    HMODULE hmod = LoadLibraryA(path.c_str());
    if (!hmod)
    {
        Message("Error: can't load DLL");
        std::exit(1);
    }
    return hmod;
}

template <typename Func>
Func* GetProc(HMODULE hmod, const std::string& name)
{
    Func* func = (Func*)GetProcAddress(hmod, name.c_str());
    if (!func)
    {
        Message("Error: can't find function " + name + " in DLL");
        std::exit(1);
    }
    return func;
}

int RegisterSoftcam() {
    auto hmod = LoadDLL(DLLPATH);
    auto RegisterServer = GetProc<HRESULT STDAPICALLTYPE()>(hmod, "DllRegisterServer");

    auto hr = RegisterServer();

    if (FAILED(hr))
    {
        std::cout << "Failed to register softcam" << std::endl;
        return 1;
    }

    return 0;
}

int DeregisterSoftcam() {
    auto hmod = LoadDLL(DLLPATH);
    auto UnregisterServer = GetProc<HRESULT STDAPICALLTYPE()>(hmod, "DllUnregisterServer");

    auto hr = UnregisterServer();

    if (FAILED(hr))
    {
        std::cout << "Failed to deregister softcam" << std::endl;
        return 1;
    }

    return 0;
}

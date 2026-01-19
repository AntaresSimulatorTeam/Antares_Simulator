// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/sys/appdata.h"
#ifdef YUNI_OS_WINDOWS
#include <Shlobj.h>

#include <yuni/core/system/windows.hdr.h>
#endif
#include <ostream>

#include <yuni/core/system/environment.h>

using namespace Yuni;

namespace OperatingSystem
{
#ifdef YUNI_OS_WINDOWS
static inline bool ConvertWideCharIntoString(String& out, const wchar_t* wpath)
{
    int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wpath, -1, nullptr, 0, nullptr, nullptr);
    if (sizeRequired <= 0)
    {
        return false;
    }

    out.reserve((uint)sizeRequired + 1);
    WideCharToMultiByte(CP_UTF8, 0, wpath, -1, (char*)out.data(), sizeRequired, nullptr, nullptr);
    out.resize(((uint)sizeRequired) - 1);
    return true;
}
#endif

bool FindLocalAppData(Yuni::String& out, bool allusers)
{
    out.clear();

#ifdef YUNI_OS_WINDOWS
    typedef HRESULT WINAPI SHGetFolderPathWFn(HWND hwndOwner,
                                              int nFolder,
                                              HANDLE hToken,
                                              DWORD dwFlags,
                                              __out LPTSTR pszPath);

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32)
    {
        // Windows XP
        SHGetFolderPathWFn* folderPathW = (SHGetFolderPathWFn*)(GetProcAddress(hShell32,
                                                                               "SHGetFolderPathW"));
        if (folderPathW)
        {
            wchar_t* wpath = new wchar_t[MAX_PATH + 1];
            if (!wpath)
            {
                return false;
            }
            folderPathW(0,
                        (allusers ? CSIDL_COMMON_APPDATA : CSIDL_LOCAL_APPDATA),
                        nullptr,
                        0,
                        wpath);
            if (ConvertWideCharIntoString(out, wpath))
            {
                delete[] wpath;
                return true;
            }
            delete[] wpath;
        }
    }
#else

    if (allusers)
    {
        out = "/etc";
        return true;
    }
    else
    {
        System::Environment::Read("HOME", out);
        if (not out.empty())
        {
            return true;
        }
    }
#endif
    return false;
}

bool FindAntaresLocalAppData(Yuni::String& out, bool allusers)
{
    if (not FindLocalAppData(out, allusers))
    {
        return false;
    }

#ifdef YUNI_OS_WINDOWS
    out += "\\RTE\\antares";
#else
    if (allusers)
    {
        out += "/antares";
    }
    else
    {
        // ~/.antares is already taken by wx - recent files
        out += "/.antares-data"; // hidden from home
    }
#endif

    return true;
}

} // namespace OperatingSystem

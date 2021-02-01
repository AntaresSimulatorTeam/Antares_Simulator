/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#include "environment.h"
#include "windows.hdr.h"
#include "../../string.h"
#ifdef YUNI_HAS_STDLIB_H
#include <stdlib.h>
#endif
#include "../string/wstring.h"

namespace Yuni
{
namespace System
{
namespace Environment
{
namespace // anonymous
{
template<class StringT>
inline bool ReadImpl(const AnyString& name, StringT& out, bool emptyBefore)
{
    if (emptyBefore)
        out.clear();

#ifdef YUNI_OS_WINDOWS
    {
        WString nameUTF16(name);

        DWORD size = GetEnvironmentVariableW(nameUTF16.c_str(), nullptr, 0);
        if (size > 0 and size <= 32767) // windows hard-coded value
        {
            // allocating a new buffer for receiving the value
            wchar_t* buffer = (wchar_t*)::malloc(sizeof(wchar_t) * size);
            if (buffer)
            {
                GetEnvironmentVariableW(nameUTF16.c_str(), buffer, size);
                if (size != 0)
                {
                    int sizeRequired
                      = WideCharToMultiByte(CP_UTF8, 0, buffer, (int)size - 1, NULL, 0, NULL, NULL);
                    if (sizeRequired > 0)
                    {
                        out.reserve(out.size() + sizeRequired);
                        int sizeRequired = WideCharToMultiByte(CP_UTF8,
                                                               0,
                                                               buffer,
                                                               (int)size - 1,
                                                               out.data() + out.size(),
                                                               size,
                                                               NULL,
                                                               NULL);
                        ::free(buffer);
                        out.resize(out.size() + (uint)sizeRequired);
                        return true;
                    }
                }
                ::free(buffer);
            }
        }
    }
#else
    {
#ifdef YUNI_HAS_STDLIB_H
        const char* e = ::getenv(name.c_str());
        if (e and '\0' != *e)
        {
            out += e;
            return true;
        }
        return false;

#else
#error missing implementation
        assert(false and "missing implementation");
#endif
    }
#endif // windows

    return false; // fallback
}

} // anonymous namespace

bool ReadAsBool(const AnyString& name)
{
#ifdef YUNI_OS_WINDOWS
    {
        String out;
        ReadImpl(name, out, false);
        if (not out.empty())
            return out.to<bool>();
    }
#else
    {
#ifdef YUNI_HAS_STDLIB_H
        AnyString value = ::getenv(name.c_str());
        if (not value.empty())
            return value.to<bool>();
#else
#error not implemented
#endif
    }
#endif
    return false;
}

yint64 ReadAsInt64(const AnyString& name, yint64 defvalue)
{
#ifdef YUNI_OS_WINDOWS
    {
        String out;
        ReadImpl(name, out, false);
        if (not out.empty())
            return out.to<yint64>();
    }
#else
    {
#ifdef YUNI_HAS_STDLIB_H
        AnyString value = ::getenv(name.c_str());
        if (not value.empty())
            return value.to<yint64>();
#else
#error not implemented
#endif
    }
#endif
    return defvalue;
}

String Read(const AnyString& name)
{
#ifdef YUNI_OS_WINDOWS
    {
        String out;
        ReadImpl(name, out, false);
        return out;
    }
#else
    {
#ifdef YUNI_HAS_STDLIB_H
        return ::getenv(name.c_str());
#else
#error not implemented
#endif
    }
#endif
    return String(); // fallback
}

bool Read(const AnyString& name, Clob& out, bool emptyBefore)
{
    return ReadImpl(name, out, emptyBefore);
}

bool Read(const AnyString& name, String& out, bool emptyBefore)
{
    return ReadImpl(name, out, emptyBefore);
}

} // namespace Environment
} // namespace System
} // namespace Yuni

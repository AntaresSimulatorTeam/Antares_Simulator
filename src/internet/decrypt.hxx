/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LICENSE_DECRYPT_HXX__
#define __ANTARES_LICENSE_DECRYPT_HXX__

#include <yuni/yuni.h>
#include "license.h"

#include <yuni/core/system/cpu.h>
#include <yuni/core/system/memory.h>
#include <antares/logs.h>

#include "../config.h"
#include "keys/allkeys.hxx"

// Debug mode for the license manager
#ifndef NDEBUG
#define LICENSE_DEBUG(x) logs.debug() << "LICENSE: " << x
#else
#define LICENSE_DEBUG(x)
#endif

#ifdef YUNI_OS_WINDOWS
#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>
#include <winsock2.h>
#pragma comment(lib, "wbemuuid.lib")
#endif

namespace Antares
{
namespace License
{
typedef unsigned char uchar;
#ifdef YUNI_OS_WINDOWS

static bool WideCharIntoString(Yuni::String& out, const wchar_t* wcstr)
{
    if (not wcstr)
    {
        out.clear();
        return false;
    }
    int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wcstr, -1, nullptr, 0, nullptr, nullptr);
    if (sizeRequired <= 0)
    {
        out.clear();
        return false;
    }
    out.reserve((unsigned int)sizeRequired + 1);
    WideCharToMultiByte(CP_UTF8, 0, wcstr, -1, (char*)out.data(), sizeRequired, nullptr, nullptr);
    out.resize(((unsigned int)sizeRequired) - 1);
    return true;
}
#endif

template<class T>
static const char* IndexToCString(T index)
{
    switch (index)
    {
        // the most common case
    case 0:
        return nullptr;
        // possible cases
    case 1:
        return ".1";
    case 2:
        return ".2";
    case 3:
        return ".3";
    case 4:
        return ".4";
    case 5:
        return ".5";
    default:
        return ".*";
    }
}

template<class StringT>
static bool Encrypt(StringT& out, const AnyString& in, const EncryptionKey& key)
{
    return true;
}

template<class StringT>
static bool Decrypt(StringT& out, const AnyString& in, const EncryptionKey& key)
{
    return false;
}

} // namespace License
} // namespace Antares

using namespace Antares;

namespace // anonymous
{
/*!
** \brief Read a host key and extract informations into a property set
**
** A hostkey is something like that :
** \code
** -----BEGIN LICENSE REQUEST KEY-----
** UBBVAxNlnxAxSyeUGKmGVqQLiMGFIRD+P1Wrt54Lvt53MZAw/DkyMk4hDdSX+HnwVlcjHl+eviaHdVlGK
** E0zHJK4HjGAHcR0SK5FUQ3Q4KjDYgZTaI5j6t8YitWpeoym9PKwKAmTiZlRZocZLLJ8t+sPTao+R4M5RM
** Ykcx1diDU=-voJ6mjlvrIzOVwk3nzXlhvgKkPE9Vmh59piwCEOiWuboqlpIaJ4PQz0rhTo5dZiUFETPst
** QsMrVrGsgCX0g7JoPdVaflsvQkljqMzu+jX2rilg52iI/pH3Bhks+FlfQdexvdzdFcVeYRzYHCKIDEFQ8
** yvNJaWSnK/kgxtZszxMpoi9T32K1JHx2q6186l73pcQnr4+kqVxWv4Gzc9lmh+A==
** -----END LICENSE REQUEST KEY-----
** \endcode
*/
template<bool ForHostKey, class StringT>
static bool DecodeAntaresKey(License::Properties& properties, StringT& content)
{
    return true;
}

} // anonymous namespace

#endif // __ANTARES_LICENSE_DECRYPT_HXX__


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
#pragma once
#include "directory.h"

namespace Yuni::IO::Directory
{
// Forward declaration
template<typename C>
struct Constant;

} // namespace Yuni::IO::Directory

namespace Yuni::IO
{
inline bool IsRelative(const AnyString& filename)
{
    return not IsAbsolute(filename);
}

template<class StringT>
bool ExtractExtension(StringT& out, const AnyString& filename, bool dot, bool clear)
{
    if (clear)
    {
        out.clear();
    }
    // If the string is empty, the buffer may be invalid (NULL)
    if (filename.size())
    {
        uint i = filename.size();
        do
        {
            --i;
            switch (filename[i])
            {
            case '.':
            {
                if (not dot)
                {
                    if (++i >= static_cast<uint>(filename.size()))
                    {
                        return true;
                    }
                }
                out.append(filename.c_str() + i, filename.size() - i);
                return true;
            }
            case '/':
            case '\\':
                return false;
            }
        } while (i != 0);
    }
    return false;
}

template<class StringT>
void MakeAbsolute(StringT& out, const AnyString& filename, bool clearBefore)
{
    if (clearBefore)
    {
        out.clear();
    }
    if (IsAbsolute(filename))
    {
        out += filename;
    }
    else
    {
        IO::Directory::Current::Get(out, clearBefore);
        out << IO::Separator << filename;
    }
}

template<class StringT>
void MakeAbsolute(StringT& out,
                  const AnyString& filename,
                  const AnyString& currentPath,
                  bool clearBefore)
{
    if (clearBefore)
    {
        out.clear();
    }
    if (IsAbsolute(filename))
    {
        out += filename;
    }
    else
    {
        out += currentPath;
        out += IO::Separator;
        out += filename;
    }
}

template<class StringT1, class StringT2>
bool ReplaceExtension(StringT1& filename, const StringT2& newExtension)
{
    // If the string is empty, the buffer may be invalid (NULL)
    if (filename.size())
    {
        uint i = filename.size();
        do
        {
            --i;
            switch (filename[i])
            {
            case '.':
            {
                filename.resize(i);
                filename += newExtension;
                return true;
            }
            case '/':
            case '\\':
                return false;
            }
        } while (i != 0);
    }
    return false;
}

} // namespace Yuni::IO

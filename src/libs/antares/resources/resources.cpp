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

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "resources.h"
#include "../logs.h"
#include "../config.h"

using namespace Yuni;

#define SEP IO::Separator

namespace // anonymous
{
//! All search paths
static String::Vector SearchPaths;

//! The root folder
static String RootFolder;

} // anonymous namespace

namespace Antares
{
namespace Resources
{
bool FindFile(Yuni::String& out, const AnyString& filename)
{
    if (not filename.empty())
    {
        String tmp;

        uint count = (uint)SearchPaths.size();
        for (uint i = 0; i != count; ++i)
        {
            tmp.clear() << SearchPaths[i] << filename;
            IO::Normalize(out, tmp);

            if (IO::File::Exists(out))
                return true;
        }
    }
    out.clear();
    return false;
}

void GetRootFolder(Yuni::String& out)
{
    out = RootFolder;
}

void WriteRootFolderToLogs()
{
    logs.info() << "  :: from " << RootFolder;
}

bool FindExampleFolder(Yuni::String& folder)
{
    // Temporary string
    String s;
    s.reserve(folder.size() + 50);

#ifdef YUNI_OS_WINDOWS
    // First guess - typical win32 install
    s.clear() << RootFolder << SEP << ".." << SEP << "examples";
    IO::Normalize(folder, s);
    if (IO::Directory::Exists(folder))
        return true;
#else
    // First guess - typical unix install
    s.clear() << RootFolder << SEP << ".." << SEP << "examples";
    s.clear() << "/usr/share/antares/" << ANTARES_VERSION << "/examples";
    IO::Normalize(folder, s);
    if (IO::Directory::Exists(folder))
        return true;
#endif

    // Second guess : Dev mode, from the source repository
    s.clear() << RootFolder << SEP << ".." << SEP << ".." << SEP << ".." << SEP << "resources"
              << SEP << "examples";
    IO::Normalize(folder, s);
    if (IO::Directory::Exists(folder))
        return true;

// Other guesses, Dev mode, special folders when ran from the SVN
// repository and from Visual Studio
#ifdef YUNI_OS_MSVC
    if (Yuni::Logs::Verbosity::Debug::enabled)
    {
        s.clear() << RootFolder << "\\..\\Debug\\..\\..\\..\\..\\resources\\examples";
        IO::Normalize(folder, s);
        if (IO::Directory::Exists(folder))
            return true;
    }
    else
    {
        s.clear() << RootFolder << "\\..\\Release\\..\\..\\..\\..\\resources\\examples";
        IO::Normalize(folder, s);
        if (IO::Directory::Exists(folder))
            return true;
    }
#endif

    // Nothing has been found
    folder.clear();
    return false;
}

void Initialize(int argc, char** argv, bool initializeSearchPath)
{
    if (argc < 1 or argv[0] == NULL)
    {
        logs.error() << "Impossible to find the root folder";
        return;
    }

    String p;
    IO::MakeAbsolute(p, argv[0]);
    String tt;
    IO::ExtractFilePath(tt, p);

    IO::Normalize(RootFolder, tt);

    if (initializeSearchPath)
    {
#ifdef YUNI_OS_WINDOWS
        p.clear() << RootFolder << "\\..\\resources\\";
        IO::Normalize(tt, p);
        SearchPaths.push_back(tt);
#else
        p.clear() << "/usr/share/antares/" << ANTARES_VERSION << "/resources/";
        SearchPaths.push_back(p);

        p.clear() << RootFolder << SEP << "../share/antares/" << ANTARES_VERSION << "/resources/";
        SearchPaths.push_back(p);
#endif

        p.clear() << RootFolder << SEP << "resources" << SEP;
        SearchPaths.push_back(p);

#ifdef YUNI_OS_MSVC
        if (Yuni::Logs::Verbosity::Debug::enabled)
        {
            p.clear() << RootFolder << "\\..\\Debug\\..\\resources\\";
            SearchPaths.push_back(p);
        }
        else
        {
            p.clear() << RootFolder << "\\..\\Release\\..\\resources\\";
            SearchPaths.push_back(p);
        }
#endif

        p.clear() << RootFolder << SEP;
        SearchPaths.push_back(p);
    }
}

bool FindFirstOf(String& out, const char* const* const list)
{
    String tmp;
    uint searchPathsCount = (uint)SearchPaths.size();
    for (uint j = 0; list[j]; ++j)
    {
        for (uint i = 0; i < searchPathsCount; ++i)
        {
            tmp.clear() << SearchPaths[i] << list[j];
            IO::Normalize(out, tmp);
            if (IO::File::Exists(out))
                return true;
        }
    }
    out.clear();
    return false;
}

} // namespace Resources
} // namespace Antares

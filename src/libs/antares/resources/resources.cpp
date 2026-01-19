// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/resources/resources.h"

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/logs/logs.h>
#include "antares/config/config.h"

using namespace Yuni;

#define SEP IO::Separator

namespace // anonymous
{
//! All search paths
static String::Vector SearchPaths;

//! The root folder
static String RootFolder;

} // anonymous namespace

namespace Antares::Resources
{
// TODO VP: delete with GUI
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
            {
                return true;
            }
        }
    }
    out.clear();
    return false;
}

// TODO VP: delete with GUI
void GetRootFolder(Yuni::String& out)
{
    out = RootFolder;
}

// TODO VP: delete with GUI
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
    {
        return true;
    }
#else
    // First guess - typical unix install
    s.clear() << RootFolder << SEP << ".." << SEP << "examples";
    s.clear() << "/usr/share/antares/" << ANTARES_VERSION << "/examples";
    IO::Normalize(folder, s);
    if (IO::Directory::Exists(folder))
    {
        return true;
    }
#endif

    // Second guess : Dev mode, from the source repository
    s.clear() << RootFolder << SEP << ".." << SEP << ".." << SEP << ".." << SEP << "resources"
              << SEP << "examples";
    IO::Normalize(folder, s);
    if (IO::Directory::Exists(folder))
    {
        return true;
    }

// Other guesses, Dev mode, special folders when ran from the SVN
// repository and from Visual Studio
#ifdef YUNI_OS_MSVC
    if (Yuni::Logs::Verbosity::Debug::enabled)
    {
        s.clear() << RootFolder << "\\..\\Debug\\..\\..\\..\\..\\resources\\examples";
        IO::Normalize(folder, s);
        if (IO::Directory::Exists(folder))
        {
            return true;
        }
    }
    else
    {
        s.clear() << RootFolder << "\\..\\Release\\..\\..\\..\\..\\resources\\examples";
        IO::Normalize(folder, s);
        if (IO::Directory::Exists(folder))
        {
            return true;
        }
    }
#endif

    // Nothing has been found
    folder.clear();
    return false;
}

void Initialize(int argc, const char** argv, bool initializeSearchPath)
{
    if (argc < 1 or !argv[0])
    {
        logs.error() << "Impossible to find the root folder";
        return;
    }

    String p;
    IO::MakeAbsolute(p, argv[0]);
    String tt;
    IO::parent_path(tt, p);

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

// TODO VP: delete with GUI
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
            {
                return true;
            }
        }
    }
    out.clear();
    return false;
}

} // namespace Antares::Resources

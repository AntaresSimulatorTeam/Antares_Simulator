// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/locator/locator.h"

#include <yuni/yuni.h>
#include "yuni/io/filename-manipulation.h"
#include <yuni/core/string.h>
#include <yuni/io/searchpath.h>

#include "antares/config/config.h"
#include "antares/resources/resources.h"

using namespace Yuni;

namespace Antares::Solver
{
bool FindLocation(String& location)
{
    // reset
    location.clear();

    // The root folder
    String root;
    Resources::GetRootFolder(root);
    String s;
    s.reserve(root.size() + 30 /*arbitrary*/);

    IO::SearchPath searchpaths;
    searchpaths.prefixes.push_back((s = "antares-"));

    if (System::windows)
    {
        searchpaths.extensions.push_back(".exe");

        searchpaths.directories.push_back((s = root) << "\\..\\bin");
        searchpaths.directories.push_back((s = root) << "\\Resources\\tools");
#ifdef NDEBUG
        searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\solver\\Release"); // msvc
#else
        searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\solver\\Debug"); // msvc
#endif

        searchpaths.directories.push_back((s = root) << "\\..\\..\\solver"); // mingw
        searchpaths.directories.push_back((s = root));
    }
    else
    {
        searchpaths.directories.push_back((s = root) << "/../../solver");
        searchpaths.directories.push_back(root); // TGZ package
        searchpaths.directories.push_back(s.clear() << "/usr/local/bin/");
        searchpaths.directories.push_back(s.clear() << "/usr/bin/");
    }

    s.clear();
    bool success = searchpaths.find(s, "solver");

    if (success)
    {
        IO::Normalize(location, s);
        return true;
    }
    return false;
}

bool FindConstraintsBuilderLocation(String& location)
{
    // reset
    location.clear();

    // The root folder
    String root;
    Resources::GetRootFolder(root);
    String s;
    s.reserve(root.size() + 30 /*arbitrary*/);

    IO::SearchPath searchpaths;
    searchpaths.prefixes.push_back((s = "antares-"));

    if (System::windows)
    {
        searchpaths.extensions.push_back(".exe");
        searchpaths.directories.push_back((s = root) << "\\..\\bin");
        searchpaths.directories.push_back((s = root) << "\\Resources\\tools");
#ifdef NDEBUG
        searchpaths.directories.push_back((s = root)
                                          << "\\..\\..\\..\\constraints-builder\\Release"); // msvc
#else
        searchpaths.directories.push_back((s = root)
                                          << "\\..\\..\\..\\constraints-builder\\Debug"); // msvc
#endif
        searchpaths.directories.push_back((s = root) << "\\..\\..\\constraints-builder"); // mingw
        searchpaths.directories.push_back((s = root));
    }
    else
    {
        searchpaths.directories.push_back((s = root) << "/../../constraints-builder");
        searchpaths.directories.push_back(s.clear() << "/usr/local/bin/");
        searchpaths.directories.push_back(s.clear() << "/usr/bin/");
    }

    return searchpaths.find(location, "constraints-builder");
}

bool FindYearByYearAggregator(Yuni::String& location)
{
    // reset
    location.clear();

    // The root folder
    String root;
    Resources::GetRootFolder(root);
    String s;
    s.reserve(root.size() + 30 /*arbitrary*/);

    IO::SearchPath searchpaths;
    searchpaths.prefixes.push_back((s = "antares-"));

    if (System::windows)
    {
        searchpaths.extensions.push_back(".exe");
        searchpaths.directories.push_back((s = root) << "\\..\\bin");
        searchpaths.directories.push_back((s = root) << "\\Resources\\tools");
#ifdef NDEBUG
        searchpaths.directories.push_back(
          (s = root) << "\\..\\..\\..\\tools\\yby-aggregator\\Release"); // msvc
#else
        searchpaths.directories.push_back((s = root)
                                          << "\\..\\..\\..\\tools\\yby-aggregator\\Debug"); // msvc
#endif
        searchpaths.directories.push_back((s = root) << "\\..\\..\\tools\\yby-aggregator"); // mingw
        searchpaths.directories.push_back((s = root));
    }
    else
    {
        searchpaths.directories.push_back((s = root) << "/../../tools/yby-aggregator");
        searchpaths.directories.push_back(s.clear() << "/usr/local/bin/");
        searchpaths.directories.push_back(s.clear() << "/usr/bin/");
    }

    return searchpaths.find(location, "ybyaggregator");
}

} // namespace Antares::Solver

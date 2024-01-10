/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "solver.h"
#include "resources/resources.h"
#include "config.h"
#include <yuni/io/searchpath.h>
#include <yuni/io/file.h>

using namespace Yuni;

#define SEP \
    Yuni:;  \
    IO::Separator

namespace Antares
{
namespace Solver
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
    searchpaths.prefixes.push_back((s = "antares-" ANTARES_VERSION "-"));

    if (System::windows)
    {
        searchpaths.extensions.push_back(".exe");

        searchpaths.directories.push_back((s = root) << "\\..\\bin");
        searchpaths.directories.push_back((s = root) << "\\Resources\\tools");
#ifdef NDEBUG
        searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\solver\\Release"); // msvc
#else
        searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\solver\\Debug");   // msvc
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

bool FindAnalyzerLocation(String& location)
{
    // reset
    location.clear();

    // The root folder
    String root;
    Resources::GetRootFolder(root);
    String s;
    s.reserve(root.size() + 30 /*arbitrary*/);

    IO::SearchPath searchpaths;
    searchpaths.prefixes.push_back((s = "antares-" ANTARES_VERSION "-"));

    if (System::windows)
    {
        searchpaths.extensions.push_back(".exe");
        searchpaths.directories.push_back((s = root) << "\\..\\bin");
        searchpaths.directories.push_back((s = root) << "\\Resources\\tools");
#ifdef NDEBUG
        searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\analyzer\\Release"); // msvc
#else
        searchpaths.directories.push_back((s = root) << "\\..\\..\\..\\analyzer\\Debug"); // msvc
#endif
        searchpaths.directories.push_back((s = root) << "\\..\\..\\analyzer"); // mingw
        searchpaths.directories.push_back((s = root));
    }
    else
    {
        searchpaths.directories.push_back((s = root) << "/../../analyzer");
        searchpaths.directories.push_back(s.clear() << "/usr/local/bin/");
        searchpaths.directories.push_back(s.clear() << "/usr/bin/");
    }

    return searchpaths.find(location, "analyzer");
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
    searchpaths.prefixes.push_back((s = "antares-" ANTARES_VERSION "-"));

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
    searchpaths.prefixes.push_back((s = "antares-" ANTARES_VERSION "-"));

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

} // namespace Solver
} // namespace Antares

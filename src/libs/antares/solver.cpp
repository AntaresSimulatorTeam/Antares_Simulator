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
bool FindLocation(String& location, Data::Version /*version*/, Solver::Feature features)
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
        searchpaths.directories.push_back(s.clear() << "/usr/local/bin/");
        searchpaths.directories.push_back(s.clear() << "/usr/bin/");
        searchpaths.directories.push_back((s = root) << "/../../solver");
        searchpaths.directories.push_back((s = root) << "."); // TGZ package
    }

    bool success = false;
    s.clear();
    switch (features)
    {
    case Solver::parallel:
    case Solver::standard:
        success = searchpaths.find(s, "solver");
        break;
    case Solver::withSwapFiles:
        success = searchpaths.find(s, "solver-swap");
        break;
    }

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

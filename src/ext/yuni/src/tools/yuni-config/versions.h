/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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
#ifndef YUNI_NO_THREAD_SAFE
#define YUNI_NO_THREAD_SAFE // disabling thread-safety
#endif
#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/version/version.h>
#include <map>

#ifdef YUNI_OS_WINDOWS
#if defined(YUNI_OS_MINGW)
#define YUNI_LIBCONFIG_DEFAULT_COMPILER "mingw"
#else
#define YUNI_LIBCONFIG_DEFAULT_COMPILER "msvc"
#endif
#else
#if defined(YUNI_OS_SUNOS) || defined(YUNI_OS_SOLARIS)
#define YUNI_LIBCONFIG_DEFAULT_COMPILER "suncc"
#else
#define YUNI_LIBCONFIG_DEFAULT_COMPILER "gcc"
#endif
#endif

namespace Yuni::LibConfig::VersionInfo
{
enum CompilerCompliant
{
    gcc,
    visualstudio,
};

enum Mapping
{
    mappingStandard,
    mappingSVNSources,
};

struct Settings
{
public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Settings();
    //@}

    bool configFile(String::List& options, bool displayError) const;

    bool parserModulesOptions(String::List& options, bool displayError);

public:
    String compiler;
    Mapping mapping;
    String path;
    String compilationMode;
    String::List modules;
    bool supportOpenGL;
    bool supportDirectX;

    String::List includePath;
    String::List libPath;

    /*!
    ** \brief Settings Per Module
    */
    struct SettingsPerModule
    {
    public:
        typedef std::map<String, bool> OptionMap;

    public:
        void merge(OptionMap& out, const OptionMap& with) const;

    public:
        //! Cxx flags for the module
        OptionMap cxxFlags;
        //! Includes to use for the module
        OptionMap includes;
        //! Frameworks needed by the module
        OptionMap frameworks;
        //! Libraries by the module
        OptionMap libs;
        //! Includes
        OptionMap libIncludes;
        //! Defines for the module
        OptionMap defines;
        //! Dependencies
        String::List dependencies;
    };

    std::map<String, SettingsPerModule> moduleSettings;
};

class List
{
public:
    typedef std::map<Version, VersionInfo::Settings> InternalList;
    typedef InternalList::reverse_iterator iterator;
    typedef InternalList::const_reverse_iterator const_iterator;

public:
    List()
    {
    }

    const String& compiler() const
    {
        return pCompiler;
    }

    void compiler(const String& c);

    void checkRootFolder(const String& root);

    void findFromPrefixes(const String::List& prefix);

    void print();

    void printWithInfos();

    VersionInfo::Settings* selected();

    void debug(bool mode)
    {
        pOptDebug = mode;
    }

private:
    void loadFromPath(const String& folder);

private:
    String pCompiler;
    InternalList pList;
    bool pOptDebug;
};

} // namespace Yuni::LibConfig::VersionInfo

#include "versions.hxx"

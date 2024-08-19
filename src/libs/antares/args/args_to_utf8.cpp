/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/args/args_to_utf8.h"

#include <yuni/yuni.h>

#ifdef YUNI_OS_WINDOWS
// Turning off format because order of windows and shellapi matters,
// apparently
// clang-format off

#include <string.h>
#include <cstdlib>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

// clang-format on
#endif // YUNI_OS_WINDOWS

IntoUTF8ArgsTranslator::IntoUTF8ArgsTranslator(int argc, const char** argv):
    argc_(argc),
    argv_(argv)
{
}

std::pair<int, const char**> IntoUTF8ArgsTranslator::convert()
{
#ifdef YUNI_OS_WINDOWS
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc_);
    argv_ = (char**)malloc(argc_ * sizeof(char*));
    for (int i = 0; i != argc_; ++i)
    {
        const uint len = (uint)wcslen(wargv[i]);
        const uint newLen = WideCharToMultiByte(CP_UTF8,
                                                0,
                                                wargv[i],
                                                len,
                                                nullptr,
                                                0,
                                                nullptr,
                                                nullptr);
        argv_[i] = (char*)malloc((newLen + 1) * sizeof(char));
        memset(argv_[i], 0, (newLen + 1) * sizeof(char));
        WideCharToMultiByte(CP_UTF8, 0, wargv[i], len, argv_[i], newLen, nullptr, nullptr);
        argv_[i][newLen] = '\0';
    }
#endif
    return {argc_, argv_};
}

IntoUTF8ArgsTranslator::~IntoUTF8ArgsTranslator()
{
#ifdef YUNI_OS_WINDOWS
    for (int i = 0; i != argc_; ++i)
    {
        free(argv_[i]);
    }
    free(argv_);
#endif
}

// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    auto& argv = const_cast<char**&>(argv_);
    argv = (char**)malloc(argc_ * sizeof(char*));
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
        argv[i] = (char*)malloc((newLen + 1) * sizeof(char));
        memset(argv[i], 0, (newLen + 1) * sizeof(char));
        WideCharToMultiByte(CP_UTF8, 0, wargv[i], len, argv[i], newLen, nullptr, nullptr);
        argv[i][newLen] = '\0';
    }
#endif
    return {argc_, argv_};
}

IntoUTF8ArgsTranslator::~IntoUTF8ArgsTranslator()
{
#ifdef YUNI_OS_WINDOWS
    auto& argv = const_cast<char**&>(argv_);
    for (int i = 0; i != argc_; ++i)
    {
        free(argv[i]);
    }
    free(argv);
#endif
}

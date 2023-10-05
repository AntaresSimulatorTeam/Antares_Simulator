
#include <yuni/yuni.h>
#include "antares/args/args_to_utf8.h"


#ifdef YUNI_OS_WINDOWS
#include <string.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include <stdio.h>
#endif

IntoUTF8ArgsTranslator::IntoUTF8ArgsTranslator(int argc, char**& argv)
    : argc_(argc), argv_(argv)
{
#ifdef YUNI_OS_WINDOWS
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    argv = (char**)malloc(argc * sizeof(char*));
    for (int i = 0; i != argc; ++i)
    {
        const uint len = (uint)wcslen(wargv[i]);
        const uint newLen = WideCharToMultiByte(CP_UTF8, 0, wargv[i], len, NULL, 0, NULL, NULL);
        argv[i] = (char*)malloc((newLen + 1) * sizeof(char));
        memset(argv[i], 0, (newLen + 1) * sizeof(char));
        WideCharToMultiByte(CP_UTF8, 0, wargv[i], len, argv[i], newLen, NULL, NULL);
        argv[i][newLen] = '\0';
    }
#endif
}

IntoUTF8ArgsTranslator::~IntoUTF8ArgsTranslator()
{
#ifdef YUNI_OS_WINDOWS
    for (int i = 0; i != argc_; ++i)
        free(argv_[i]);
    free(argv_);
#endif
}

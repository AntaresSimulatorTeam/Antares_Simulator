
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

#ifdef YUNI_OS_WINDOWS
char** AntaresGetUTF8Arguments(int argc, char**)
{
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    char** argvUTF8 = (char**)malloc(argc * sizeof(char*));
    for (int i = 0; i != argc; ++i)
    {
        const uint len = (uint)wcslen(wargv[i]);
        const uint newLen = WideCharToMultiByte(CP_UTF8, 0, wargv[i], len, NULL, 0, NULL, NULL);
        argvUTF8[i] = (char*)malloc((newLen + 1) * sizeof(char));
        memset(argvUTF8[i], 0, (newLen + 1) * sizeof(char));
        WideCharToMultiByte(CP_UTF8, 0, wargv[i], len, argvUTF8[i], newLen, NULL, NULL);
        argvUTF8[i][newLen] = '\0';
    }
    return argvUTF8;
}

void FreeUTF8Arguments(int argc, char** argv)
{
    for (int i = 0; i != argc; ++i)
        free(argv[i]);
    free(argv);
}

#else

char** AntaresGetUTF8Arguments(int argc, char** argv)
{
    return argv;
}

void FreeUTF8Arguments(int argc, char** argv)
{}

#endif
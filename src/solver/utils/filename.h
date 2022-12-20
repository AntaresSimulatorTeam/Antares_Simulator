#pragma once

#include <string>
#include <yuni/core/string.h>

std::string getFilenameWithExtension(const YString& prefix,
                                     const YString& extension,
                                     int year,
                                     int week,
                                     int optNumber = 0);

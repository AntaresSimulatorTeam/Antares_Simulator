#pragma once

#include <string>
#include <yuni/core/string.h>

std::string getFilenameWithExtension(const YString& prefix,
                                     const YString& extension,
                                     uint numSpace,
                                     int optNumber = 0);

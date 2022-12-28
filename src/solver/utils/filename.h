#pragma once

#include <string>

std::string getFilenameWithExtension(const std::string& prefix,
                                     const std::string& extension,
                                     const unsigned int year,
                                     const unsigned int week,
                                     const unsigned int optNumber);

// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_ANTARES_IO_FILE_H__
#define __LIBS_ANTARES_IO_FILE_H__

#include <filesystem>
#include <string>

namespace Antares::IO
{
/*!
** \brief Reset the content of a file
**
** This routine will wait if there is not enough disk space
*/
bool fileSetContent(const std::string& filename, const std::string& content);

std::string readFile(const std::filesystem::path&);

} // namespace Antares::IO

#endif // __LIBS_ANTARES_IO_FILE_H__

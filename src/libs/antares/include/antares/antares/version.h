// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_VERSION_H__
#define __ANTARES_LIBS_VERSION_H__

namespace Antares
{
/*!
** \brief Get the version of Antares in a C-string format
*/
const char* VersionToCString();

/*!
** \brief Print the version of Antares on the standard output
*/
void PrintVersionToStdCout();

} // namespace Antares

#endif // __ANTARES_LIBS_VERSION_H__

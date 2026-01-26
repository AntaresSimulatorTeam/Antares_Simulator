// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_RESOURCE_H__
#define __ANTARES_LIBS_RESOURCE_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace Antares::Resources
{
/*!
** \brief Get the full path of a resource file
**
** \param relFileName The filename of the resource, relative to the resource folder
** \return The full path of the resource file if found, an empty string otherwise
*/
bool FindFile(YString& out, const AnyString& filename);

/*!
** \brief Get the full path of a resource file
**
** \param relFileName The filename of the resource, relative to the resource folder
** \return The full path of the resource file if found, an empty string otherwise
*/
bool FindFirstOf(YString& out, const char* const* const list);

/*!
** \brief Try to find the examples folder
*/
bool FindExampleFolder(YString& folder);

/*!
** \brief Initialize variables about resource handling
*/
void Initialize(int argc, const char* argv[], bool initializeSearchPath = false);

/*!
** \brief Copy the root folder
*/
void GetRootFolder(YString& out);

} // namespace Antares::Resources

#endif // __ANTARES_LIBS_RESOURCE_H__

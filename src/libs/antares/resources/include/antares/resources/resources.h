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
#ifndef __ANTARES_LIBS_RESOURCE_H__
#define __ANTARES_LIBS_RESOURCE_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace Antares
{
namespace Resources
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

} // namespace Resources
} // namespace Antares

#endif // __ANTARES_LIBS_RESOURCE_H__

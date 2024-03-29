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
#ifndef __ANTARES_APPLICATION_RECENT_FILES_H__
#define __ANTARES_APPLICATION_RECENT_FILES_H__

#include <yuni/yuni.h>
#include "../toolbox/wx-wrapper.h"
#include <list>

namespace Antares
{
namespace RecentFiles
{
//! The maximum number of recent files
enum
{
    Max = 10,
};

//! List of recent files
using List = std::list<std::pair<wxString, wxString>>;
//! Ptr
using ListPtr = Yuni::SmartPtr<List>;

/*!
** \brief Get the whole list of recent files
*/
ListPtr Get();

/*!
** \brief Insert a recent file
*/
void Add(const wxString& path, const wxString& title = wxString());

/*!
** \brief
*/
ListPtr AddAndGet(wxString path, const wxString& title = wxString());

/*!
** \brief Write the settings about the list of recent files
*/
void Write(const ListPtr lst);

/*!
** \brief Set if the path of the studies must be displayed in the menu
*/
void ShowPathInMenu(bool value);

/*!
** \brief Get if the path of the studies should be displayed in the menu
*/
bool ShowPathInMenu();

} // namespace RecentFiles
} // namespace Antares

#endif // __ANTARES_APPLICATION_RECENT_FILES_H__

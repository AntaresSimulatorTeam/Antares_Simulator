// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_RECENT_FILES_H__
#define __ANTARES_APPLICATION_RECENT_FILES_H__

#include <yuni/yuni.h>
#include "../toolbox/wx-wrapper.h"
#include <list>

namespace Antares::RecentFiles
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

} // namespace Antares::RecentFiles

#endif // __ANTARES_APPLICATION_RECENT_FILES_H__

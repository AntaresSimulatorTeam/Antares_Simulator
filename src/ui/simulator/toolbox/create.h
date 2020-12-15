/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_TOOLBOX_CREATE_H__
#define __ANTARES_TOOLBOX_CREATE_H__

#include <antares/wx-wrapper.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>

namespace Antares
{
namespace Component
{
/*!
** \brief Create a standard button
**
** \param parent  The parent control
** \param caption The caption for the new button
** \param object  A popinter to the object which will receive the onClick event
** \param method  The method to bind for the onClick event
*/
template<class T, class StringT>
wxButton* CreateButton(wxWindow* parent,
                       const StringT& caption,
                       T* object = NULL,
                       void (T::*method)(void*) = NULL,
                       void* userdata = NULL);

/*!
** \brief Create a standard label
**
** \param parent  The parent control
** \param caption The caption for the new button
*/
wxStaticText* CreateLabel(wxWindow* parent, const wxString& caption);

/*!
** \brief Create a standard label
**
** \param parent  The parent control
** \param caption The caption for the new button
** \param bold    True to use a bold font
** \param grey    True to lighten the label
*/
wxStaticText* CreateLabel(wxWindow* parent, const wxString& caption, bool bold, bool grey = false);

/*!
** \brief Create a standard label
**
** \param parent  The parent control
** \param caption The caption for the new button
** \param bold    True to use a bold font
** \param grey    True to lighten the label
** \param size    Relative size (...-1, 0, +1...)
*/
wxStaticText* CreateLabel(wxWindow* parent,
                          const wxString& caption,
                          bool bold,
                          bool grey,
                          int size);

/*!
** \brief Add a new vertical separator
*/
wxWindow* AddVerticalSeparator(wxWindow* parent, wxSizer* sizer, uint size = 1, uint border = 4);

/*!
** \brief Add a new vertical separator
*/
wxWindow* AddHorizontalSeparator(wxWindow* parent, wxSizer* sizer, bool spacing = true);

} // namespace Component
} // namespace Antares

#include "create.hxx"

#endif // __ANTARES_TOOLBOX_CREATE_H__

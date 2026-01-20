// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_CREATE_H__
#define __ANTARES_TOOLBOX_CREATE_H__

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <yuni/yuni.h>

namespace Antares::Component
{
/*!
** \brief Create a standard button
**
** \param parent  The parent control
** \param caption The caption for the new button
** \param object  A popinter to the object which will receive the onClick event
** \param method  The method to bind for the onClick event
*/
template<class T, class StringT, class UserDataT>
wxButton* CreateButton(wxWindow* parent,
                       const StringT& caption,
                       T* object = NULL,
                       void (T::*method)(UserDataT) = NULL,
                       UserDataT userdata = NULL);

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

} // namespace Antares::Component

#include "create.hxx"

#endif // __ANTARES_TOOLBOX_CREATE_H__

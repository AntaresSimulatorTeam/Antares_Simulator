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
#ifndef __ANTARES_WINDOWS_INSPECTOR_EDITOR_STUDY_CALENDAR_H__
#define __ANTARES_WINDOWS_INSPECTOR_EDITOR_STUDY_CALENDAR_H__

#include <wx/menu.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/editors.h>

namespace Antares
{
namespace Window
{
namespace Inspector
{
class StudyCalendarBtnEditor : public wxPGTextCtrlEditor
{
    DECLARE_DYNAMIC_CLASS(StudyCalendarBtnEditor)
public:
    StudyCalendarBtnEditor();
    virtual ~StudyCalendarBtnEditor();

    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const;

    virtual wxString GetName() const;

    virtual bool OnEvent(wxPropertyGrid* grid,
                         wxPGProperty* property,
                         wxWindow* ctrl,
                         wxEvent& event) const;

}; // class StudyCalendarBtnEditor

WX_PG_DECLARE_EDITOR(StudyCalendarBtnEditor)

} // namespace Inspector
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_INSPECTOR_EDITOR_STUDY_CALENDAR_H__

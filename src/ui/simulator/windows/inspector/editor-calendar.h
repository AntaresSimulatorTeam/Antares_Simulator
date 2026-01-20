// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_INSPECTOR_EDITOR_STUDY_CALENDAR_H__
#define __ANTARES_WINDOWS_INSPECTOR_EDITOR_STUDY_CALENDAR_H__

#include <wx/menu.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/editors.h>

namespace Antares::Window::Inspector
{
class StudyCalendarBtnEditor: public wxPGTextCtrlEditor
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

} // namespace Antares::Window::Inspector

#endif // __ANTARES_WINDOWS_INSPECTOR_EDITOR_STUDY_CALENDAR_H__

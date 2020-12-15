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

#include "editor-calendar.h"
#include "frame.h"
#include <yuni/core/math.h>
#include "../../application/menus.h"
#include "../calendar/calendar.h"
#include "../../application/main.h"
#include "../../application/study.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace Inspector
{
StudyCalendarBtnEditor::StudyCalendarBtnEditor()
{
}

StudyCalendarBtnEditor::~StudyCalendarBtnEditor()
{
}

wxPGWindowList StudyCalendarBtnEditor::CreateControls(wxPropertyGrid* grid,
                                                      wxPGProperty* property,
                                                      const wxPoint& pos,
                                                      const wxSize& sz) const
{
    // Create and populate buttons-subwindow
    wxPGMultiButton* buttons = new wxPGMultiButton(grid, sz);
    // Add two regular buttons
    buttons->Add(wxT(".."));
    // Create the 'primary' editor control (textctrl in this case)
    wxPGWindowList wndList
      = wxPGTextCtrlEditor::CreateControls(grid, property, pos, buttons->GetPrimarySize());
    buttons->Finalize(grid, pos);
    wndList.SetSecondary(buttons);
    return wndList;
}

bool StudyCalendarBtnEditor::OnEvent(wxPropertyGrid* grid,
                                     wxPGProperty* property,
                                     wxWindow* ctrl,
                                     wxEvent& event) const
{
    if (event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED)
    {
        auto study = Data::Study::Current::Get();
        if (!study)
            return false;

        auto* buttons = (wxPGMultiButton*)grid->GetEditorControlSecondary();

        if (event.GetId() == buttons->GetButtonId(0))
        {
            // looking for the simulation range
            Date::DayInterval range;
            range.first = 0;
            range.end = 366; // arbitrary

            range.first = Math::Max(range.first, study->parameters.simulationDays.first);
            range.end = Math::Min(range.end, study->parameters.simulationDays.end);

            auto* mainFrm = Antares::Forms::ApplWnd::Instance();

            Antares::Window::CalendarSelect form(mainFrm);
            form.selectionDayRange[0] = range.first;
            form.selectionDayRange[1] = range.end;
            form.allowRangeSelection = true;
            form.requireWholeWeekSelection = true;
            form.ShowModal();

            if (form.modified())
            {
                if (form.selectionDayRange[0] != (uint)-1)
                {
                    assert(form.selectionDayRange[1] > form.selectionDayRange[0]);
                    study->parameters.simulationDays.first = form.selectionDayRange[0];
                    study->parameters.simulationDays.end = form.selectionDayRange[1];
                    MarkTheStudyAsModified(study);
                    OnStudySimulationSettingsChanged();
                }
            }

            // Do something when first button is pressed,
            // Return true if value in editor changed by this
            // action.
            return true;
        }
    }
    return wxPGTextCtrlEditor::OnEvent(grid, property, ctrl, event);
}

} // namespace Inspector
} // namespace Window
} // namespace Antares

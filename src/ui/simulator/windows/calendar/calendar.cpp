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

#include <yuni/yuni.h>
#include <yuni/core/system/memory.h>
#include "calendar.h"
#include "../../application/study.h"
#include <antares/study/version.h>
#include "../../toolbox/resources.h"
#include "../../toolbox/create.h"
#include "../../../config.h"
#include "../../application/menus.h"
#include <ui/common/component/panel.h>

#include <wx/sizer.h>
#include <wx/statline.h>

using namespace Yuni;
#include "view-standard.hxx"

namespace Antares
{
namespace Window
{
BEGIN_EVENT_TABLE(CalendarSelect, wxDialog)
EVT_SHOW(CalendarSelect::evtOnShow)
END_EVENT_TABLE()

CalendarSelect::CalendarSelect(wxWindow* parent) :
 wxDialog(parent, wxID_ANY, wxT("Calendar"), wxDefaultPosition, wxDefaultSize),
 pButtonSelect(nullptr),
 pApplyButton(nullptr),
 pCalendarView(nullptr)
{
    selectionDayRange[0] = (uint)-1;
    selectionDayRange[1] = (uint)-1;
    allowRangeSelection = true;
    allowQuickSelect = false;
    requireWholeWeekSelection = true;
    pComponentCreated = false;
    pHasBeenModified = false;
    pCanceled = true;
}

CalendarSelect::~CalendarSelect()
{
}

void CalendarSelect::evtOnShow(wxShowEvent& evt)
{
    if (evt.IsShown()) // show can hide !!!
    {
        pHasBeenModified = false;
        pCanceled = true;

        if (not pComponentCreated)
        {
            // Informations about the study
            wxColour defaultBgColor = GetBackgroundColour();
            SetBackgroundColour(wxColour(255, 255, 255));

            auto* sizer = new wxBoxSizer(wxVERTICAL);

            auto* standardview = new Component::Panel(this);
            standardview->SetBackgroundColour(wxColour(255, 255, 255));
            auto* svsizer = new wxBoxSizer(wxVERTICAL);
            standardview->SetSizer(svsizer);

            if (not allowQuickSelect)
            {
                auto* btn = new Component::Button(
                  standardview, wxT("(invalid)"), "images/16x16/calendar_month.png");
                btn->menu(true);
                btn->onPopupMenu(this, &CalendarSelect::onPopupMenu);
                btn->caption();

                auto* s = new wxBoxSizer(wxHORIZONTAL);
                s->Add(40, 5);
                s->Add(btn, 0, wxALL | wxEXPAND);
                s->AddStretchSpacer();
                svsizer->Add(7, 7);
                svsizer->Add(s, 0, wxALL | wxEXPAND);
                svsizer->Add(4, 4);

                pButtonSelect = btn;
            }

            auto* calendarview = new CalendarViewStandard(standardview, *this);
            pCalendarView = calendarview;
            svsizer->Add(calendarview, 1, wxALL | wxEXPAND);
            calendarview->onUpdateSelectionText.bind(this, &CalendarSelect::updateSelectionText);
            sizer->Add(standardview, 1, wxALL | wxEXPAND);

            // Panel with buttons
            {
                // sizer->AddSpacer(10);

                sizer->Add(new wxStaticLine(this), 0, wxALL | wxEXPAND);
                auto* panel = new Antares::Component::Panel(this);
                auto* vsizer = new wxBoxSizer(wxVERTICAL);
                auto* sizerBar = new wxBoxSizer(wxHORIZONTAL);
                vsizer->Add(sizerBar, 1, wxALL | wxEXPAND, 7);
                sizerBar->AddStretchSpacer();
                panel->SetSizer(vsizer);
                panel->SetBackgroundColour(defaultBgColor);

                // Close button
                {
                    auto* btn = Antares::Component::CreateButton(
                      panel, wxT("Cancel"), this, &CalendarSelect::onClose);
                    sizerBar->Add(btn, 0, wxFIXED_MINSIZE | wxALIGN_CENTRE_VERTICAL | wxALL);
                    sizerBar->Add(5, 2);

                    if (not allowQuickSelect)
                    {
                        btn = Antares::Component::CreateButton(
                          panel, wxT("  Apply the new date  "), this, &CalendarSelect::onProceed);
                        pApplyButton = btn;
                        sizerBar->Add(btn, 0, wxFIXED_MINSIZE | wxALIGN_CENTRE_VERTICAL | wxALL);
                    }

                    sizerBar->Add(15, 2);

                    // Focus for the last created button, whatever it is
                    btn->SetDefault();
                    btn->SetFocus();
                }

                sizer->Add(panel, 0, wxALL | wxEXPAND);
            }

            SetSizer(sizer);
            // sizer->Fit(this);

            wxSize p = GetSize();
            p.SetWidth(CalendarViewStandard::recommendedWindowWidth);
            p.SetHeight(CalendarViewStandard::recommendedWindowHeight + 22 + 7
                        + 7 /*panel ok/cancel*/ + 12 /*selection button*/);
            SetSize(p);

            CenterOnParent();
            pComponentCreated = true;
        }
    }
}

void CalendarSelect::onClose(void*)
{
    Dispatcher::GUI::Close(this);
}

void CalendarSelect::onProceed(void*)
{
    if (selectionDayRange[0] != (uint)-1)
    {
        pCanceled = false;
        Dispatcher::GUI::Close(this);
    }
}

void CalendarSelect::updateSelectionText(const YString& text)
{
    if (pButtonSelect)
    {
        pButtonSelect->caption(wxStringFromUTF8(text));
        pButtonSelect->Refresh();
    }
    if (pApplyButton)
        pApplyButton->Enable((selectionDayRange[0] != (uint)-1));
}

void CalendarSelect::onPopupMenu(Component::Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Select the whole year"));
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(CalendarSelect::onSelectWholeYear),
                 nullptr,
                 this);

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Clear the selection"));
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(CalendarSelect::onSelectNone),
                 nullptr,
                 this);
}

void CalendarSelect::onSelectWholeYear(wxCommandEvent&)
{
    auto* calendarView = dynamic_cast<CalendarViewStandard*>(pCalendarView);
    if (calendarView)
        calendarView->selectWholeYear();
}

void CalendarSelect::onSelectNone(wxCommandEvent&)
{
    auto* calendarView = dynamic_cast<CalendarViewStandard*>(pCalendarView);
    if (calendarView)
        calendarView->selectNone();
}

bool CalendarSelect::modified() const
{
    return pHasBeenModified && (not pCanceled);
}

} // namespace Window
} // namespace Antares

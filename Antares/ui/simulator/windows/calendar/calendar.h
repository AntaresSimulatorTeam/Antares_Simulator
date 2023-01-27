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
#ifndef __ANTARES_APPLICATION_WINDOWS_CALENDAR_CALENDAR_H__
#define __ANTARES_APPLICATION_WINDOWS_CALENDAR_CALENDAR_H__

#include <antares/wx-wrapper.h>
#include <wx/dialog.h>
#include "../../toolbox/components/button.h"

namespace Antares
{
namespace Window
{
class CalendarViewStandard;

/*!
** \brief Dialog Window for selecting a date range
*/
class CalendarSelect final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    CalendarSelect(wxWindow* parent);
    //! Destructor
    virtual ~CalendarSelect();
    //@}

    /*!
    ** \brief Get if the date has been modified
    */
    bool modified() const;

public:
    //! Final Day range selection
    uint selectionDayRange[2];
    //! option: True to allow range selection
    bool allowRangeSelection;
    //! option: True to allow a quick selection (without clicking to a button)
    bool allowQuickSelect;
    //!
    bool requireWholeWeekSelection;

private:
    //! Event: the user requested to close the window
    void onClose(void*);
    void onProceed(void*);
    void evtOnShow(wxShowEvent&);
    void updateSelectionText(const YString& text);
    void onPopupMenu(Component::Button&, wxMenu& menu, void*);

    void onSelectWholeYear(wxCommandEvent&);
    void onSelectNone(wxCommandEvent&);

private:
    //! Flag to know if the components are already created
    bool pComponentCreated;
    //! Modified
    bool pHasBeenModified;
    //! Flag to know if the process has been canceled
    bool pCanceled;
    //!
    Component::Button* pButtonSelect;
    wxWindow* pApplyButton;
    wxWindow* pCalendarView;
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

    friend class CalendarViewStandard;

}; // class CalendarSelect

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_CALENDAR_CALENDAR_H__

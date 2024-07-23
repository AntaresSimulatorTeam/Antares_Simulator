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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_APPLICATION_WINDOWS_CALENDAR_CALENDAR_H__
#define __ANTARES_APPLICATION_WINDOWS_CALENDAR_CALENDAR_H__

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

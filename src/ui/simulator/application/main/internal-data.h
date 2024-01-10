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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_APPLICATION_MAIN_INTERNAL_DATA_H__
#define __ANTARES_APPLICATION_MAIN_INTERNAL_DATA_H__

#include <yuni/yuni.h>
#include "main.h"
#include <wx/menuitem.h>
#include <cassert>
#include "../../toolbox/components/button.h"
#include <antares/study/study.h>

namespace Antares
{
namespace Forms
{
enum
{
    //! The minimal width for the main frame
    minimalWidth = 900,
    //! The minimal height for the main frame
    minimalHeight = 600,
};

class MainFormData final
{
public:
    //! \name Constructor & Destructor
    //@{
    //! Default constructor
    MainFormData(ApplWnd& form);
    //@}

    void editCurrentLocation(const wxString& string);

    //! \name Events related to the main toolbar
    //@{
    //! Show the Wizard
    void onToolbarWizard(void*);
    //! New study
    void onToolbarNewStudy(void*);
    //! Open a local study
    void onToolbarOpenLocalStudy(void*);
    void onToolbarOpenRecentMenu(Component::Button&, wxMenu& menu, void*);
    //! Save
    void onToolbarSave(void*);
    //! Run a simulation
    void onToolbarRunSimulation(void*);
    //! Logs
    void onToolbarLogs(void*);
    //! Fullscreen
    void onToolbarFullscreen(void*);
    //! Inspector
    void onToolbarInspector(void*);
    //! Optimization preferences
    void onToolbarOptimizationPreferences(void*);
    //@}

public:
    //! \name WIP
    //@{
    //! True if the WIP status is enabled
    bool wipEnabled;
    //!
    wxWindow* wipPanel;
    //@}

private:
    //! Reference to the main form
    ApplWnd& pMainForm;
    //! Menu item: current location
    wxMenuItem* pEditCurrentLocation;
    // friend
    friend class ApplWnd;

}; // class MainFormData

} // namespace Forms
} // namespace Antares

#endif // __ANTARES_APPLICATION_MAIN_INTERNAL_DATA_H__

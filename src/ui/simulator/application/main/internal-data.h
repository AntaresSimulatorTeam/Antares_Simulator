// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_MAIN_INTERNAL_DATA_H__
#define __ANTARES_APPLICATION_MAIN_INTERNAL_DATA_H__

#include <yuni/yuni.h>
#include "main.h"
#include <wx/menuitem.h>
#include <cassert>
#include "../../toolbox/components/button.h"
#include <antares/study/study.h>

namespace Antares::Forms
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

} // namespace Antares::Forms

#endif // __ANTARES_APPLICATION_MAIN_INTERNAL_DATA_H__

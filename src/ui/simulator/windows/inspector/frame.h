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
#ifndef __ANTARES_WINDOWS_INSPECTOR_FRAME_H__
#define __ANTARES_WINDOWS_INSPECTOR_FRAME_H__

#include <wx/propgrid/propgrid.h>

#include <antares/wx-wrapper.h>
#include <antares/study.h>
#include "../notes.h"
#include "data.h"
#include <ui/common/component/panel.h>

namespace Antares
{
namespace Window
{
namespace Inspector
{
// Forward declaration
class InspectorGrid;

class Frame final : public Antares::Component::Panel, public Yuni::IEventObserver<Frame>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    ** \param allowAnyObject True to allow the edition of any object, not just the given ones
    */
    explicit Frame(wxWindow* parent, bool allowAnyObject = false);
    //! Destructor
    virtual ~Frame();
    //@}

    //! \name Apply a configuration
    //@{
    /*!
    ** \brief Prepare the inspector with the given selection of objects
    **
    ** \param data A selection of objects (can be NULL)
    */
    void apply(const InspectorData::Ptr& data);

    /*!
    ** \brief Apply the global selection (delayed)
    */
    void delayApplyGlobalSelection();

    /*!
    ** \brief Apply the current selection (delayed)
    */
    void delayApply();

    //@}

    /*!
    ** \brief Attach this inspector to the main form
    **
    ** A new pane will be created and the inspector will be attached to it.
    */
    void attachToTheMainForm();

    /*!
    ** \brief Detach this inspector from the main form
    */
    void detachFromTheMainForm();

public:
    // Events
    void onSelectAllLinks(wxCommandEvent& evt);
    void onSelectLink(wxCommandEvent& evt);
    void onSelectAllLinksFromArea(wxCommandEvent& evt);

    void onSelectAllPlants(wxCommandEvent& evt);
    void onSelectPlant(wxCommandEvent& evt);
    void onSelectAllPlantsFromArea(wxCommandEvent& evt);

    void clearAssociatinsBetweenIDAndPtr()
    {
        mapIDPointer.clear();
    }
    void assign(int id, const void* p)
    {
        mapIDPointer[id] = const_cast<void*>(p);
    }

protected:
    void onDelayApplyGlobalSelection();
    void onDelayApply();
    void onStudyClosed();
    void onSelectProperties(void*);
    void onSelectNotes(void*);
    void onPropertyChanging(wxPropertyGridEvent& event);
    void onLoadUserNotes();
    void onInternalRefresh(const void* sender);

private:
    //! The property grid
    wxPropertyGrid* pPropertyGrid;
    //! The main panel
    wxPanel* pMainPanel;
    //! User's notes
    Window::Notes* pNotes;
    //! Flag to know if the inspector can deal with any object, or just
    // with the givel selection
    bool pAllowAnyObject;
    //! The current selection
    InspectorData::Ptr pCurrentSelection;
    //! Callback to inform the property grid about any selection change
    Yuni::Bind<void(const InspectorData::Ptr&)> pApplyPropertyGrid;

    // Some property pointers are kept to speed-up the update
    // About a Study
    wxPGProperty* pPGCommonStudyName;
    wxPGProperty* pPGCommonStudyAuthor;

    // About studies
    wxPGProperty* pPGStudySpace1;
    wxPGProperty* pPGStudySpace2;
    wxPGProperty* pPGStudyTitle;
    wxPGProperty* pPGStudyGrpSimulation;
    wxPGProperty* pPGStudyGrpCalendar;
    wxPGProperty* pPGStudyMCScenarios;
    wxPGProperty* pPGStudyOutputProfile;
    wxPGProperty* pPGStudyMode;
    wxPGProperty* pPGStudyYears;
    wxPGProperty* pPGStudyCalendarBegin;
    wxPGProperty* pPGStudyCalendarEnd;
    wxPGProperty* pPGStudy1rstJanuary;
    wxPGProperty* pPGStudyHorizon;
    wxPGProperty* pPGStudyCalendarMonth;
    wxPGProperty* pPGStudyLeapYear;
    wxPGProperty* pPGStudyCalendarWeek;
    wxPGProperty* pPGStudyBuildMode;
    wxPGProperty* pPGStudyPlaylist;
    wxPGProperty* pPGStudySimulationSynthesis;
    wxPGProperty* pPGStudyYearByYear;
    wxPGProperty* pPGGeographicTrimming;
    wxPGProperty* pPGThematicTrimming;
    wxPGProperty* pPGStudyUseMCScenarios;

    // About Areas
    wxPGProperty* pPGAreaSeparator;
    wxPGProperty* pPGAreaTitle;
    wxPGProperty* pPGAreaGeneral;
    wxPGProperty* pPGAreaOptimization;
    wxPGProperty* pPGAreaLocalization;
    wxPGProperty* pPGAreaFilteringStatus;
    wxPGProperty* pPGAreaFilteringSynthesis[5];
    wxPGProperty* pPGAreaFilteringYbY[5];
    wxPGProperty* pPGAreaDeps;
    wxPGProperty* pPGAreaResort;
    wxPGProperty* pPGAreaResortNon;
    wxPGProperty* pPGAreaResortHydroPower;
    wxPGProperty* pPGAreaResortOther;
    wxPGProperty* pPGUnsupplied;
    wxPGProperty* pPGSpilled;
    wxPGProperty* pPGAreaName;
    wxPGProperty* pPGAreaColor;
    wxPGProperty* pPGAreaLinks;
    wxPGProperty* pPGAreaPlants;

    // About links
    wxPGProperty* pPGLinkSeparator;
    wxPGProperty* pPGLinkFilteringStatus;
    wxPGProperty* pPGLinkFilteringSynthesis[5];
    wxPGProperty* pPGLinkFilteringYbY[5];
    wxPGProperty* pPGLinkColor;
    wxPGProperty* pPGLinkStyle;
    wxPGProperty* pPGLinkWidth;

    // About Thermal clusters
    wxPGProperty* pPGClusterSeparator;
    wxPGProperty* pPGClusterGeneral;
    wxPGProperty* pPGClusterParams;
    wxPGProperty* pPGClusterReliabilityModel;
    wxPGProperty* pPGClusterCosts;
    wxPGProperty* pPGClusterName;
    wxPGProperty* pPGClusterNominalCapacity;
    wxPGProperty* pPGClusterEnabled;
    wxPGProperty* pPGClusterUnitCount;
    wxPGProperty* pPGClusterInstalled;
    wxPGProperty* pPGClusterMustRun;
    wxPGProperty* pPGClusterGroup;
    wxPGProperty* pPGClusterArea;
    wxPGProperty* pPGClusterCO2;
    wxPGProperty* pPGClusterVolatilityForced;
    wxPGProperty* pPGClusterVolatilityPlanned;
    wxPGProperty* pPGClusterLawForced;
    wxPGProperty* pPGClusterLawPlanned;
    wxPGProperty* pPGClusterSpinning;

    wxPGProperty* pPGClusterMarginalCost;
    wxPGProperty* pPGClusterFixedCost;
    wxPGProperty* pPGClusterStartupCost;
    wxPGProperty* pPGClusterOperatingCost;
    wxPGProperty* pPGClusterRandomSpread;

    wxPGProperty* pPGClusterMinStablePower;
    wxPGProperty* pPGClusterMinUpTime;
    wxPGProperty* pPGClusterMinDownTime;

    // About constraints
    wxPGProperty* pPGConstraintSeparator;
    wxPGProperty* pPGConstraintTitle;
    wxPGProperty* pPGConstraintName;
    wxPGProperty* pPGConstraintGeneral;
    wxPGProperty* pPGConstraintComments;
    wxPGProperty* pPGConstraintEnabled;
    wxPGProperty* pPGConstraintType;

    // Association between id and a pointer
    // This ugly map is required because wxMenuItem/Connect() does not support
    // user-data........
    std::map<int, void*> mapIDPointer;

    Component::Button* pBtnInspector;
    bool pAlreadyConnectedToSimulationChangesEvent;

    // Friend !
    friend class InspectorGrid;

}; // class InspectorFramce

// Singleton
extern Frame* gInspector;

} // namespace Inspector
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_INSPECTOR_FRAME_H__

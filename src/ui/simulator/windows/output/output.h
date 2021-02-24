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
#ifndef __ANTARES_WINDOWS_OUTPUT_OUTPUT_H__
#define __ANTARES_WINDOWS_OUTPUT_OUTPUT_H__

#include <antares/wx-wrapper.h>
#include <ui/common/component/panel.h>
#include "../../application/study.h"
#include "../../toolbox/components/button.h"
#include <wx/treectrl.h>
#include <wx/splitter.h>
#include <wx/sizer.h>
#include "layer.h"
#include "fwd.h"
#include <antares/study/fwd.h>
#include <wx/statbmp.h>
#include <wx/scrolwin.h>
#include "panel.h"
#include <ui/common/component/spotlight.h>

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
/*!
** \brief Component: Viewer for simulation results
*/
class Component : public Antares::Component::Panel, public Yuni::IEventObserver<Component>
{
public:
    //! Content
    typedef Antares::Private::OutputViewerData::Content Content;

public:
    /*!
    ** \brief Convert a name into an ID
    */
    static void ConvertVarNameToID(Yuni::String& id, const Yuni::String& name);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Component(wxWindow* parent, bool parentIsStandaloneWindow = false);
    //! Destructor
    virtual ~Component();
    //@}

    /*!
    ** \brief Clear all output
    */
    void clear();

    /*!
    ** \brief Update the list of all outputs
    */
    void update(const Data::Output::List& list, const Data::Output::Ptr& selection);

    void updateLayerList();

    /*!
    ** \brief Copy data from another output viewer
    */
    void copyFrom(const Component& source);

    /*!
    ** \brief Ask to all panels to scroll as the given component
    */
    void scrollAllPanels(wxScrolledWindow* component);

    void selectCellAllPanels(uint x, uint y);

    //! Refresh all tabs (but not their content)
    void refreshAllTabs();
    //! Refresh all panels (only their content)
    void refreshAllPanels();
    //! Refresh all virtual layers
    void refreshAllPanelsWithVirtualLayer();

    void refreshPanel(uint index);

private:
    class Tab
    {
    public:
        //! SmartPtr
        typedef Yuni::SmartPtr<Tab> Ptr;
        //! Vector
        typedef std::vector<Ptr> Vector;

    public:
        //! \name Constructor & Destructor
        //@{
        //! Default constructor
        explicit Tab(Component& comparent);
        //! Destructor
        ~Tab();
        //@}

        void updateAreaOrLinkName(SelectionType selection, const Yuni::String& text);

    public:
        //! The parent component
        Component& component;
        //! Panel support
        Antares::Component::Panel* support;
        //! Button, for displaying all available simulations
        Antares::Component::Button* btnOutput;
        //! Button, for displaying custom selection
        Antares::Component::Button* btnCustomSelection;
        //! Button, for displaying all available areas/links
        Antares::Component::Button* btnItem;
        //! Sizer for buttons
        wxSizer* btnSizer;
    };

private:
    //! Clear all output selections
    void outputSelectionsClear();

    //! Create all sub components if not already done
    void createAllControlsIfNeeded();
    //! The study has been closed
    void onStudyClosed();
    //! Update all internal controls
    void internalUpdate();
    //! Back to input data
    void backToInputData(void*);
    //! All years / year by year selector
    void allYearsOrYearByYearSelector(Antares::Component::Button&, wxMenu& menu, void*);
    //! All variables
    void dropDownAllVariables(Antares::Component::Button&, wxMenu&, void*);
    //! Go to the next individual year
    void incrementYearByYear(void*);
    //! Go to the previous individual year
    void decrementYearByYear(void*);
    //! Go to a specific individual year
    void goToASpecificYear(void*);

    //! Open a new window
    void openNewWindow(void*);
    //! Ask to update from another event
    void updateFromExternalEvent(const Data::Output::List&, const Data::Output::Ptr selection);

    //! Drop down all outputs
    void onDropDownOutputs(Antares::Component::Button&, wxMenu& menu, void*);
    void onDropDownComparison(Antares::Component::Button&, wxMenu& menu, void*);
    void onDropDownDetachMenu(Antares::Component::Button&, wxMenu& menu, void*);

    //! Select an output
    void selectAnotherOutput(const Data::Output::Ptr& selectedOutput);
    //! Remove an output from the display
    void removeOutput(Layer* layer);

    //! Detach the current layer
    void onDetachCurrentLayer(wxCommandEvent& evt);

    //! Add a new output selection
    uint outputSelectionsAdd(Data::Output::Ptr output);
    //! Create a new virtual layer
    void createNewVirtualLayer(LayerType type);

    //! Select a data level
    void onSelectDataLevel(void*);
    //! Select a time level
    void onSelectTimeLevel(void*);

    //! Toggle sidebar
    void onToggleSidebar(void*);

    //! Get a study output is used in the output viewer
    const char* imageForLayerPlaceOrder(const Layer* so) const;
    const char* imageForLayerPlaceOrder(const Data::Output::Ptr& so) const;
    /*!
    ** \brief Get the image index for an output
    ** \return -1: *, -2: empty, >= 0 standard index
    */
    int imageIndexForOutput(const Data::Output::Ptr& output) const;
    //! Create the sub menu for switching orders
    void createSubMenuForSwitchingOrders(wxMenu* menu, const Layer* currentLayer) const;
    //! Rebuild internal indexes
    void rebuildIndexes();

    //! Refresh informations about the number of the current individual year
    void refreshCurrentYear();

    /*!
    ** \brief Display a frame with the given data provider
    */
    void displayMiniFrame(wxWindow* parent,
                          Antares::Component::Spotlight::IProvider* provider,
                          int width = 340,
                          bool searchInput = true,
                          bool groups = true);

    //! Reset the sash position
    void bugFixResetSashPosition();

    //! Start the merge of all outputs
    void mergeOutputs();
    //! Stop any merging currently in progress
    void stopMerging();

    //! Clear the main tree
    void treeDataClear();
    //! Load files and folders with a structure similiar to mc-all
    void treeDataWaiting();
    //! Update the tree accordingly
    void treeDataUpdate();
    //! Update the tree from a given content
    void treeDataUpdateWL(const Content& content);
    //! Check if an area is common to all outputs
    bool checkAreaIsCommonToAllOutputs(const Data::AreaName& name);
    //! Check if a link is common to all outputs
    bool checkLinkIsCommonToAllOutputs(const Data::AreaLinkName& name);
    //! Check year-by-year mode to all outputs
    void checkYearByYearMode();
    //! Update the tree data with "no data"
    void noSimulationData();
    //! Update the name of the current link/area
    void updateAreaOrLinkName();

    //! Update the global selection
    void updateGlobalSelection();

    void updateButtonView();

    /*!
    ** \brief Empty the internal cache related to the outputs
    */
    void emptyCache();

    /*!
    ** \brief Close any sub-frames
    */
    void closeSubFrames();

private:
    //! Alias to button
    typedef Antares::Component::Button Button;
    //! Array of wxPanel
    typedef std::vector<wxPanel*> WxPanelVector;

private:
    //! List of all output
    Data::Output::List pOutputs;
    //! The current selection
    Layer::Vector pSelections;
    //! The name of the current area or link
    Yuni::String pCurrentAreaOrLink;
    //! Get if the current selection on the list box is an area or a link
    SelectionType pCurrentSelectionType;
    //! Flag to know if the detailled data should be used instead
    LevelOfDetails pCurrentLOD;
    //! The current year (detailled view)
    uint pCurrentYear;
    //! Year interval
    uint pYearsLimits[2];
    //! Flag to know if individual years are present
    bool pHasYearByYear;
    //! Flag to know if concatened datasets are present (or can be generated)
    bool pHasConcatenedDataset;
    //! The current variable
    Yuni::String pCurrentVariable;
    //! The current variable ID (aka current variable without special characters)
    Yuni::String pCurrentVariableID;

    //! All thermal clusters for the current selection of outputs
    Antares::Private::OutputViewerData::ThermalNameSetPerArea* pCurrentClusters;

    //! All outputs
    Tab::Vector pTabs;
    //! Support for pAllTabs
    WxPanelVector pBtnPanelAllOutputs;
    //! Sizer used for buttons which deal with the display of all outputs
    wxSizer* pSizerForBtnOutputs;
    //! The window to use as parent for dynamyc buttons related to the outputs
    wxWindow* pWindowForBtnOutputs;
    //! All panels for all outputs
    Antares::Private::OutputViewerData::Panel::Vector pPanelAllOutputs;
    //! Sizer used for panels which deal with the display of all outputs
    wxSizer* pSizerForPanelOutputs;
    //! The window to use as parent for dynamyc panels related to the outputs
    wxWindow* pWindowForPanelOutputs;
    //! Event for scrolling all outputs
    Yuni::Event<void(wxScrolledWindow*)> pOnScrollAllPanels;
    //! Event for selecting a single for all panels
    Yuni::Event<void(uint, uint)> pOnSelectCellAllPanels;

    //! Button, all years or year-by-year
    Button* pBtnAllYears;

    //! Sizer about all MC years, year-by-year
    wxSizer* pSizerYearsToolbar;
    //! Sizer for all specific component related to the individual years
    wxSizer* pSizerYearByYearToolbar;
    //! Sizer related to the variables selection
    wxSizer* pSizerVariables;

    wxSizer* pSizerToolbar;

    //! The current UI Output info used by the menu
    Layer* pCurrentLayerForMenu;
    //! Name of the current area or link
    wxWindow* pLblAreaOrLinkName;

    //! Listbox for all areas / links found in outputs
    Antares::Component::Spotlight* pSpotlight;

    //! Flag to delay the creation of the UI controls
    bool pControlsAlreadyCreated;

    //! Separator
    wxSplitterWindow* pSplitter;
    wxWindow* pSplitterWindow1;
    wxWindow* pSplitterWindow2;
    //! Flag to know if the sidebar is collapsed
    bool pSidebarCollapsed;
    int pOldSidebarWidth;
    Button* pBtnExpandCollapseSidebar;

    //! The attached window (can be null)
    wxWindow* pParentWindow;

    //! Label where the user will find the name of the current area or the current link
    wxWindow* pLabelItemName;
    //! Data level: values
    Button* pBtnValues;
    //! Data level: details
    Button* pBtnDetails;
    //! Data level: id
    Button* pBtnID;

    //! Time level: hourly
    Button* pBtnHourly;
    //! Time level: daily
    Button* pBtnDaily;
    //! Time level: weekly
    Button* pBtnWeekly;
    //! Time level: monthly
    Button* pBtnMonthly;
    //! Time level: annual
    Button* pBtnAnnual;

    //! Year selection
    Button* pBtnCurrentYearNumber;
    //! All variables
    Button* pBtnVariables;

    //! \name Background process
    //@{
    //! Mutex
    Yuni::Mutex pMutex;
    //! Jobs
    Antares::Private::OutputViewerData::JobVector pJobs;
    //! All already prepared outputs
    // The access to this variable must be protected with pMutex
    // This variable is used to build the list of areas / links once
    // prepared.
    Antares::Private::OutputViewerData::ContentMap pAlreadyPreparedContents;
    //! The number of jobs which remain before full merging
    // The access to this variable must be protected with pMutex
    uint pJobsRemaining;
    //@}

    //! Flag to know if the component has been created for a new window
    bool pOpenedInANewWindow;

    //! Flag to know if a selection has already been made
    bool pASelectionHasAlreadyBeenMade;

    // friends !
    friend class Antares::Private::OutputViewerData::Job;
    friend class Antares::Private::OutputViewerData::Panel;
    friend class Tab;
    friend class SpotlightProvider;
    friend class SpotlightProviderGlobalSelection;
    friend class SpotlightProviderMCAll;
    friend class Provider::Variables;
    friend class Provider::Outputs;
    friend class Provider::Comparison;

}; // class Component

} // namespace OutputViewer
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_OUTPUT_H__

// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__
#define __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__

#include <ui/common/component/panel.h>
#include <antares/study/fwd.h>
#include <antares/study/scenario-builder/rules.h>
#include <antares/study/scenario-builder/sets.h>
#include "../../toolbox/components/button.h"

namespace Antares::Window::ScenarioBuilder
{
/*!
** \brief Panel to access to the simulation settings
*/
class Panel final: public Antares::Component::Panel, public Yuni::IEventObserver<Panel>
{
public:
    //! \name Constructor && Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    Panel(wxWindow* parent);
    //! Destructor
    virtual ~Panel();
    //@}

public:
    //! Event: A rules set has been changed
    Yuni::Event<void(Data::ScenarioBuilder::Rules::Ptr)> updateRules;

private:
    void onStudyClosed();
    void onStudyChanged(Data::Study&);
    void onFileMenu(Antares::Component::Button&, wxMenu& menu, void*);
    void onActiveMenu(Antares::Component::Button&, wxMenu& menu, void*);

    void onFileNew(wxCommandEvent&);
    void onFileDelete(wxCommandEvent&);
    void onFileRename(wxCommandEvent&);
    void onActiveRuleset(wxCommandEvent&);

    void scenarioBuilderDataAreLoaded();
    void update();

private:
    //! The current rule
    Data::ScenarioBuilder::Rules::Ptr pRules;
    //!
    Component::Button* pBtnFile;
    Component::Button* pBtnActive;
    //! Mapping between wxMenuItems ID and the ruleset
    Data::ScenarioBuilder::Rules::MappingID pActiveList;
    Data::ScenarioBuilder::Rules::MappingID pDeleteList;
    Data::ScenarioBuilder::Rules::MappingID pRenameList;

}; // class Panel

} // namespace Antares::Window::ScenarioBuilder

#endif // __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__

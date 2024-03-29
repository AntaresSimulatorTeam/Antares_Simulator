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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__
#define __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__

#include <ui/common/component/panel.h>
#include <antares/study/fwd.h>
#include <antares/study/scenario-builder/rules.h>
#include <antares/study/scenario-builder/sets.h>
#include "../../toolbox/components/button.h"

namespace Antares
{
namespace Window
{
namespace ScenarioBuilder
{
/*!
** \brief Panel to access to the simulation settings
*/
class Panel final : public Antares::Component::Panel, public Yuni::IEventObserver<Panel>
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

} // namespace ScenarioBuilder
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__

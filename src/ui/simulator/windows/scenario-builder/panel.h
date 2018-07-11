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
#ifndef __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__
# define __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__

# include <antares/wx-wrapper.h>
# include <ui/common/component/panel.h>
# include <antares/study/fwd.h>
# include <antares/study/scenario-builder/rules.h>
# include <antares/study/scenario-builder/sets.h>
# include "../../toolbox/components/button.h"




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
		Yuni::Event<void (Data::ScenarioBuilder::Rules::Ptr)>  updateRules;

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
		Data::ScenarioBuilder::Rules::Ptr  pRules;
		//!
		Component::Button* pBtnFile;
		Component::Button* pBtnActive;
		//! Mapping between wxMenuItems ID and the ruleset
		Data::ScenarioBuilder::Rules::MappingID  pActiveList;
		Data::ScenarioBuilder::Rules::MappingID  pDeleteList;
		Data::ScenarioBuilder::Rules::MappingID  pRenameList;

	}; // class Panel






} // namespace ScenarioBuilder
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_SCENARIO_BUILDER_PANEL_H__

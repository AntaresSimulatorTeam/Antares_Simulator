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

#include "../main.h"
#include "../study.h"
#include "../../windows/options/temp-folder/temp-folder.h"
#include "../../windows/options/playlist/playlist.h"
#include "../../windows/options/optimization/optimization.h"
#include "../../windows/options/advanced/advanced.h"
#include "../../windows/message.h"


namespace Antares
{
namespace Forms
{

	void ApplWnd::evtOnOptionsTempFolder(wxCommandEvent&)
	{
		Dispatcher::GUI::CreateAndShowModal<Window::Options::ConfigureTempFolder>(this);
	}


	void ApplWnd::evtOnOptionsMCPlaylist(wxCommandEvent&)
	{
		Forms::Disabler<ApplWnd> disabler(*this);
		if (Data::Study::Current::Valid())
			Dispatcher::GUI::CreateAndShowModal<Window::Options::MCPlaylist>(this);
	}

	void ApplWnd::evtOnOptionsDistricts(wxCommandEvent&)
	{
		Forms::Disabler<ApplWnd> disabler(*this);
		if (Data::Study::Current::Valid())
		{
			assert(NULL != pNotebook);
			pNotebook->select("sets");
		}
	}

	void ApplWnd::evtOnOptionsMCScenarioBuilder(wxCommandEvent&)
	{
		auto study = Data::Study::Current::Get();
		if (!study)
			return;

		Forms::Disabler<ApplWnd> disabler(*this);

		// Checking the minimum requirements
		if (study->areas.empty())
		{
			// No area, it is meaningless to go further
			Window::Message message(this, wxT("Configure MC Scenario Builder"),
				wxEmptyString,
				wxT("The study is empty. Please add one or more areas before using the MC Scenario builder"), "images/misc/warning.png");
			message.add(Window::Message::btnCancel, true);
			message.showModal();
		}
		else
		{
			// Open the MC Scenario Builder
			pSectionNotebook->select(wxT("scenariobuilder"), true);
		}
	}


	void ApplWnd::evtOnOptionsOptimizationPrefs(wxCommandEvent&)
	{
		Forms::Disabler<ApplWnd> disabler(*this);
		if (Data::Study::Current::Valid())
			Dispatcher::GUI::CreateAndShowModal<Window::Options::Optimization>(this);
	}


	void ApplWnd::evtOnOptionsAdvanced(wxCommandEvent&)
	{
		Forms::Disabler<ApplWnd> disabler(*this);
		if (Data::Study::Current::Valid())
			Dispatcher::GUI::CreateAndShowModal<Window::Options::AdvancedParameters>(this);
	}






} // namespace Forms
} // namespace Antares


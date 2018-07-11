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
#include "../../../windows/xcast/xcast.h"
#include "../../../toolbox/input/area.h"
#include "../../../windows/correlation/correlation.h"
#include "../../../toolbox/components/datagrid/renderer/area/nodal-optimization.h"
#include "standard-page.hxx"


using namespace Yuni;


namespace Antares
{
namespace Forms
{


	void ApplWnd::createNBNodalOptimization()
	{
		assert(NULL != pNotebook);

		// Create a standard page with an input selector
		std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> page =
			createStdNotebookPage<Toolbox::InputSelector::Area>(pNotebook,
			wxT("nodal"), wxT("Economic Opt."));

		pageNodalOptim = page.first->add(
			new Component::Datagrid::Component(page.first,
			new Component::Datagrid::Renderer::NodalOptimization(page.first), wxEmptyString, true, true,
				false, false, true),
			wxT("All areas"));
		pageNodalOptim->displayExtraControls(false);
	}




} // namespace Forms
} // namespace Antares


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

#include "series.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/area/timeseries.h"


using namespace Yuni;


namespace Antares
{
namespace Window
{
namespace Hydro
{


	Series::Series(wxWindow* parent, Toolbox::InputSelector::Area* notifier) :
		Component::Panel(parent),
		pNotifier(notifier),
		pLastArea(nullptr)
	{
		auto* notebook = new Component::Notebook(this, Component::Notebook::orTop);
		notebook->displayTitle(false);
		notebook->theme(Component::Notebook::themeLight);

		auto* com = new Component::Datagrid::Component(notebook);
		com->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroFatal(com, notifier));
		pPageFatal = notebook->add(com, wxT("Run-of-the-river (ROR)"));

		com = new Component::Datagrid::Component(notebook);
		com->renderer(new Component::Datagrid::Renderer::TimeSeriesHydroMod(com, notifier));
		notebook->add(com, wxT("Hydro Storage"));

		// Connection to the notifier
		if (pNotifier)
			pNotifier->onAreaChanged.connect(this, &Series::onAreaChanged);
		OnStudyClosed.connect(this, &Series::onStudyClosed);

		wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
		sizer->Add(notebook, 1, wxALL|wxEXPAND);
		SetSizer(sizer);

		OnStudyLoaded.connect(this, &Series::onStudyLoaded);
	}



	Series::~Series()
	{
		destroyBoundEvents();
	}


	void Series::onAreaChanged(Data::Area* area)
	{
		pLastArea = Data::Study::Current::Valid() ? area : nullptr;
	}


	void Series::onStudyClosed()
	{
		pLastArea = nullptr;
		if (pNotifier)
			pNotifier->onAreaChanged(nullptr);
	}


	void Series::onStudyLoaded()
	{
		if (pPageFatal)
			pPageFatal->select();
	}




} // namespace Hydro
} // namespace Window
} // namespace Antares


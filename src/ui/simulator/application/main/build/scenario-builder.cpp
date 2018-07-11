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
#include "../../../toolbox/input/area.h"
#include "../../../toolbox/components/datagrid/component.h"
#include "../../../toolbox/components/datagrid/renderer/scenario-builder.h"
#include "standard-page.hxx"
#include "../../../windows/scenario-builder/panel.h"
#include "../../../windows/output/output.h"


using namespace Yuni;


namespace Antares
{
namespace Forms
{

	namespace // anonymous
	{

		template<enum Data::TimeSeries T>
		void CreateSCComponent(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook, const wxString& name,
			const wxString& caption, bool withAreaInput = false)
		{
			typedef Component::Datagrid::Component  DatagridType;
			typedef Component::Datagrid::Renderer::ScenarioBuilder  RendererType;

			// The new renderer

			if (withAreaInput)
			{
				// Create a standard page with an input selector
				std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> page =
					createStdNotebookPage<Toolbox::InputSelector::Area>(notebook, name, caption);

				RendererType* renderer = new RendererType(page.second);
				control->updateRules.connect(renderer, &RendererType::onRulesChanged);
				DatagridType* grid = new DatagridType(page.first, renderer);
				page.first->add(grid, wxT("thermal"), wxT("Thermal"));
				renderer->control(grid);
				page.first->select(wxT("thermal"));
			}
			else
			{
				RendererType* renderer = new RendererType(T);
				control->updateRules.connect(renderer, &RendererType::onRulesChanged);
				DatagridType* grid = new DatagridType(notebook, renderer);

				notebook->add(grid, name, caption);
				renderer->control(grid);
			}
		}

	} // anonymous namespace



	void ApplWnd::onScenarioBuilderNotebookPageChanging(Component::Notebook::Page& page)
	{
		if (page.name() == wxT("back"))
			backToInputData();
	}


	void ApplWnd::onOutputNotebookPageChanging(Component::Notebook::Page& page)
	{
		if (page.name() == wxT("back"))
			backToInputData();
	}


	void ApplWnd::createNBScenarioBuilder()
	{
		// Scenario Builder
		pScenarioBuilderNotebook = new Component::Notebook(pSectionNotebook);
		pScenarioBuilderNotebook->onPageChanged.connect(this, &ApplWnd::onScenarioBuilderNotebookPageChanging);
		pSectionNotebook->add(pScenarioBuilderNotebook, wxT("scenariobuilder"), wxT("scenariobuilder"));

		// Title
		Window::ScenarioBuilder::Panel* control = new Window::ScenarioBuilder::Panel(pScenarioBuilderNotebook);
		pScenarioBuilderNotebook->addCommonControlTop(control, 0, wxPoint(100, 60));

		// Back to standard edition
		pScenarioBuilderNotebook->add(new wxPanel(pScenarioBuilderNotebook), wxT("back"),
			wxT("  Back to input data"));
		pScenarioBuilderNotebook->addSeparator();

		CreateSCComponent<Data::timeSeriesLoad>(control, pScenarioBuilderNotebook, wxT("load"), wxT("Load"));
		CreateSCComponent<Data::timeSeriesThermal>(control, pScenarioBuilderNotebook, wxT("thermal"), wxT("Thermal"), true);
		CreateSCComponent<Data::timeSeriesHydro>(control, pScenarioBuilderNotebook, wxT("hydro"), wxT("Hydro"));
		CreateSCComponent<Data::timeSeriesWind>(control, pScenarioBuilderNotebook, wxT("wind"), wxT("Wind"));
		CreateSCComponent<Data::timeSeriesSolar>(control, pScenarioBuilderNotebook, wxT("solar"), wxT("Solar"));
	}



	void ApplWnd::createNBOutputViewer()
	{
		wxWindow* output = new Window::OutputViewer::Component(pSectionNotebook);
		pSectionNotebook->add(output, wxT("output"), wxT("output"));
		return;
		pOutputViewerNotebook = new Component::Notebook(pSectionNotebook);
		pOutputViewerNotebook->onPageChanged.connect(this, &ApplWnd::onScenarioBuilderNotebookPageChanging);
		pSectionNotebook->add(pOutputViewerNotebook, wxT("output"), wxT("output"));

		// Back to standard edition
		pOutputViewerNotebook->add(new wxPanel(pOutputViewerNotebook), wxT("back"), wxT("  Back to input data"));
		pOutputViewerNotebook->addSeparator();
	}





} // namespace Forms
} // namespace Antares


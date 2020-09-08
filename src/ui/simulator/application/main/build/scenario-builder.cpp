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
#include "standard-page.hxx"
#include "../../../windows/scenario-builder/panel.h"
#include "../../../windows/output/output.h"

#include "../../../toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h"
#include "../../../toolbox/components/datagrid/renderer/scenario-builder-load-renderer.h"
#include "../../../toolbox/components/datagrid/renderer/scenario-builder-thermal-renderer.h"
#include "../../../toolbox/components/datagrid/renderer/scenario-builder-hydro-renderer.h"
#include "../../../toolbox/components/datagrid/renderer/scenario-builder-wind-renderer.h"
#include "../../../toolbox/components/datagrid/renderer/scenario-builder-solar-renderer.h"

using namespace Yuni;
using namespace Component::Datagrid;


namespace Antares
{
namespace Forms
{

	namespace // anonymous
	{
		// Basic class ...
		class basicScBuilderGrid
		{
		protected:
			typedef Component::Datagrid::Component  DatagridType;

		public:
			basicScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook)
				: control_(control), notebook_(notebook), renderer_(nullptr)
			{}

			virtual void create()
			{
				createRenderer();
				control_->updateRules.connect(renderer_, &Renderer::ScBuilderRendererBase::onRulesChanged);
				createGrid();
				addToNotebook();
				renderer_->control(grid_);
			}

		protected:
			virtual void createRenderer() = 0;
			virtual void createGrid() { grid_ = new DatagridType(notebook_, renderer_); }
			virtual void addToNotebook() = 0;

		protected:
			Window::ScenarioBuilder::Panel* control_;
			Component::Notebook* notebook_;
			Renderer::ScBuilderRendererBase* renderer_;
			DatagridType* grid_;
		};

		// Load ...
		class loadScBuilderGrid : public basicScBuilderGrid
		{
		public:
			loadScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) : basicScBuilderGrid(control, notebook) {}

		private:
			void createRenderer()
			{ 
				renderer_ = new Renderer::loadScBuilderRenderer();
			}
			void addToNotebook() { notebook_->add(grid_, wxT("load"), wxT("Load")); }
		};

		
		// Hydro ...
		class hydroScBuilderGrid : public basicScBuilderGrid
		{
		public:
			hydroScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) : basicScBuilderGrid(control, notebook) {}

		private:
			void createRenderer() { renderer_ = new Renderer::hydroScBuilderRenderer(); }
			void addToNotebook() { notebook_->add(grid_, wxT("hydro"), wxT("Hydro")); }
		};

		
		// Wind ...
		class windScBuilderGrid : public basicScBuilderGrid
		{
		public:
			windScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) : basicScBuilderGrid(control, notebook) {}

		private:
			void createRenderer() { renderer_ = new Renderer::windScBuilderRenderer(); }
			void addToNotebook() { notebook_->add(grid_, wxT("wind"), wxT("Wind")); }
		};
		
		// Solar ...
		class solarScBuilderGrid : public basicScBuilderGrid
		{
		public:
			solarScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) : basicScBuilderGrid(control, notebook) {}

		private:
			void createRenderer() { renderer_ = new Renderer::solarScBuilderRenderer(); }
			void addToNotebook() { notebook_->add(grid_, wxT("solar"), wxT("Solar")); }
		};

		// Thermal ...
		class thermalScBuilderGrid : public basicScBuilderGrid
		{
		public:
			thermalScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) : basicScBuilderGrid(control, notebook) {}

			void create()
			{
				page_ = createStdNotebookPage<Toolbox::InputSelector::Area>(notebook_, wxT("thermal"), wxT("Thermal"));
				createRenderer();
				control_->updateRules.connect(renderer_, &Renderer::thermalScBuilderRenderer::onRulesChanged);
				createGrid();
				addToNotebook();
			}

		private:
			void createRenderer() { renderer_ = new Renderer::thermalScBuilderRenderer(page_.second); }
			void createGrid() { grid_ = new DatagridType(page_.first, renderer_); }
			void addToNotebook()
			{
				page_.first->add(grid_, wxT("thermal"), wxT("Thermal"));
				renderer_->control(grid_); // gp : Shouldn't that be inside create() ?
				page_.first->select(wxT("thermal"));
			}

		private:
			std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> page_;
		};

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

		// Creating scenario builder notebook's tabs
		loadScBuilderGrid loadScBuilder(control, pScenarioBuilderNotebook);
		loadScBuilder.create();

		thermalScBuilderGrid thermalScBuilder(control, pScenarioBuilderNotebook);
		thermalScBuilder.create();

		hydroScBuilderGrid hydroScBuilder(control, pScenarioBuilderNotebook);
		hydroScBuilder.create();

		windScBuilderGrid windScBuilder(control, pScenarioBuilderNotebook);
		windScBuilder.create();

		solarScBuilderGrid solarScBuilder(control, pScenarioBuilderNotebook);
		solarScBuilder.create();
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


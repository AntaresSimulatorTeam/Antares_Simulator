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

#include "application/main/main.h"
#include "toolbox/components/datagrid/component.h"
#include "standard-page.hxx"
#include "windows/scenario-builder/panel.h"
#include "windows/output/output.h"

#include "toolbox/components/datagrid/renderer/scenario-builder-renderer-base.h"
#include "toolbox/components/datagrid/renderer/scenario-builder-load-renderer.h"
#include "toolbox/components/datagrid/renderer/scenario-builder-thermal-renderer.h"
#include "toolbox/components/datagrid/renderer/scenario-builder-renewable-renderer.h"
#include "toolbox/components/datagrid/renderer/scenario-builder-hydro-renderer.h"
#include "toolbox/components/datagrid/renderer/scenario-builder-wind-renderer.h"
#include "toolbox/components/datagrid/renderer/scenario-builder-solar-renderer.h"
#include "toolbox/components/datagrid/renderer/scenario-builder-hydro-levels-renderer.h"

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
    typedef Component::Datagrid::Component DatagridType;

public:
    basicScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) :
        control_(control), notebook_(notebook), renderer_(nullptr)
    {}

    virtual void create()
    {
        addAreaSelectorPage();
        createRenderer();
        createGrid();
        addGridToNotebook();
        connectUpdateRulesEventToFunction();
    }

protected:
    virtual void addAreaSelectorPage() {/* by default : does nothing */ }
    virtual void createRenderer() = 0;
    virtual void createGrid()
    {
        grid_ = new DatagridType(notebook_, renderer_);
        renderer_->control(grid_);
    }
    void connectUpdateRulesEventToFunction()
    {
        control_->updateRules.connect(renderer_, &Renderer::ScBuilderRendererBase::onRulesChanged);
    }
    virtual void addGridToNotebook() = 0;

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
    loadScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) :
     basicScBuilderGrid(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::loadScBuilderRenderer();
    }
    void addGridToNotebook()
    {
        notebook_->add(grid_, wxT("load"), wxT("Load"));
    }
};

// Hydro ...
class hydroScBuilderGrid : public basicScBuilderGrid
{
public:
    hydroScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) :
     basicScBuilderGrid(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::hydroScBuilderRenderer();
    }
    void addGridToNotebook()
    {
        notebook_->add(grid_, wxT("hydro"), wxT("Hydro"));
    }
};

// Wind ...
class windScBuilderGrid : public basicScBuilderGrid
{
public:
    windScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) :
     basicScBuilderGrid(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::windScBuilderRenderer();
    }
    void addGridToNotebook()
    {
        notebook_->add(grid_, wxT("wind"), wxT("Wind"));
    }
};

// Solar ...
class solarScBuilderGrid : public basicScBuilderGrid
{
public:
    solarScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) :
     basicScBuilderGrid(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::solarScBuilderRenderer();
    }
    void addGridToNotebook()
    {
        notebook_->add(grid_, wxT("solar"), wxT("Solar"));
    }
};

class clusterScBuilderGrid : public basicScBuilderGrid
{
public:
    clusterScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) :
        basicScBuilderGrid(control, notebook)
    {}

private:
    virtual const char* name() const = 0;
    virtual const char* caption() const = 0;

    void addAreaSelectorPage() override
    {
        area_selector_page_ = createStdNotebookPage<Toolbox::InputSelector::Area>(notebook_, name(), caption());
    }

    virtual void createGrid() override
    {
        grid_ = new DatagridType(area_selector_page_.first, renderer_);
        renderer_->control(grid_);
    }

    virtual void addGridToNotebook() override
    {
        area_selector_page_.first->add(grid_, name(), caption());
        area_selector_page_.first->select(name());
    }

protected:
    std::pair<Component::Notebook*, Toolbox::InputSelector::Area*> area_selector_page_;
};

// Thermal clusters ...
class thermalScBuilderGrid : public clusterScBuilderGrid
{
public:
    thermalScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) :
        clusterScBuilderGrid(control, notebook)
    {}

    virtual void createRenderer() override
    {
        renderer_ = new Renderer::thermalScBuilderRenderer(area_selector_page_.second);
    }

    virtual const char* name() const override
    {
        return "thermal";
    }

    virtual const char* caption() const override
    {
        return "Thermal";
    }
};

// Renewable clusters ...
class renewableScBuilderGrid : public clusterScBuilderGrid
{
public:
    renewableScBuilderGrid(Window::ScenarioBuilder::Panel* control, Component::Notebook* notebook) :
     clusterScBuilderGrid(control, notebook)
    {}

    virtual void createRenderer() override
    {
        renderer_ = new Renderer::renewableScBuilderRenderer(area_selector_page_.second);
    }

    virtual const char* name() const override
    {
        return "renewable";
    }

    virtual const char* caption() const override
    {
        return "Renewable";
    }
};

// Hydro levels ...
class hydroLevelsScBuilderGrid : public basicScBuilderGrid
{
public:
    hydroLevelsScBuilderGrid(Window::ScenarioBuilder::Panel* control,
                             Component::Notebook* notebook) :
     basicScBuilderGrid(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::hydroLevelsScBuilderRenderer();
    }
    void addGridToNotebook()
    {
        notebook_->add(grid_, wxT("hydro levels"), wxT("Hydro Levels"));
    }
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
    Window::ScenarioBuilder::Panel* control
      = new Window::ScenarioBuilder::Panel(pScenarioBuilderNotebook);
    pScenarioBuilderNotebook->addCommonControlTop(control, 0, wxPoint(100, 60));

    // Back to standard edition
    pScenarioBuilderNotebook->add(
      new wxPanel(pScenarioBuilderNotebook), wxT("back"), wxT("  Back to input data"));
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

    renewableScBuilderGrid renewableScBuilder(control, pScenarioBuilderNotebook);
    renewableScBuilder.create();

    pScenarioBuilderNotebook->addSeparator();

    hydroLevelsScBuilderGrid hydroLevelsScBuilder(control, pScenarioBuilderNotebook);
    hydroLevelsScBuilder.create();
}

void ApplWnd::createNBOutputViewer()
{
    wxWindow* output = new Window::OutputViewer::Component(pSectionNotebook);
    pSectionNotebook->add(output, wxT("output"), wxT("output"));
    return;
    pOutputViewerNotebook = new Component::Notebook(pSectionNotebook);
    pOutputViewerNotebook->onPageChanged.connect(this,
                                                 &ApplWnd::onScenarioBuilderNotebookPageChanging);
    pSectionNotebook->add(pOutputViewerNotebook, wxT("output"), wxT("output"));

    // Back to standard edition
    pOutputViewerNotebook->add(
      new wxPanel(pOutputViewerNotebook), wxT("back"), wxT("  Back to input data"));
    pOutputViewerNotebook->addSeparator();
}

} // namespace Forms
} // namespace Antares

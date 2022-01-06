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
#include "toolbox/components/datagrid/renderer/scenario-builder-ntc-renderer.h"

using namespace Yuni;

using namespace Component;
using namespace Component::Datagrid;


namespace Antares
{
namespace Forms
{
namespace // anonymous
{
// Basic class ...
class basicScBuilderPageMaker
{
protected:
    typedef Component::Datagrid::Component DatagridType;

public:
    basicScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
        control_(control), notebook_(notebook), renderer_(nullptr)
    {}
    virtual ~basicScBuilderPageMaker() = default;

    Notebook::Page* createPage()
    {
        addSelectorPage();
        createRenderer();
        rendererHandlesRulesEvent();
        createGrid();
        return addPageToNotebook();
    }

protected:
    virtual void addSelectorPage()
    { 
        // In the scenario builder, a selector page can be an area or link selector page.
        // For example : for thermal, you need to select an area to get a grid (rows : clusters, columns : years) 
        // by default : does nothing. Should be overloaded to add a selector page
    }
    virtual void createRenderer() = 0;
    virtual void createGrid()
    {
        grid_ = new DatagridType(notebook_, renderer_);
        renderer_->control(grid_);
    }
    void rendererHandlesRulesEvent()
    {
        control_->updateRules.connect(renderer_, &Renderer::ScBuilderRendererBase::onRulesChanged);
    }
    virtual Notebook::Page* addPageToNotebook() = 0;

protected:
    Window::ScenarioBuilder::Panel* control_;
    Notebook* notebook_;
    Renderer::ScBuilderRendererBase* renderer_ = nullptr;
    DatagridType* grid_ = nullptr;
    // Notebook::Page* notebookPage_ = nullptr;
};

// Load ...
class loadScBuilderPageMaker : public basicScBuilderPageMaker
{
public:
    loadScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
     basicScBuilderPageMaker(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::loadScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook()
    {
        return notebook_->add(grid_, wxT("load"), wxT("Load"));
    }
};

// Hydro ...
class hydroScBuilderPageMaker : public basicScBuilderPageMaker
{
public:
    hydroScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
     basicScBuilderPageMaker(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::hydroScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook()
    {
        return notebook_->add(grid_, wxT("hydro"), wxT("Hydro"));
    }
};

// Wind ...
class windScBuilderPageMaker : public basicScBuilderPageMaker
{
public:
    windScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
     basicScBuilderPageMaker(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::windScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook()
    {
        return notebook_->add(grid_, wxT("wind"), wxT("Wind"));
    }
};

// Solar ...
class solarScBuilderPageMaker : public basicScBuilderPageMaker
{
public:
    solarScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
     basicScBuilderPageMaker(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::solarScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook()
    {
        return notebook_->add(grid_, wxT("solar"), wxT("Solar"));
    }
};

// Links NTC ...
class ntcScBuilderPageMaker final : public basicScBuilderPageMaker
{
public:
    ntcScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
        basicScBuilderPageMaker(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::ntcScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook()
    {
        return notebook_->add(grid_, wxT("ntc"), wxT("NTC"));
    }
};


class clusterScBuilderPageMaker : public basicScBuilderPageMaker
{
public:
    clusterScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
        basicScBuilderPageMaker(control, notebook)
    {}

private:
    virtual const char* name() const = 0;
    virtual const char* caption() const = 0;

    void addSelectorPage() override
    {
        area_selector_page_ = createStdNotebookPage<Toolbox::InputSelector::Area>(notebook_, name(), caption());
    }

    void createGrid() override
    {
        grid_ = new DatagridType(area_selector_page_.first, renderer_);
        renderer_->control(grid_);
    }

    Notebook::Page* addPageToNotebook() override
    {
        notebookPage_ = area_selector_page_.first->add(grid_, name(), caption());
        area_selector_page_.first->select(name());
        return notebookPage_;
    }

protected:
    std::pair<Notebook*, Toolbox::InputSelector::Area*> area_selector_page_;
    Notebook::Page* notebookPage_ = nullptr;
};

// Thermal clusters ...
class thermalScBuilderPageMaker final : public clusterScBuilderPageMaker
{
public:
    thermalScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
        clusterScBuilderPageMaker(control, notebook)
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
class renewableScBuilderPageMaker final : public clusterScBuilderPageMaker
{
public:
    renewableScBuilderPageMaker(Window::ScenarioBuilder::Panel* control, Notebook* notebook) :
     clusterScBuilderPageMaker(control, notebook)
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
class hydroLevelsScBuilderPageMaker : public basicScBuilderPageMaker
{
public:
    hydroLevelsScBuilderPageMaker(Window::ScenarioBuilder::Panel* control,
                             Notebook* notebook) :
     basicScBuilderPageMaker(control, notebook)
    {}

private:
    void createRenderer()
    {
        renderer_ = new Renderer::hydroLevelsScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook()
    {
        return notebook_->add(grid_, wxT("hydro levels"), wxT("Hydro Levels"));
    }
};

} // anonymous namespace

void ApplWnd::onScenarioBuilderNotebookPageChanging(Notebook::Page& page)
{
    if (page.name() == wxT("back"))
        backToInputData();
}

void ApplWnd::onOutputNotebookPageChanging(Notebook::Page& page)
{
    if (page.name() == wxT("back"))
        backToInputData();
}

void ApplWnd::createNBScenarioBuilder()
{
    // Scenario Builder
    pScenarioBuilderNotebook = new Notebook(pSectionNotebook);
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
    loadScBuilderPageMaker loadSBpageMaker(control, pScenarioBuilderNotebook);
    pageScBuilderLoad = loadSBpageMaker.createPage();

    thermalScBuilderPageMaker thermalSBpageMaker(control, pScenarioBuilderNotebook);
    pageScBuilderThermal = thermalSBpageMaker.createPage();

    hydroScBuilderPageMaker hydroSBpageMaker(control, pScenarioBuilderNotebook);
    pageScBuilderHydro = hydroSBpageMaker.createPage();

    windScBuilderPageMaker windSBpageMaker(control, pScenarioBuilderNotebook);
    pageScBuilderWind = windSBpageMaker.createPage();

    solarScBuilderPageMaker solarSBpageMaker(control, pScenarioBuilderNotebook);
    pageScBuilderSolar = solarSBpageMaker.createPage();

    ntcScBuilderPageMaker ntcSBpageMaker(control, pScenarioBuilderNotebook);
    pageScBuilderNTC = ntcSBpageMaker.createPage();

    renewableScBuilderPageMaker renewableSBpageMaker(control, pScenarioBuilderNotebook);
    pageScBuilderRenewable = renewableSBpageMaker.createPage();

    pScenarioBuilderNotebook->addSeparator();

    hydroLevelsScBuilderPageMaker hydroLevelsSBpageMaker(control, pScenarioBuilderNotebook);
    pageScBuilderHydroLevels = hydroLevelsSBpageMaker.createPage();
}

void ApplWnd::createNBOutputViewer()
{
    wxWindow* output = new Window::OutputViewer::Component(pSectionNotebook);
    pSectionNotebook->add(output, wxT("output"), wxT("output"));
    return;
    pOutputViewerNotebook = new Notebook(pSectionNotebook);
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

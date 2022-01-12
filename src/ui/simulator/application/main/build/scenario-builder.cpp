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
using namespace Toolbox;


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
    basicScBuilderPageMaker(Window::ScenarioBuilder::Panel* scenarioBuilderPanel, Notebook* notebook) :
        scBuilderPanel_(scenarioBuilderPanel), notebook_(notebook)
    {}
    virtual ~basicScBuilderPageMaker() = default;

    Notebook::Page* createPage()
    {
        addAreaSelectorPage();
        createRenderer();
        rendererHandlesRulesEvent();
        createGrid();
        return addPageToNotebook();
    }

protected:
    Notebook* notebook() { return notebook_; }
    Renderer::ScBuilderRendererBase* renderer() { return renderer_; }
    DatagridType* grid() { return grid_; }

private:
    // Private methods
    // ---------------
    virtual void addAreaSelectorPage() = 0;

    virtual Renderer::ScBuilderRendererBase* getRenderer() = 0;
    virtual void createRenderer()
    {
        renderer_ = getRenderer();
    }
    void rendererHandlesRulesEvent()
    {
        scBuilderPanel_->updateRules.connect(renderer_, &Renderer::ScBuilderRendererBase::onRulesChanged);
    }
    void createGrid()
    {
        grid_ = getGrid();
        renderer()->control(grid_);
    }
    virtual DatagridType* getGrid() = 0;
    virtual Notebook::Page* addPageToNotebook() = 0;

    // Private attibutes
    // -----------------
    Window::ScenarioBuilder::Panel* scBuilderPanel_;
    Notebook* notebook_;
    Renderer::ScBuilderRendererBase* renderer_ = nullptr;
    DatagridType* grid_ = nullptr;
};


// Simple scenario builder page maker : makes page with no area selector

class simpleScBuilderPageMaker : public basicScBuilderPageMaker
{
    using basicScBuilderPageMaker::basicScBuilderPageMaker;

public:
    ~simpleScBuilderPageMaker() override = default;

private:
    void addAreaSelectorPage() override
    {
        // In the scenario builder, a selector page can be an area or link selector page.
        // For example : for thermal, you need to select an area to get a grid (rows : clusters, columns : years) 
        // by default : does nothing. Should be overloaded to add a selector page
    }
    DatagridType* getGrid() override
    {
        return new DatagridType(notebook(), renderer());
    }
};

// Load ...
class loadScBuilderPageMaker final : public simpleScBuilderPageMaker
{
    using simpleScBuilderPageMaker::simpleScBuilderPageMaker;

    Renderer::ScBuilderRendererBase* getRenderer() override
    {
        return new Renderer::loadScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook() override
    {
        return notebook()->add(grid(), wxT("load"), wxT("Load"));
    }
};

// Hydro ...
class hydroScBuilderPageMaker final : public simpleScBuilderPageMaker
{
    using simpleScBuilderPageMaker::simpleScBuilderPageMaker;

    Renderer::ScBuilderRendererBase* getRenderer() override
    {
        return new Renderer::hydroScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook() override
    {
        return notebook()->add(grid(), wxT("hydro"), wxT("Hydro"));
    }
};

// Wind ...
class windScBuilderPageMaker final : public simpleScBuilderPageMaker
{
    using simpleScBuilderPageMaker::simpleScBuilderPageMaker;
    Renderer::ScBuilderRendererBase* getRenderer() override
    {
        return new Renderer::windScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook() override
    {
        return notebook()->add(grid(), wxT("wind"), wxT("Wind"));
    }
};

// Solar ...
class solarScBuilderPageMaker final : public simpleScBuilderPageMaker
{
    using simpleScBuilderPageMaker::simpleScBuilderPageMaker;

    Renderer::ScBuilderRendererBase* getRenderer() override
    {
        return new Renderer::solarScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook() override
    {
        return notebook()->add(grid(), wxT("solar"), wxT("Solar"));
    }
};

// Hydro levels ...
class hydroLevelsScBuilderPageMaker final : public simpleScBuilderPageMaker
{
    using simpleScBuilderPageMaker::simpleScBuilderPageMaker;

    Renderer::ScBuilderRendererBase* getRenderer() override
    {
        return new Renderer::hydroLevelsScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook() override
    {
        return notebook()->add(grid(), wxT("hydro levels"), wxT("Hydro Levels"));
    }
};

// Links NTC ...
class ntcScBuilderPageMaker final : public simpleScBuilderPageMaker
{
    using simpleScBuilderPageMaker::simpleScBuilderPageMaker;

    Renderer::ScBuilderRendererBase* getRenderer() override
    {
        return new Renderer::ntcScBuilderRenderer();
    }
    Notebook::Page* addPageToNotebook() override
    {
        return notebook()->add(grid(), wxT("ntc"), wxT("NTC"));
    }
};


class clusterScBuilderPageMaker : public basicScBuilderPageMaker
{
    using basicScBuilderPageMaker::basicScBuilderPageMaker;

    virtual const char* name() const = 0;
    virtual const char* caption() const = 0;

    void addAreaSelectorPage() override
    {
        area_selector_page_ = createStdNotebookPage<InputSelector::Area>(notebook(), name(), caption());
    }
    DatagridType* getGrid() override
    {
        return new DatagridType(area_selector_page_.first, renderer());
    }
    Notebook::Page* addPageToNotebook() override
    {
        notebookPage_ = area_selector_page_.first->add(grid(), name(), caption());
        area_selector_page_.first->select(name());
        return notebookPage_;
    }

protected:
    InputSelector::Area* areaSelectorPage()
    {
        return area_selector_page_.second;
    }

private:
    std::pair<Notebook*, InputSelector::Area*> area_selector_page_;
    Notebook::Page* notebookPage_ = nullptr;
};

// Thermal clusters ...
class thermalScBuilderPageMaker final : public clusterScBuilderPageMaker
{
    using clusterScBuilderPageMaker::clusterScBuilderPageMaker;

public:
    Renderer::ScBuilderRendererBase* getRenderer() override
    {
        return new Renderer::thermalScBuilderRenderer(areaSelectorPage());
    }

    const char* name() const override
    {
        return "thermal";
    }

    const char* caption() const override
    {
        return "Thermal";
    }
};

// Renewable clusters ...
class renewableScBuilderPageMaker final : public clusterScBuilderPageMaker
{
    using clusterScBuilderPageMaker::clusterScBuilderPageMaker;

public:
    Renderer::ScBuilderRendererBase* getRenderer() override
    {
        return  new Renderer::renewableScBuilderRenderer(areaSelectorPage());
    }

    const char* name() const override
    {
        return "renewable";
    }

    const char* caption() const override
    {
        return "Renewable";
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
    pScenarioBuilderMainPage = pSectionNotebook->add(pScenarioBuilderNotebook, wxT("scenariobuilder"), wxT("scenariobuilder"));

    // Title
    auto* scenarioBuilderPanel = new Window::ScenarioBuilder::Panel(pScenarioBuilderNotebook);
    pScenarioBuilderNotebook->addCommonControlTop(scenarioBuilderPanel, 0, wxPoint(100, 60));

    // Back to standard edition
    pScenarioBuilderNotebook->add(
      new wxPanel(pScenarioBuilderNotebook), wxT("back"), wxT("  Back to input data"));
    pScenarioBuilderNotebook->addSeparator();

    // Creating scenario builder notebook's tabs
    loadScBuilderPageMaker loadSBpageMaker(scenarioBuilderPanel, pScenarioBuilderNotebook);
    pageScBuilderLoad = loadSBpageMaker.createPage();

    thermalScBuilderPageMaker thermalSBpageMaker(scenarioBuilderPanel, pScenarioBuilderNotebook);
    pageScBuilderThermal = thermalSBpageMaker.createPage();

    hydroScBuilderPageMaker hydroSBpageMaker(scenarioBuilderPanel, pScenarioBuilderNotebook);
    pageScBuilderHydro = hydroSBpageMaker.createPage();

    windScBuilderPageMaker windSBpageMaker(scenarioBuilderPanel, pScenarioBuilderNotebook);
    pageScBuilderWind = windSBpageMaker.createPage();

    solarScBuilderPageMaker solarSBpageMaker(scenarioBuilderPanel, pScenarioBuilderNotebook);
    pageScBuilderSolar = solarSBpageMaker.createPage();

    ntcScBuilderPageMaker ntcSBpageMaker(scenarioBuilderPanel, pScenarioBuilderNotebook);
    pageScBuilderNTC = ntcSBpageMaker.createPage();

    renewableScBuilderPageMaker renewableSBpageMaker(scenarioBuilderPanel, pScenarioBuilderNotebook);
    pageScBuilderRenewable = renewableSBpageMaker.createPage();

    pScenarioBuilderNotebook->addSeparator();

    hydroLevelsScBuilderPageMaker hydroLevelsSBpageMaker(scenarioBuilderPanel, pScenarioBuilderNotebook);
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

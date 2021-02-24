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

#include "correlation.h"
#include "../../application/study.h"
#include "../../toolbox/components/notebook/notebook.h"
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/renderer/correlation.h"
#include "../../toolbox/components/refresh.h"
#include <antares/date.h>
#include "../../toolbox/resources.h"
#include <antares/array/correlation.h>

#include <wx/tglbtn.h>
#include <wx/stattext.h>

#include "datasources.hxx"

using namespace Yuni;

namespace Antares
{
namespace Window
{
class CorrelationPanelData final
{
public:
    typedef Component::Datagrid::Renderer::CorrelationMatrix CorrelationMatrixType;

public:
    CorrelationPanelData() : pCorrelation(nullptr)
    {
        for (uint i = 0; i != 12; ++i)
            pGridMonthly[i] = nullptr;
        for (uint i = 0; i != 12 + 1; ++i)
            renderer[i] = nullptr;
    }

public:
    Data::Correlation* pCorrelation;
    int timeseries;
    Component::Datagrid::Component* pGridAnnual;
    Component::Datagrid::Component* pGridMonthly[12];
    CorrelationMatrixType* renderer[12 + 1];
    CorrelationMatrixType::IDatasource::Ptr datasource;
};

CorrelationPanel::CorrelationPanel(wxWindow* parent, int timeseries) : wxPanel(parent, wxID_ANY)
{
    // Init
    pData = new CorrelationPanelData();
    pData->timeseries = timeseries;
    pData->datasource = new DatasourceAlphaOrder();
    for (uint i = 0; i != 12 + 1; ++i)
    {
        pData->renderer[i] = new Component::Datagrid::Renderer::CorrelationMatrix();
        pData->renderer[i]->datasource(pData->datasource);
    }

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Panel
    {
        static const wchar_t* const months[] = {L"Jan",
                                                L"Feb",
                                                L"Mar",
                                                L"Apr",
                                                L"May",
                                                L"Jun",
                                                L"Jul",
                                                L"Aug",
                                                L"Sep",
                                                L"Oct",
                                                L"Nov",
                                                L"Dec"};

        Component::Notebook* notebook = new Component::Notebook(this, Component::Notebook::orTop);
        notebook->displayTitle(false);
        notebook->theme(Component::Notebook::themeLight);
        notebook->alignment(Component::Notebook::alignRight);

        wxWindow* panel = notebook->titlePanelControl();
        panel->GetSizer()->AddSpacer(20);
        wxBitmapButton* btn;

// Sort - alpha
#ifdef YUNI_OS_WINDOWS
        enum
        {
            margin = 3
        };
#else
        enum
        {
            margin = 0
        };
#endif

        btn
          = Resources::BitmapButtonLoadFromFile(panel, wxID_ANY, "images/16x16/sort_alphabet.png");
        btn->Connect(btn->GetId(),
                     wxEVT_COMMAND_BUTTON_CLICKED,
                     wxCommandEventHandler(CorrelationPanel::onSortAlpha),
                     nullptr,
                     this);
        panel->GetSizer()->Add(btn, 0, wxALL | wxEXPAND, margin);

        // Sort - reverse
        btn = Resources::BitmapButtonLoadFromFile(
          panel, wxID_ANY, "images/16x16/sort_alphabet_descending.png");
        btn->Connect(btn->GetId(),
                     wxEVT_COMMAND_BUTTON_CLICKED,
                     wxCommandEventHandler(CorrelationPanel::onSortAlphaReverse),
                     nullptr,
                     this);
        panel->GetSizer()->Add(btn, 0, wxALL | wxEXPAND, margin);

        // Sort - color
        btn = Resources::BitmapButtonLoadFromFile(panel, wxID_ANY, "images/16x16/color.png");
        btn->Connect(btn->GetId(),
                     wxEVT_COMMAND_BUTTON_CLICKED,
                     wxCommandEventHandler(CorrelationPanel::onSortColor),
                     nullptr,
                     this);
        panel->GetSizer()->Add(btn, 0, wxALL | wxEXPAND, margin);

        if (timeseries != Data::timeSeriesHydro)
        {
            for (uint i = 0; i != 12; ++i)
            {
                pData->pGridMonthly[i]
                  = new Component::Datagrid::Component(notebook, pData->renderer[i]);
                pData->renderer[i]->control(pData->pGridMonthly[i]);
                notebook->add(pData->pGridMonthly[i], months[i]);
            }
        }

        pData->pGridAnnual = new Component::Datagrid::Component(notebook, pData->renderer[12]);
        pData->renderer[12]->control(pData->pGridAnnual);
        pPageAnnual = notebook->add(pData->pGridAnnual, wxT("annual"), wxT("Annual"));

        sizer->Add(notebook, 1, wxALL | wxEXPAND);
    }

    // Rebuilding the layout
    sizer->Layout();
    SetSizer(sizer);

    // Events
    OnStudyClosed.connect(this, &CorrelationPanel::onStudyClosed);
    OnStudyLoaded.connect(this, &CorrelationPanel::onStudyLoaded);
    OnStudyEndUpdate.connect(this, &CorrelationPanel::reload);
    OnStudyAreaDelete.connect(this, &CorrelationPanel::onStudyAreaDelete);
    OnStudyAreaAdded.connect(this, &CorrelationPanel::onStudyAreaAdded);
    OnStudyAreasChanged.connect(this, &CorrelationPanel::onStudyAreasChanged);
}

void CorrelationPanel::selectAllDefaultPages()
{
    pPageAnnual->select();
}

CorrelationPanel::~CorrelationPanel()
{
    // Destroying internal data
    CorrelationPanelData* data = pData;
    pData = nullptr;
    delete data;

    // Destroying bound events
    destroyBoundEvents();

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    wxSizer* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void CorrelationPanel::onStudyLoaded()
{
    pData->datasource = new DatasourceAlphaOrder();
    updateAllDatasources();
    pPageAnnual->select();
}

void CorrelationPanel::onStudyClosed()
{
    reload();
}

void CorrelationPanel::onStudyAreaDelete(Data::Area*)
{
    reload();
}

void CorrelationPanel::onStudyAreasChanged()
{
    reload();
}

void CorrelationPanel::onStudyAreaAdded(Data::Area*)
{
    reload();
}

void CorrelationPanel::reload()
{
    assignMatrices(nullptr);
    if (not pData)
        return;

    pData->datasource->reload();

    // Reset to nullptr
    pData->pCorrelation = nullptr;
    // Retrieving the good pointer to the correlation coefficients
    auto study = Data::Study::Current::Get();
    if (!(!study))
    {
        switch (pData->timeseries)
        {
        case Data::timeSeriesLoad:
            pData->pCorrelation = &(study->preproLoadCorrelation);
            break;
        case Data::timeSeriesSolar:
            pData->pCorrelation = &(study->preproSolarCorrelation);
            break;
        case Data::timeSeriesWind:
            pData->pCorrelation = &(study->preproWindCorrelation);
            break;
        case Data::timeSeriesHydro:
            pData->pCorrelation = &(study->preproHydroCorrelation);
            break;
        }
    }

    // It is useless to reassign matrices since they are already
    // assigned to null
    if (pData->pCorrelation)
        assignMatrices(pData->pCorrelation);

    // Force refresh
    RefreshAllControls(pData->pGridAnnual);
    for (uint i = 0; i != 12; ++i)
        RefreshAllControls(pData->pGridMonthly[i]);
}

void CorrelationPanel::updateAllDatasources()
{
    pData->datasource->reload();
    for (uint i = 0; i != 12 + 1; ++i)
    {
        if (pData->renderer[i])
            pData->renderer[i]->datasource(pData->datasource);
    }

    for (uint i = 0; i != 12; ++i)
    {
        if (pData->pGridMonthly[i])
            pData->pGridMonthly[i]->forceRefresh();
    }
    pData->pGridAnnual->forceRefresh();
}

void CorrelationPanel::onSortAlpha(wxCommandEvent&)
{
    pData->datasource = new DatasourceAlphaOrder();
    updateAllDatasources();
}

void CorrelationPanel::onSortAlphaReverse(wxCommandEvent&)
{
    pData->datasource = new DatasourceReverseAlphaOrder();
    updateAllDatasources();
}

void CorrelationPanel::onSortColor(wxCommandEvent&)
{
    pData->datasource = new DatasourceColorOrder();
    updateAllDatasources();
}

void CorrelationPanel::assignMatrices(Data::Correlation* corr)
{
    if (pData)
    {
        if (corr == NULL)
        {
            for (uint i = 0; i < 12 + 1; ++i)
            {
                if (pData->renderer)
                    pData->renderer[i]->matrix(nullptr);
            }
        }
        else
        {
            for (uint i = 0; i < 12; ++i)
            {
                if (pData->renderer[i])
                    pData->renderer[i]->matrix(&(corr->monthly[i]));
            }
            pData->renderer[12]->matrix(corr->annual);
        }
    }
}

} // namespace Window
} // namespace Antares

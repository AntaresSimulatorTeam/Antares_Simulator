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
#include "layers.h"
#include "../../../../windows/inspector.h"
#include "../component.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
LayersUI::LayersUI(Toolbox::InputSelector::Area* notifier) : pControl(nullptr), pArea(nullptr)
{
    if (notifier)
    {
        // Event: The current selected area
        notifier->onAreaChanged.connect(this, &LayersUI::onAreaChanged);
    }
}

LayersUI::~LayersUI()
{
    destroyBoundEvents();
}

bool LayersUI::valid() const
{
    return !(!study) && study->areas.size() != 0 && pArea;
}

int LayersUI::width() const
{
    if (!(!study))
    {
        return study->layers.size();
    }
    return 0;
}

int LayersUI::height() const
{
    return (!study) ? 0 : 3; // switch to 4?
}

void LayersUI::onAreaChanged(Data::Area* area)
{
    if (area != pArea)
    {
        pArea = area;
        onRefresh();
        if (pControl)
            pControl->Refresh();
    }
}

wxString LayersUI::columnCaption(int colIndx) const
{
    if (study)
    {
        if ((uint)colIndx < study->layers.size())
        {
            auto layerIterator = study->layers.begin();
            std::advance(layerIterator, colIndx);
            return wxString() << wxT(" ") << wxStringFromUTF8(layerIterator->second) << wxT("  ");
        }
    }
    return wxEmptyString;
}

wxString LayersUI::rowCaption(int rowIndx) const
{
    switch (rowIndx)
    {
    case 0:
        return wxT("   Visible");
    case 1:
        return wxT("   X");
    case 2:
        return wxT("   Y");
        /*					case 3: return wxT("  Color");*/
    }
    return wxEmptyString;
}

// TODO
bool LayersUI::cellValue(int x, int y, const Yuni::String& value)
{
    if (!study || (uint)x >= study->layers.size())
        return false;
    if (x == 0 && y == 0)
        return true;

    auto layerIterator = study->layers.begin();
    std::advance(layerIterator, x);
    std::vector<size_t>& layerList = pArea->ui->mapLayersVisibilityList;
    auto layerPosition = std::find(layerList.begin(), layerList.end(), layerIterator->first);
    switch (y)
    {
    case 0: // Visibility
    {
        bool b = value.to<bool>();

        if (b)
        {
            if (layerPosition == layerList.end())
                layerList.push_back(layerIterator->first);
            pArea->ui->layerX[layerIterator->first] = pArea->ui->layerX[0];
            pArea->ui->layerY[layerIterator->first] = pArea->ui->layerY[0];
            pControl->Refresh();
        }
        else
        {
            if (layerPosition != layerList.end())
                layerList.erase(layerPosition);
        }
        Window::Inspector::Refresh();
        OnInspectorRefresh(nullptr);
        return true;
        break;
    }
    case 1: // X
    {
        int intValue;
        if (!value.to(intValue))
            intValue = 0;

        if (layerPosition != layerList.end())
        {
            pArea->ui->layerX[layerIterator->first] = intValue;
            Window::Inspector::Refresh();
            OnInspectorRefresh(nullptr);
            return true;
        }
        return false;
    }

    case 2: // Y
    {
        int intValue;
        if (!value.to(intValue))
            intValue = 0;

        if (layerPosition != layerList.end())
        {
            pArea->ui->layerY[layerIterator->first] = intValue;
            Window::Inspector::Refresh();
            OnInspectorRefresh(nullptr);
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}

double LayersUI::cellNumericValue(int x, int y) const
{
    if (!(!study) && (uint)x < study->layers.size() && pArea)
    {
        auto layerIterator = study->layers.begin();
        std::advance(layerIterator, x);
        std::vector<size_t>& layerList = pArea->ui->mapLayersVisibilityList;
        auto layerListEnd = layerList.end();
        auto layerPosition = std::find(layerList.begin(), layerListEnd, layerIterator->first);
        switch (y)
        {
        case 0:

            return layerPosition != layerListEnd ? 1. : 0.;

        case 1:

            return layerPosition != layerListEnd ? pArea->ui->layerX[layerIterator->first] : 0.;

        case 2:

            return layerPosition != layerListEnd ? pArea->ui->layerY[layerIterator->first] : 0.;
        }
    }
    return 0.;
}

wxString LayersUI::cellValue(int x, int y) const
{
    if (!(!study) && (uint)x < study->layers.size() && pArea)
    {
        auto layerIterator = study->layers.begin();
        std::advance(layerIterator, x);
        std::vector<size_t>& layerList = pArea->ui->mapLayersVisibilityList;
        auto layerListEnd = layerList.end();
        auto layerPosition = std::find(layerList.begin(), layerListEnd, layerIterator->first);
        switch (y)
        {
        case 0:

            return layerPosition != layerListEnd ? wxT("Yes") : wxT("No");

        case 1:

            return layerPosition != layerListEnd
                     ? wxString::Format(wxT("%d"), pArea->ui->layerX[layerIterator->first])
                     : wxT("-");

        case 2:

            return layerPosition != layerListEnd
                     ? wxString::Format(wxT("%d"), pArea->ui->layerY[layerIterator->first])
                     : wxT("-");
        }
    }
    return wxEmptyString;
}

IRenderer::CellStyle LayersUI::cellStyle(int x, int y) const
{
    if (x == 0 && y == 0)
        return cellStyleLayerDisabled;
    return cellStyleDefaultCenter;
}

void LayersUI::onStudyClosed()
{
    pArea = nullptr;
    IRenderer::onStudyClosed();
}

/*layers visibility grid renderer*/
LayersVisibility::LayersVisibility() : pControl(nullptr)
{
}

LayersVisibility::~LayersVisibility()
{
    destroyBoundEvents();
}

bool LayersVisibility::valid() const
{
    return !(!study) && study->areas.size() != 0;
}

int LayersVisibility::width() const
{
    if (!(!study))
    {
        return study->layers.size();
    }
    return 0;
}

int LayersVisibility::height() const
{
    return (!study) ? 0 : study->areas.size();
}

wxString LayersVisibility::columnCaption(int colIndx) const
{
    if (study)
    {
        if ((uint)colIndx < study->layers.size())
        {
            auto layerIterator = study->layers.begin();
            std::advance(layerIterator, colIndx);
            return wxString() << wxT(" ") << wxStringFromUTF8(layerIterator->second) << wxT("  ");
        }
    }
    return wxEmptyString;
}

wxString LayersVisibility::rowCaption(int rowIndx) const
{
    if (study)
    {
        if ((uint)rowIndx < study->areas.size())
        {
            auto areaIterator = study->areas.begin();
            std::advance(areaIterator, rowIndx);
            return wxString() << wxT(" ") << wxStringFromUTF8(areaIterator->first) << wxT("  ");
        }
    }
    return wxEmptyString;
}

// TODO
bool LayersVisibility::cellValue(int x, int y, const Yuni::String& value)
{
    if (!study || (uint)x >= study->layers.size() || (uint)y >= study->areas.size())
        return false;
    if (x == 0)
        return true;

    auto layerIterator = study->layers.begin();
    std::advance(layerIterator, x);
    auto areaIterator = study->areas.begin();
    std::advance(areaIterator, y);
    Antares::Data::Area* pArea = areaIterator->second;
    std::vector<size_t>& layerList = pArea->ui->mapLayersVisibilityList;
    auto layerPosition = std::find(layerList.begin(), layerList.end(), layerIterator->first);
    bool b = value.to<bool>();

    if (b)
    {
        if (layerPosition == layerList.end())
            layerList.push_back(layerIterator->first);
        pArea->ui->layerX[layerIterator->first] = pArea->ui->layerX[0];
        pArea->ui->layerY[layerIterator->first] = pArea->ui->layerY[0];
    }
    else
    {
        if (layerPosition != layerList.end())
            layerList.erase(layerPosition);
    }
    Window::Inspector::Refresh();
    OnInspectorRefresh(nullptr);
    return true;
}

double LayersVisibility::cellNumericValue(int x, int y) const
{
    if (!(!study) && (uint)x < study->layers.size() && (uint)y < study->areas.size())
    {
        auto layerIterator = study->layers.begin();
        std::advance(layerIterator, x);
        auto areaIterator = study->areas.begin();
        std::advance(areaIterator, y);
        Antares::Data::Area* pArea = areaIterator->second;
        std::vector<size_t>& layerList = pArea->ui->mapLayersVisibilityList;
        auto layerPosition = std::find(layerList.begin(), layerList.end(), layerIterator->first);

        return layerPosition != layerList.end() ? 1. : 0.;
    }
    return 0.;
}

wxString LayersVisibility::cellValue(int x, int y) const
{
    if (!(!study) && (uint)x < study->layers.size() && (uint)y < study->areas.size())
    {
        auto layerIterator = study->layers.begin();
        std::advance(layerIterator, x);
        auto areaIterator = study->areas.begin();
        std::advance(areaIterator, y);
        Antares::Data::Area* pArea = areaIterator->second;
        std::vector<size_t>& layerList = pArea->ui->mapLayersVisibilityList;
        auto layerPosition = std::find(layerList.begin(), layerList.end(), layerIterator->first);

        return layerPosition != layerList.end() ? wxT("Yes") : wxT("No");
    }
    return wxEmptyString;
}

IRenderer::CellStyle LayersVisibility::cellStyle(int x, int y) const
{
    if (x == 0)
        return cellStyleLayerDisabled;
    return cellStyleDefaultCenter;
}

void LayersVisibility::onStudyClosed()
{
    IRenderer::onStudyClosed();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

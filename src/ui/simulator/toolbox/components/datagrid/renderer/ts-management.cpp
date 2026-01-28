// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "cell.h"
#include "ts-management.h"
#include <wx/panel.h>

using namespace Yuni;

namespace Antares::Component::Datagrid::Renderer
{
using namespace Antares::Data;

TSmanagement::TSmanagement():
    pControl(nullptr)
{
    columns_.push_back(new classicColumn(timeSeriesLoad, "      Load      "));
    columns_.push_back(new thermalColumn());
    columns_.push_back(new classicColumn(timeSeriesHydro, "      Hydro      "));
}

void TSmanagement::checkLineNumberInColumns()
{
    for (auto it = columns_.begin(); it != columns_.end(); it++)
    {
        assert((*it)->getNumberOfLines() == MAX_NB_OF_LINES);
    }
}

TSmanagement::~TSmanagement()
{
    for (auto& c: columns_)
    {
        delete c;
    }
    destroyBoundEvents();
}

int TSmanagement::width() const
{
    return columns_.size();
}

int TSmanagement::height() const
{
    return MAX_NB_OF_LINES;
}

wxString TSmanagement::columnCaption(int colIndx) const
{
    if (colIndx < width())
    {
        return columns_[colIndx]->getCaption();
    }
    return wxEmptyString;
}

wxString TSmanagement::rowCaption(int rowIndx) const
{
    static const wxChar* const captions[] = {
      wxT("Ready made TS"),
      wxT("        Status"),
      wxT("Stochastic TS"),
      wxT("        Status"),
      wxT("        Number"),
      wxT("        Refresh: DEPRECATED"),
      wxT("        Refresh span: DEPRECATED"),
      wxT("        Seasonal correlation "),
      wxT("        Store in input  "),
      wxT("        Store in output  "),
      wxT("Draws correlation  "),
      wxT("        intra-modal"),
      wxT("        inter-modal"),
    };
    if (rowIndx < height())
    {
        return captions[rowIndx];
    }
    return wxEmptyString;
}

bool TSmanagement::cellValue(int x, int y, const String& value)
{
    if (not study || x < 0 || x > width())
    {
        return false;
    }

    bool to_return = columns_[x]->getLine(y)->setCellValue(value);
    onSimulationTSManagementChanged();
    return to_return;
}

double TSmanagement::cellNumericValue(int x, int y) const
{
    if (not study || x < 0 || x > width())
    {
        return 0.;
    }
    return columns_[x]->getLine(y)->cellNumericValue();
}

wxString TSmanagement::cellValue(int x, int y) const
{
    if (not study || x < 0 || x > width())
    {
        return wxEmptyString;
    }

    return columns_[x]->getLine(y)->cellValue();
}

void TSmanagement::onSimulationTSManagementChanged()
{
    if (pControl)
    {
        pControl->InvalidateBestSize();
        pControl->Refresh();
    }
}

IRenderer::CellStyle TSmanagement::cellStyle(int x, int y) const
{
    if (not study || x < 0 || x > width())
    {
        return IRenderer::cellStyleError;
    }

    return columns_[x]->getLine(y)->cellStyle();
}

wxColour TSmanagement::horizontalBorderColor(int x, int y) const
{
    if (y == 1 || y == 9)
    {
        return Default::BorderDaySeparator();
    }
    return IRenderer::verticalBorderColor(x, y);
}

} // namespace Antares::Component::Datagrid::Renderer

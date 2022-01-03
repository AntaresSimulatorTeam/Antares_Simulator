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

#include "cell.h"
#include "ts-management.h"
#include <wx/panel.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
using namespace Antares::Data;

TSmanagement::TSmanagement() : pControl(nullptr)
{
    columns_.push_back(new classicColumn(timeSeriesLoad, "      Load      "));
    columns_.push_back(new classicColumn(timeSeriesThermal, "   Thermal   "));
    columns_.push_back(new classicColumn(timeSeriesHydro, "      Hydro      "));
    columns_.push_back(new ColumnNTC());
}

void TSmanagement::checkLineNumberInColumns()
{
    for (auto it = columns_.begin(); it != columns_.end(); it++)
        assert((*it)->getNumberOfLines() == MAX_NB_OF_LINES);
}

TSmanagement::~TSmanagement()
{
    for (auto& c : columns_)
        delete c;
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
        return columns_[colIndx]->getCaption();
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
      wxT("        Refresh"),
      wxT("        Refresh span"),
      wxT("        Seasonal correlation "),
      wxT("        Store in input  "),
      wxT("        Store in output  "),
      wxT("Draws correlation  "),
      wxT("        intra-modal"),
      wxT("        inter-modal"),
    };
    if (rowIndx < height())
        return captions[rowIndx];
    return wxEmptyString;
}

bool TSmanagement::cellValue(int x, int y, const String& value)
{
    if (not study || x < 0 || x > width())
        return false;

    bool to_return = columns_[x]->getLine(y)->cellValue(value);
    onSimulationTSManagementChanged();
    return to_return;
}

double TSmanagement::cellNumericValue(int x, int y) const
{
    if (not study || x < 0 || x > width())
        return 0.;
    return columns_[x]->getLine(y)->cellNumericValue();
}

wxString TSmanagement::cellValue(int x, int y) const
{
    if (not study || x < 0 || x > width())
        return wxEmptyString;

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
        return IRenderer::cellStyleError;

    return columns_[x]->getLine(y)->cellStyle();
}

wxColour TSmanagement::horizontalBorderColor(int x, int y) const
{
    if (y == 1 || y == 9)
        return Default::BorderDaySeparator();
    return IRenderer::verticalBorderColor(x, y);
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

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
#include <memory>

#include "column.h"
#include "cell.h"
#include <antares/logs.h>
#include <memory>

// Anonymous namespace
namespace
{
template<class T, class... Args>
inline T* factory(Args... args)
{
    auto ptr = new (std::nothrow) T(args...);
    if (ptr == nullptr)
    {
        Antares::logs.error() << "Resource allocation failed";
    }
    return ptr;
}
} // namespace

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
// -------------------
// Base column class
// -------------------
Column::Column(Antares::Data::TimeSeries ts, const wxString& caption) : tsKind_(ts), caption_(caption)
{
}

Cell* Column::getLine(int y) const
{
    return cells_[y];
}

wxString Column::getCaption() const
{
    return caption_;
}

int Column::getNumberOfLines() const
{
    return cells_.size();
}

Column::~Column()
{
    for (auto& cell : cells_)
        delete cell;
}
// -------------------- -
// Classic column
// ---------------------
classicColumn::classicColumn(TimeSeries ts, const wxString& caption) : Column(ts, caption)
{
    cells_ = {::factory<blankCell>(),
              ::factory<readyMadeTSstatus>(tsKind_),
              ::factory<blankCell>(),
              ::factory<generatedTSstatus>(tsKind_),
              ::factory<NumberTsCell>(tsKind_),
              ::factory<RefreshTsCell>(tsKind_),
              ::factory<RefreshSpanCell>(tsKind_),
              ::factory<SeasonalCorrelationCell>(tsKind_),
              ::factory<storeToInputCell>(tsKind_),
              ::factory<storeToOutputCell>(tsKind_),
              ::factory<blankCell>(),
              ::factory<intraModalCell>(tsKind_),
              ::factory<interModalCell>(tsKind_)};
}

// -------------------
//  Thermal column
// -------------------
thermalColumn::thermalColumn() : Column(timeSeriesThermal, "   Thermal   ")
{
    cells_ = {::factory<blankCell>(),
              ::factory<readyMadeTSstatus>(tsKind_),
              ::factory<blankCell>(),
              ::factory<generatedTSstatus>(tsKind_),
              ::factory<NumberTsCellThermal>(),
              ::factory<RefreshTsCellThermal>(),
              ::factory<RefreshSpanCellThermal>(),
              ::factory<SeasonalCorrelationCellThermal>(),
              ::factory<storeToInputCell>(tsKind_),
              ::factory<storeToOutputCell>(tsKind_),
              ::factory<blankCell>(),
              ::factory<intraModalCell>(tsKind_),
              ::factory<interModalCell>(tsKind_)};
}

// -------------------------------
// Column renewable clusters
// -------------------------------
ColumnRenewableClusters::ColumnRenewableClusters() : Column(timeSeriesRenewable, "   Renewable   ")
{
    cells_ = {::factory<blankCell>(),
              ::factory<inactiveCell>(wxT("On")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<blankCell>(),
              ::factory<intraModalCell>(tsKind_),
              ::factory<interModalCell>(tsKind_)};
}

// -------------------------------
// Column for NTC
// -------------------------------
ColumnNTC::ColumnNTC() : Column(timeSeriesTransmissionCapacities, "  Links NTC  ")
{
    cells_ = {::factory<blankCell>(),
              ::factory<inactiveCell>(wxT("On")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<inactiveCell>(wxT("-")),
              ::factory<blankCell>(),
              ::factory<intraModalCell>(tsKind_),
              ::factory<inactiveCell>(wxT("-"))};
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

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

#include "column.h"
#include "cell.h"
#include <antares/memory/new_check.hxx>

using namespace Antares::MemoryUtils;

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
    cells_ = {new_check_allocation<blankCell>(),
              new_check_allocation<readyMadeTSstatus>(tsKind_),
              new_check_allocation<blankCell>(),
              new_check_allocation<generatedTSstatus>(tsKind_),
              new_check_allocation<NumberTsCell>(tsKind_),
              new_check_allocation<RefreshTsCell>(tsKind_),
              new_check_allocation<RefreshSpanCell>(tsKind_),
              new_check_allocation<SeasonalCorrelationCell>(tsKind_),
              new_check_allocation<storeToInputCell>(tsKind_),
              new_check_allocation<storeToOutputCell>(tsKind_),
              new_check_allocation<blankCell>(),
              new_check_allocation<intraModalCell>(tsKind_),
              new_check_allocation<interModalCell>(tsKind_)};
}

// -------------------------------
// Column renewable clusters
// -------------------------------
ColumnRenewableClusters::ColumnRenewableClusters() : Column(timeSeriesRenewable, "   Renewable   ")
{
    cells_ = {new_check_allocation<blankCell>(),
              new_check_allocation<inactiveCell>(wxT("On")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<blankCell>(),
              new_check_allocation<intraModalCell>(tsKind_),
              new_check_allocation<interModalCell>(tsKind_)};
}

// -------------------------------
// Column for NTC
// -------------------------------
ColumnNTC::ColumnNTC() : Column(timeSeriesTransmissionCapacities, "  Links NTC  ")
{
    cells_ = {new_check_allocation<blankCell>(),
              new_check_allocation<inactiveCell>(wxT("On")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<inactiveCell>(wxT("-")),
              new_check_allocation<blankCell>(),
              new_check_allocation<intraModalCell>(tsKind_),
              new_check_allocation<inactiveCell>(wxT("-"))};
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <memory>

#include "column.h"
#include "cell.h"
#include <antares/memory/new_check.hxx>

using namespace Antares::MemoryUtils;

// Anonymous namespace
namespace
{
template<class T, class... Args>
inline T* factory(Args... args)
{
    T* ptr = new (std::nothrow) T(args...);
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
Column::Column(Antares::Data::TimeSeriesType ts, const wxString& caption) :
 tsKind_(ts), caption_(caption)
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
classicColumn::classicColumn(TimeSeriesType ts, const wxString& caption) : Column(ts, caption)
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

// -------------------
//  Thermal column
// -------------------
thermalColumn::thermalColumn() : Column(timeSeriesThermal, "   Thermal   ")
{
    cells_ = {new_check_allocation<blankCell>(),
              new_check_allocation<readyMadeTSstatus>(tsKind_),
              new_check_allocation<blankCell>(),
              new_check_allocation<generatedTSstatus>(tsKind_),
              new_check_allocation<NumberTsCellThermal>(),
              new_check_allocation<RefreshTsCellThermal>(),
              new_check_allocation<RefreshSpanCellThermal>(),
              new_check_allocation<SeasonalCorrelationCellThermal>(),
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

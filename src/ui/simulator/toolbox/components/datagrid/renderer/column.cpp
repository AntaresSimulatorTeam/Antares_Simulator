// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

namespace Antares::Component::Datagrid::Renderer
{
// -------------------
// Base column class
// -------------------
Column::Column(Antares::Data::TimeSeriesType ts, const wxString& caption):
    tsKind_(ts),
    caption_(caption)
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
    for (auto& cell: cells_)
    {
        delete cell;
    }
}

// -------------------- -
// Classic column
// ---------------------
classicColumn::classicColumn(TimeSeriesType ts, const wxString& caption):
    Column(ts, caption)
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
thermalColumn::thermalColumn():
    Column(timeSeriesThermal, "   Thermal   ")
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
ColumnRenewableClusters::ColumnRenewableClusters():
    Column(timeSeriesRenewable, "   Renewable   ")
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
ColumnNTC::ColumnNTC():
    Column(timeSeriesTransmissionCapacities, "  Links NTC  ")
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
} // namespace Antares::Component::Datagrid::Renderer

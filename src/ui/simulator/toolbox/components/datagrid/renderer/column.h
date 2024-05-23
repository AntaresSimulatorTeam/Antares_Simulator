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

#pragma once

#include <vector>
#include <wx/string.h>
#include <antares/study/fwd.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class Cell;

// -------------------
//  Base column class
// -------------------
class Column
{
public:
    Column(Antares::Data::TimeSeriesType ts, const wxString& caption);
    virtual ~Column();
    Cell* getLine(int y) const;
    int getNumberOfLines() const;
    wxString getCaption() const;

protected:
    std::vector<Cell*> cells_;
    Antares::Data::TimeSeriesType tsKind_;
    wxString caption_;
};

// -------------------
//  Classic column
// -------------------
class classicColumn final : public Column
{
public:
    classicColumn(Antares::Data::TimeSeriesType ts, const wxString& caption);
    ~classicColumn() override = default;
};

// -------------------
//  Thermal column
// -------------------
class thermalColumn final : public Column
{
public:
    thermalColumn();
};

// -----------------------------
//  Renewable clusters column
// -----------------------------
class ColumnRenewableClusters final : public Column
{
public:
    ColumnRenewableClusters();
    ~ColumnRenewableClusters() override = default;
};

// -------------------------------
// Column for NTC
// -------------------------------
class ColumnNTC final : public Column
{
public:
    ColumnNTC();
    ~ColumnNTC() override = default;
};
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

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
    Column(Antares::Data::TimeSeries ts, const wxString& caption);
    virtual ~Column();
    Cell* getLine(int y) const;
    int getNumberOfLines() const;
    wxString getCaption() const;

protected:
    std::vector<Cell*> cells_;
    Antares::Data::TimeSeries tsKind_;
    wxString caption_;
};

// -------------------
//  Classic column
// -------------------
class classicColumn final : public Column
{
public:
    classicColumn(Antares::Data::TimeSeries ts, const wxString& caption);
    ~classicColumn() override = default;
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

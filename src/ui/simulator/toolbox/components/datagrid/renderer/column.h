// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#pragma once

#include <vector>
#include <wx/string.h>
#include <antares/study/fwd.h>

namespace Antares::Component::Datagrid::Renderer
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
class classicColumn final: public Column
{
public:
    classicColumn(Antares::Data::TimeSeriesType ts, const wxString& caption);
    ~classicColumn() override = default;
};

// -------------------
//  Thermal column
// -------------------
class thermalColumn final: public Column
{
public:
    thermalColumn();
};

// -----------------------------
//  Renewable clusters column
// -----------------------------
class ColumnRenewableClusters final: public Column
{
public:
    ColumnRenewableClusters();
    ~ColumnRenewableClusters() override = default;
};

// -------------------------------
// Column for NTC
// -------------------------------
class ColumnNTC final: public Column
{
public:
    ColumnNTC();
    ~ColumnNTC() override = default;
};
} // namespace Antares::Component::Datagrid::Renderer

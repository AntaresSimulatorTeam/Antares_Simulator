/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#pragma once

#include <wx/string.h>
#include "../renderer.h"
#include <map>

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

class Cell : public Yuni::IEventObserver<Cell>
{
public:
    Cell(TimeSeriesType ts);
    ~Cell();
    virtual wxString cellValue() const = 0;
    virtual double cellNumericValue() const = 0;
    virtual bool setCellValue(const String& value) = 0;
    virtual IRenderer::CellStyle cellStyle() const = 0;

protected:
    void onStudyLoaded();
    virtual bool isTSgeneratorOn() const;

protected:
    Data::Study::Ptr study_;
    TimeSeriesType tsKind_;
};

class blankCell : public Cell
{
public:
    blankCell();
    ~blankCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class inactiveCell : public Cell
{
public:
    inactiveCell(wxString toPrintInCell);
    ~inactiveCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;

private:
    wxString toBePrintedInCell_;
};

class readyMadeTSstatus : public Cell
{
public:
    readyMadeTSstatus(TimeSeriesType ts);
    ~readyMadeTSstatus() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class generatedTSstatus : public Cell
{
public:
    generatedTSstatus(TimeSeriesType ts);
    ~generatedTSstatus() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class NumberTsCell : public Cell
{
public:
    NumberTsCell(TimeSeriesType ts);
    ~NumberTsCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    std::map<TimeSeriesType, uint*> tsToNumberTs_;
};

class NumberTsCellThermal : public NumberTsCell
{
public:
    NumberTsCellThermal();
    IRenderer::CellStyle cellStyle() const override;
};

class RefreshTsCell : public Cell
{
public:
    RefreshTsCell(TimeSeriesType ts);
    ~RefreshTsCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class RefreshTsCellThermal : public RefreshTsCell
{
public:
    RefreshTsCellThermal();
    IRenderer::CellStyle cellStyle() const override;
};

class RefreshSpanCell : public Cell
{
public:
    RefreshSpanCell(TimeSeriesType ts);
    ~RefreshSpanCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    std::map<TimeSeriesType, uint*> tsToRefreshSpan_;
};

class RefreshSpanCellThermal : public RefreshSpanCell
{
public:
    RefreshSpanCellThermal();
    IRenderer::CellStyle cellStyle() const override;
};

class SeasonalCorrelationCell : public Cell
{
public:
    SeasonalCorrelationCell(TimeSeriesType ts);
    ~SeasonalCorrelationCell() = default;
    virtual wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    std::map<TimeSeriesType, Correlation*> tsToCorrelation_;
};

class SeasonalCorrelationCellThermal : public SeasonalCorrelationCell
{
public:
    SeasonalCorrelationCellThermal();
    wxString cellValue() const override;
    IRenderer::CellStyle cellStyle() const override;
};

class storeToInputCell : public Cell
{
public:
    storeToInputCell(TimeSeriesType ts);
    ~storeToInputCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class storeToOutputCell : public Cell
{
public:
    storeToOutputCell(TimeSeriesType ts);
    ~storeToOutputCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class intraModalCell : public Cell
{
public:
    intraModalCell(TimeSeriesType ts);
    ~intraModalCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class interModalCell : public Cell
{
public:
    interModalCell(TimeSeriesType ts);
    ~interModalCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool setCellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

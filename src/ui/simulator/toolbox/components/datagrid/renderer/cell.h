
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
using namespace std;

class Cell : public Yuni::IEventObserver<Cell>
{
public:
    Cell(TimeSeries ts);
    ~Cell();
    virtual wxString cellValue() const = 0;
    virtual double cellNumericValue() const = 0;
    virtual bool cellValue(const String& value) = 0;
    virtual IRenderer::CellStyle cellStyle() const = 0;

protected:
    void onStudyLoaded();
    virtual bool isTSgeneratorOn() const;

protected:
    Data::Study::Ptr study_;
    TimeSeries tsKind_;
};

class blankCell : public Cell
{
public:
    blankCell();
    ~blankCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class inactiveRenewableClusterCell : public Cell
{
public:
    inactiveRenewableClusterCell(wxString toPrintInCell);
    ~inactiveRenewableClusterCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;

private:
    wxString toBePrintedInCell_;
};

class readyMadeTSstatus : public Cell
{
public:
    readyMadeTSstatus(TimeSeries ts);
    ~readyMadeTSstatus() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class generatedTSstatus : public Cell
{
public:
    generatedTSstatus(TimeSeries ts);
    ~generatedTSstatus() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class NumberTsCell : public Cell
{
public:
    NumberTsCell(TimeSeries ts);
    ~NumberTsCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    virtual IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    map<TimeSeries, uint*> tsToNumberTs_;
};

class NumberTsCellThermal : public NumberTsCell
{
public:
    NumberTsCellThermal();
    virtual IRenderer::CellStyle cellStyle() const override;
};

class RefreshTsCell : public Cell
{
public:
    RefreshTsCell(TimeSeries ts);
    ~RefreshTsCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    virtual IRenderer::CellStyle cellStyle() const override;
};

class RefreshTsCellThermal : public RefreshTsCell
{
public:
    RefreshTsCellThermal();
    virtual IRenderer::CellStyle cellStyle() const override;
};

class RefreshSpanCell : public Cell
{
public:
    RefreshSpanCell(TimeSeries ts);
    ~RefreshSpanCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    virtual IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    map<TimeSeries, uint*> tsToRefreshSpan_;
};

class RefreshSpanCellThermal : public RefreshSpanCell
{
public:
    RefreshSpanCellThermal();
    virtual IRenderer::CellStyle cellStyle() const override;
};

class SeasonalCorrelationCell : public Cell
{
public:
    SeasonalCorrelationCell(TimeSeries ts);
    ~SeasonalCorrelationCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    virtual IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    map<TimeSeries, Correlation*> tsToCorrelation_;
};

class SeasonalCorrelationCellThermal : public SeasonalCorrelationCell
{
public:
    SeasonalCorrelationCellThermal();
    virtual IRenderer::CellStyle cellStyle() const override;
};

class storeToInputCell : public Cell
{
public:
    storeToInputCell(TimeSeries ts);
    ~storeToInputCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class storeToOutputCell : public Cell
{
public:
    storeToOutputCell(TimeSeries ts);
    ~storeToOutputCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class intraModalCell : public Cell
{
public:
    intraModalCell(TimeSeries ts);
    ~intraModalCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class interModalCell : public Cell
{
public:
    interModalCell(TimeSeries ts);
    ~interModalCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

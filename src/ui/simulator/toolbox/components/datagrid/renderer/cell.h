
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
protected:
    using study_ptr = Data::Study::Ptr;

public:
    Cell(TimeSeries ts);
    ~Cell();
    virtual wxString cellValue() const = 0;
    virtual double cellNumericValue() const = 0;
    virtual bool cellValue(const String& value) = 0;
    virtual IRenderer::CellStyle cellStyle() const = 0;

protected:
    void onStudyLoaded();
    bool isTSgeneratorOn() const;

protected:
    study_ptr study_;
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
    IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    map<TimeSeries, uint*> tsToNumberTs_;
};

class RefreshTsCell : public Cell
{
public:
    RefreshTsCell(TimeSeries ts);
    ~RefreshTsCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class RefreshSpanCell : public Cell
{
public:
    RefreshSpanCell(TimeSeries ts);
    ~RefreshSpanCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    map<TimeSeries, uint*> tsToRefreshSpan_;
};

class SeasonalCorrelationCell : public Cell
{
public:
    SeasonalCorrelationCell(TimeSeries ts);
    ~SeasonalCorrelationCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;

private:
    void onStudyLoaded();

private:
    map<TimeSeries, Correlation*> tsToCorrelation_;
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

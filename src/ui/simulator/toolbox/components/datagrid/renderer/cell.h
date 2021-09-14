
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

class cell : public Yuni::IEventObserver<cell>
{
protected:
    using study_ptr = Data::Study::Ptr;
public:
    cell(TimeSeries ts);
    ~cell();
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

class blankCell : public cell
{
public:
    blankCell();
    ~blankCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};


class statusCell : public cell
{
public:
    statusCell(TimeSeries ts);
    ~statusCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class NumberTsCell : public cell
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

class RefreshTsCell : public cell
{
public:
    RefreshTsCell(TimeSeries ts);
    ~RefreshTsCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class RefreshSpanCell : public cell
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

class SeasonalCorrelationCell : public cell
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

class storeToInputCell : public cell
{
public:
    storeToInputCell(TimeSeries ts);
    ~storeToInputCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class storeToOutputCell : public cell
{
public:
    storeToOutputCell(TimeSeries ts);
    ~storeToOutputCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class intraModalCell : public cell
{
public:
    intraModalCell(TimeSeries ts);
    ~intraModalCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

class interModalCell : public cell
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
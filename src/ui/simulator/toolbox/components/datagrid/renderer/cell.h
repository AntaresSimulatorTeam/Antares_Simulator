
#pragma once

#include <wx/string.h>
#include "../renderer.h"
// #include <yuni/core/event.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

using namespace Antares::Data;

class cell
{
protected:
    using study_ptr = Data::Study::Ptr;
public:
    cell(study_ptr study, TimeSeries ts);
    ~cell() = default;
    virtual wxString cellValue() const = 0;
    virtual double cellNumericValue() const = 0;
    virtual bool cellValue(double value) = 0;
    virtual IRenderer::CellStyle cellStyle() const = 0;
protected:
    study_ptr study_;
    TimeSeries tsKind_;
    bool tsGenerator_;
};

class blankCell : public cell
{
public:
    blankCell();
    ~blankCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(double value) override;
    IRenderer::CellStyle cellStyle() const override;
};


class statusCell : public cell
{
public:
    statusCell(study_ptr study, TimeSeries ts);
    ~statusCell() = default;
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(double value) override;
    IRenderer::CellStyle cellStyle() const override;

// private:
//     TimeSeries tsKind_;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
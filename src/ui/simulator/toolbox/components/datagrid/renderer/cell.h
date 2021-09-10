

#pragma once

#include <wx/string.h>
// #include <yuni/core/system/stdint.h>
// #include <yuni/core/fwd.h>
#include <yuni/core/event.h>
#include "../renderer.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

class cell
{
public:
    cell();
    ~cell();
    virtual wxString cellValue() const = 0;
    virtual double cellNumericValue() const = 0;
    virtual bool cellValue(const Yuni::String& value) = 0;
    virtual IRenderer::CellStyle cellStyle() const = 0;
};

class blankCell : public cell
{
public:
    blankCell();
    ~blankCell();
    wxString cellValue() const override;
    double cellNumericValue() const override;
    bool cellValue(const Yuni::String& value) override;
    IRenderer::CellStyle cellStyle() const override;
};

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
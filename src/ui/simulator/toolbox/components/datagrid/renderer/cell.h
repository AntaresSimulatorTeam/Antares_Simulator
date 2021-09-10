

#pragma once


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
    blankCell();
    ~blankCell();
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares


#include "cell.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

// -------------------
// Base cell class
// -------------------
cell::cell() {}
cell::~cell() {}


// -------------------
// Blank cell
// -------------------
blankCell::blankCell() : cell()
{}

blankCell::~blankCell()
{}

wxString blankCell::cellValue() const
{
	return wxEmptyString;
}
double blankCell::cellNumericValue() const
{
	return 0.;
}
bool blankCell::cellValue(const Yuni::String& value)
{
	return false;
}
IRenderer::CellStyle blankCell::cellStyle() const
{
	return IRenderer::cellStyleDefaultDisabled;
}


} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares


#include "cell.h"
#include <yuni/core/math.h>
#include "application/study.h"

using namespace Yuni;

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
cell::cell(TimeSeries ts) : tsKind_(ts)
{
    // We have to wait for the study to be loaded before initializing data members from study.
    // That's why constructor delegates to member function onStudyLoaded.
    OnStudyLoaded.connect(this, &cell::onStudyLoaded);
}

cell::~cell()
{
    destroyBoundEvents();
}

void cell::onStudyLoaded()
{
    // We are sure study is loaded now.
    study_ = Data::Study::Current::Get();
    tsGenerator_ = (0 != (study_->parameters.timeSeriesToGenerate & tsKind_));
}


// ===================
// Blank cell
// ===================
blankCell::blankCell() : cell(timeSeriesCount /*arbitrary, not used here anyway */) {}
wxString blankCell::cellValue() const { return wxEmptyString; }
double blankCell::cellNumericValue() const { return 0.; }
bool blankCell::cellValue(double value) { return false;}
IRenderer::CellStyle blankCell::cellStyle() const { return IRenderer::cellStyleDefaultDisabled; }

// ===================
// Status cell
// ===================
statusCell::statusCell(TimeSeries ts) : cell(ts) {}
wxString statusCell::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? wxT("Off") : wxT("On");
}
double statusCell::cellNumericValue() const
{ 
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? 0 : 1.;
}
bool statusCell::cellValue(double value)
{ 
    if (not Math::Zero(value))
        study_->parameters.timeSeriesToGenerate &= ~tsKind_;
    else
        study_->parameters.timeSeriesToGenerate |= tsKind_;
    return true;
}
IRenderer::CellStyle statusCell::cellStyle() const
{ 
    // Status READY made TS
    return tsGenerator_ ? IRenderer::cellStyleConstraintNoWeight
        : IRenderer::cellStyleConstraintWeight;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
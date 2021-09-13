

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
cell::cell(study_ptr study, TimeSeries ts) : study_(study), tsKind_(ts)
{
    OnStudyLoaded.connect(this, &cell::onStudyLoaded);
    
    if (not study_)
        return;
    tsGenerator_ = (0 != (study_->parameters.timeSeriesToGenerate & tsKind_));
}

cell::~cell()
{
    destroyBoundEvents();
}

void cell::onStudyLoaded()
{
    study_ = Data::Study::Current::Get();
    tsGenerator_ = (0 != (study_->parameters.timeSeriesToGenerate & tsKind_));
}


// ===================
// Blank cell
// ===================
blankCell::blankCell() : cell(nullptr, timeSeriesCount /*arbitrary, not used here anyway */) {}
wxString blankCell::cellValue() const { return wxEmptyString; }
double blankCell::cellNumericValue() const { return 0.; }
bool blankCell::cellValue(double value) { return false;}
IRenderer::CellStyle blankCell::cellStyle() const { return IRenderer::cellStyleDefaultDisabled; }

// ===================
// Status cell
// ===================
statusCell::statusCell(study_ptr study, TimeSeries ts) : cell(study, ts) {}
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


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
}

bool cell::isTSgeneratorOn() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_));
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
    return isTSgeneratorOn() ? IRenderer::cellStyleConstraintNoWeight : IRenderer::cellStyleConstraintWeight;
}

// ===================
// Number TS cell
// ===================
NumberTsCell::NumberTsCell(TimeSeries ts) : cell(ts)
{
    OnStudyLoaded.connect(this, &NumberTsCell::onStudyLoaded);
}

void NumberTsCell::onStudyLoaded()
{
    tsToNumberTs_[timeSeriesLoad] = &(study_->parameters.nbTimeSeriesLoad);
    tsToNumberTs_[timeSeriesThermal] = &(study_->parameters.nbTimeSeriesThermal);
    tsToNumberTs_[timeSeriesHydro] = &(study_->parameters.nbTimeSeriesHydro);
    tsToNumberTs_[timeSeriesWind] = &(study_->parameters.nbTimeSeriesWind);
    tsToNumberTs_[timeSeriesSolar] = &(study_->parameters.nbTimeSeriesSolar);
}

wxString NumberTsCell::cellValue() const
{
    wxString to_return = wxEmptyString;
    if (tsToNumberTs_.find(tsKind_) != tsToNumberTs_.end())
        to_return = wxString() << *(tsToNumberTs_.at(tsKind_));
    return to_return; 
}

double NumberTsCell::cellNumericValue() const
{
    uint to_return = 0.;
    if (tsToNumberTs_.find(tsKind_) != tsToNumberTs_.end())
        to_return = *(tsToNumberTs_.at(tsKind_));
    return to_return;
}

bool NumberTsCell::cellValue(double value)
{
    uint nbTimeSeries = (uint)(Math::Round(value));
    if (not nbTimeSeries)
        nbTimeSeries = 1;
    else
    {
        if (nbTimeSeries > 1000)
        {
            logs.debug() << " Number of timeseries hard limit to 1000";
            nbTimeSeries = 1000;
        }
    }

    bool to_return = false;
    if (tsToNumberTs_.find(tsKind_) != tsToNumberTs_.end())
    {
        *(tsToNumberTs_[tsKind_]) = nbTimeSeries;
        to_return = true;
    }
    return to_return;
}

IRenderer::CellStyle NumberTsCell::cellStyle() const
{
    // default style
    return isTSgeneratorOn() ? IRenderer::cellStyleDefault : IRenderer::cellStyleDefaultDisabled;
}


} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
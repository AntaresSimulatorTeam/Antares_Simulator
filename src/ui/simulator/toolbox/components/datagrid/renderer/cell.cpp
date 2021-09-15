

#include "cell.h"
#include <yuni/core/math.h>
#include "application/study.h"

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{

bool convertToDouble(const String& value, double& valueDouble)
{
    bool conversionValid = value.to(valueDouble);
    if (not conversionValid)
    {
        bool b;
        if (value.to(b))
        {
            conversionValid = true;
            valueDouble = (b) ? 1. : 0.;
        }
    }
    return conversionValid;
}

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
bool blankCell::cellValue(const String& value) { return false;}
IRenderer::CellStyle blankCell::cellStyle() const { return IRenderer::cellStyleDefaultDisabled; }

// =================================
// Inactive renewable cluster cell
// =================================
inactiveRenewableClusterCell::inactiveRenewableClusterCell(wxString toPrintInCell) : 
    cell(timeSeriesCount /*arbitrary, not used here anyway */),
    toBePrintedInCell_(toPrintInCell)
{}

wxString inactiveRenewableClusterCell::cellValue() const { return toBePrintedInCell_; }
double inactiveRenewableClusterCell::cellNumericValue() const { return 0.; }
bool inactiveRenewableClusterCell::cellValue(const String& value) { return false; }
IRenderer::CellStyle inactiveRenewableClusterCell::cellStyle() const { return IRenderer::cellStyleDisabled; }

// ===================
// Status cell
// ===================
readyMadeTSstatus::readyMadeTSstatus(TimeSeries ts) : cell(ts) {}

wxString readyMadeTSstatus::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? wxT("Off") : wxT("On");
}
double readyMadeTSstatus::cellNumericValue() const
{ 
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? 0 : 1.;
}
bool readyMadeTSstatus::cellValue(const String& value)
{ 
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    if (not Math::Zero(valueDouble))
        study_->parameters.timeSeriesToGenerate &= ~tsKind_;
    else
        study_->parameters.timeSeriesToGenerate |= tsKind_;
    return true;
}
IRenderer::CellStyle readyMadeTSstatus::cellStyle() const
{ 
    // Status READY made TS
    return isTSgeneratorOn() ? IRenderer::cellStyleConstraintNoWeight : IRenderer::cellStyleConstraintWeight;
}


// ==========================
// Generated TS status cell
// ==========================
generatedTSstatus::generatedTSstatus(TimeSeries ts) : cell(ts) {}

wxString generatedTSstatus::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? wxT("On") : wxT("Off");
}
double generatedTSstatus::cellNumericValue() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? 1. : 0.;
}
bool generatedTSstatus::cellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    if (Math::Zero(valueDouble))
        study_->parameters.timeSeriesToGenerate &= ~tsKind_;
    else
        study_->parameters.timeSeriesToGenerate |= tsKind_;
    return true;
}
IRenderer::CellStyle generatedTSstatus::cellStyle() const
{
    return isTSgeneratorOn() ? IRenderer::cellStyleConstraintWeight : IRenderer::cellStyleConstraintNoWeight;
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

bool NumberTsCell::cellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    uint nbTimeSeries = (uint)(Math::Round(valueDouble));
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

// ===================
// Refresh TS cell
// ===================
RefreshTsCell::RefreshTsCell(TimeSeries ts) : cell(ts)
{
    OnStudyLoaded.connect(this, &RefreshTsCell::onStudyLoaded);
}

wxString RefreshTsCell::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToRefresh & tsKind_)) ? wxT("Yes") : wxT("No");
}

double RefreshTsCell::cellNumericValue() const
{
    return (0 != (study_->parameters.timeSeriesToRefresh & tsKind_)) ? 1. : 0.;
}

bool RefreshTsCell::cellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    if (Math::Zero(valueDouble))
        study_->parameters.timeSeriesToRefresh &= ~tsKind_;
    else
        study_->parameters.timeSeriesToRefresh |= tsKind_;
    return true;
}

IRenderer::CellStyle RefreshTsCell::cellStyle() const
{
    // default style
    return isTSgeneratorOn() ? IRenderer::cellStyleDefault : IRenderer::cellStyleDefaultDisabled;
}

// ===================
// Refresh Span cell
// ===================
RefreshSpanCell::RefreshSpanCell(TimeSeries ts) : cell(ts)
{
    OnStudyLoaded.connect(this, &RefreshSpanCell::onStudyLoaded);
}

void RefreshSpanCell::onStudyLoaded()
{
    tsToRefreshSpan_[timeSeriesLoad] = &(study_->parameters.refreshIntervalLoad);
    tsToRefreshSpan_[timeSeriesThermal] = &(study_->parameters.refreshIntervalThermal);
    tsToRefreshSpan_[timeSeriesHydro] = &(study_->parameters.refreshIntervalHydro);
    tsToRefreshSpan_[timeSeriesWind] = &(study_->parameters.refreshIntervalWind);
    tsToRefreshSpan_[timeSeriesSolar] = &(study_->parameters.refreshIntervalSolar);
}

wxString RefreshSpanCell::cellValue() const
{
    wxString to_return = wxEmptyString;
    if (tsToRefreshSpan_.find(tsKind_) != tsToRefreshSpan_.end())
        to_return = wxString() << *(tsToRefreshSpan_.at(tsKind_));
    return to_return;
}

double RefreshSpanCell::cellNumericValue() const
{
    uint to_return = 0.;
    if (tsToRefreshSpan_.find(tsKind_) != tsToRefreshSpan_.end())
        to_return = *(tsToRefreshSpan_.at(tsKind_));
    return to_return;
}

bool RefreshSpanCell::cellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;
    
    uint refreshSpan = std::max((int)std::round(valueDouble), 1);

    bool to_return = false;
    if (tsToRefreshSpan_.find(tsKind_) != tsToRefreshSpan_.end())
    {
        *(tsToRefreshSpan_[tsKind_]) = refreshSpan;
        to_return = true;
    }
    return to_return;
}

IRenderer::CellStyle RefreshSpanCell::cellStyle() const
{
    return (isTSgeneratorOn() && 0 != (study_->parameters.timeSeriesToRefresh & tsKind_))
        ? IRenderer::cellStyleDefault
        : IRenderer::cellStyleDefaultDisabled;
}

// ============================
//  Seasonal correlation cell
// ============================
SeasonalCorrelationCell::SeasonalCorrelationCell(TimeSeries ts) : cell(ts)
{
    OnStudyLoaded.connect(this, &SeasonalCorrelationCell::onStudyLoaded);
}

void SeasonalCorrelationCell::onStudyLoaded()
{
    tsToCorrelation_[timeSeriesLoad] = &(study_->preproLoadCorrelation);
    tsToCorrelation_[timeSeriesWind] = &(study_->preproWindCorrelation);
    tsToCorrelation_[timeSeriesSolar] = &(study_->preproSolarCorrelation);
}

wxString SeasonalCorrelationCell::cellValue() const
{
    Data::Correlation::Mode mode = Data::Correlation::modeNone;
    if (tsToCorrelation_.find(tsKind_) != tsToCorrelation_.end())
        mode = tsToCorrelation_.at(tsKind_)->mode();
    else if (tsKind_ == Data::timeSeriesHydro)
        return wxT("annual");
    else if (tsKind_ == Data::timeSeriesThermal)
        return wxT("n/a");
    else
        return wxT("--");
    return (mode == Data::Correlation::modeAnnual) ? wxT("annual") : wxT("monthly");
}

double SeasonalCorrelationCell::cellNumericValue() const
{
    Data::Correlation::Mode mode = Data::Correlation::modeNone;
    if (tsToCorrelation_.find(tsKind_) != tsToCorrelation_.end())
        mode = tsToCorrelation_.at(tsKind_)->mode();
    else
        return 0.;
    return (mode == Data::Correlation::modeAnnual) ? 1. : -1.;
}

bool SeasonalCorrelationCell::cellValue(const String& value)
{
    double valueDouble;
    bool convertToDoubleValid = convertToDouble(value, valueDouble);

    Antares::Data::Correlation::Mode mode = Data::Correlation::modeNone;
    CString<64, false> s = value;
    s.trim(" \t");
    s.toLower();
    if ((convertToDoubleValid && Math::Equals(valueDouble, +1.)) || s == "annual" || s == "a")
        mode = Data::Correlation::modeAnnual;
    else
    {
        if ((convertToDoubleValid && Math::Equals(valueDouble, -1.)) || s == "monthly" || s == "month" || s == "m")
            mode = Data::Correlation::modeMonthly;
    }

    if (mode != Antares::Data::Correlation::modeNone)
    {
        if (tsToCorrelation_.find(tsKind_) != tsToCorrelation_.end())
            tsToCorrelation_.at(tsKind_)->mode(mode);
        return true;
    }

    return false;
}

IRenderer::CellStyle SeasonalCorrelationCell::cellStyle() const
{
    return (isTSgeneratorOn()) ? IRenderer::cellStyleDefault : IRenderer::cellStyleDefaultDisabled;
}

// =====================
// Store to input cell
// =====================
storeToInputCell::storeToInputCell(TimeSeries ts) : cell(ts) {}

wxString storeToInputCell::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToImport & tsKind_)) ? wxT("Yes") : wxT("No");
}

double storeToInputCell::cellNumericValue() const
{
    return (0 != (study_->parameters.timeSeriesToImport & tsKind_)) ? 1. : 0.;
}

bool storeToInputCell::cellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    if (Math::Zero(valueDouble))
        study_->parameters.timeSeriesToImport &= ~tsKind_;
    else
        study_->parameters.timeSeriesToImport |= tsKind_;
    return true;
}

IRenderer::CellStyle storeToInputCell::cellStyle() const
{
    return (isTSgeneratorOn() && 0 != (study_->parameters.timeSeriesToImport & tsKind_))
        ? IRenderer::cellStyleDefault
        : IRenderer::cellStyleDefaultDisabled;
}

// ======================
// Store to output cell
// ======================
storeToOutputCell::storeToOutputCell(TimeSeries ts) : cell(ts) {}

wxString storeToOutputCell::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToArchive & tsKind_)) ? wxT("Yes") : wxT("No");
}

double storeToOutputCell::cellNumericValue() const
{
    return (0 != (study_->parameters.timeSeriesToArchive & tsKind_)) ? 1. : 0.;
}

bool storeToOutputCell::cellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    if (Math::Zero(valueDouble))
        study_->parameters.timeSeriesToArchive &= ~tsKind_;
    else
        study_->parameters.timeSeriesToArchive |= tsKind_;
    return true;
}

IRenderer::CellStyle storeToOutputCell::cellStyle() const
{
    return (isTSgeneratorOn() && 0 != (study_->parameters.timeSeriesToArchive & tsKind_))
        ? IRenderer::cellStyleDefault
        : IRenderer::cellStyleDefaultDisabled;
}

// ======================
// Intra modal cell
// ======================
intraModalCell::intraModalCell(TimeSeries ts) : cell(ts) {}

wxString intraModalCell::cellValue() const
{
    return (0 != (study_->parameters.intraModal & tsKind_)) ? wxT("Yes") : wxT("No");
}

double intraModalCell::cellNumericValue() const
{
    return (0 != (study_->parameters.intraModal & tsKind_)) ? 1. : 0.;
}

bool intraModalCell::cellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    if (Math::Zero(valueDouble))
        study_->parameters.intraModal &= ~tsKind_;
    else
        study_->parameters.intraModal |= tsKind_;
    return true;
}

IRenderer::CellStyle intraModalCell::cellStyle() const
{
    return (0 != (study_->parameters.intraModal & tsKind_)) ? IRenderer::cellStyleDefault : IRenderer::cellStyleDefaultDisabled;
}

// ======================
// Inter modal cell
// ======================
interModalCell::interModalCell(TimeSeries ts) : cell(ts) {}

wxString interModalCell::cellValue() const
{
    return (0 != (study_->parameters.interModal & tsKind_)) ? wxT("Yes") : wxT("No");
}

double interModalCell::cellNumericValue() const
{
    return (0 != (study_->parameters.interModal & tsKind_)) ? 1. : 0.;
}

bool interModalCell::cellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    if (Math::Zero(valueDouble))
        study_->parameters.interModal &= ~tsKind_;
    else
        study_->parameters.interModal |= tsKind_;
    return true;
}

IRenderer::CellStyle interModalCell::cellStyle() const
{
    return (0 != (study_->parameters.interModal & tsKind_)) ? IRenderer::cellStyleDefault : IRenderer::cellStyleDefaultDisabled;
}


} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
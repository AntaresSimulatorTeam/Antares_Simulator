/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/


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
static bool convertToDouble(const String& value, double& valueDouble)
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
// Base Cell class
// -------------------
Cell::Cell(TimeSeriesType ts) : tsKind_(ts)
{
    // We have to wait for the study to be loaded before initializing data members from study.
    // That's why constructor delegates to member function onStudyLoaded.
    OnStudyLoaded.connect(this, &Cell::onStudyLoaded);
}

Cell::~Cell()
{
    destroyBoundEvents();
}

void Cell::onStudyLoaded()
{
    // We are sure study is loaded now.
    study_ = GetCurrentStudy();
}

bool Cell::isTSgeneratorOn() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_));
}

// ===================
// Blank cell
// ===================
blankCell::blankCell() : Cell(timeSeriesCount /*arbitrary, not used here anyway */)
{
}
wxString blankCell::cellValue() const
{
    return wxEmptyString;
}
double blankCell::cellNumericValue() const
{
    return 0.;
}
bool blankCell::setCellValue(const String& /* value */)
{
    return false;
}
IRenderer::CellStyle blankCell::cellStyle() const
{
    return IRenderer::cellStyleDefaultDisabled;
}

// ========================
// Inactive cell
// ========================
inactiveCell::inactiveCell(wxString toPrintInCell) :
 Cell(timeSeriesCount /*arbitrary, not used here anyway */), toBePrintedInCell_(toPrintInCell)
{
}

wxString inactiveCell::cellValue() const
{
    return toBePrintedInCell_;
}
double inactiveCell::cellNumericValue() const
{
    return 0.;
}
bool inactiveCell::setCellValue(const String& /* value */)
{
    return false;
}
IRenderer::CellStyle inactiveCell::cellStyle() const
{
    return IRenderer::cellStyleDisabled;
}

// ===================
// Status cell
// ===================
readyMadeTSstatus::readyMadeTSstatus(TimeSeriesType ts) : Cell(ts)
{
}

wxString readyMadeTSstatus::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? wxT("Off") : wxT("On");
}
double readyMadeTSstatus::cellNumericValue() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? 0 : 1.;
}
bool readyMadeTSstatus::setCellValue(const String& value)
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
    return isTSgeneratorOn() ? IRenderer::cellStyleConstraintNoWeight
                             : IRenderer::cellStyleConstraintWeight;
}

// ==========================
// Generated TS status cell
// ==========================
generatedTSstatus::generatedTSstatus(TimeSeriesType ts) : Cell(ts)
{
}

wxString generatedTSstatus::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? wxT("On") : wxT("Off");
}
double generatedTSstatus::cellNumericValue() const
{
    return (0 != (study_->parameters.timeSeriesToGenerate & tsKind_)) ? 1. : 0.;
}
bool generatedTSstatus::setCellValue(const String& value)
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
    return isTSgeneratorOn() ? IRenderer::cellStyleConstraintWeight
                             : IRenderer::cellStyleConstraintNoWeight;
}

// ===================
// Number TS Cell
// ===================
NumberTsCell::NumberTsCell(TimeSeriesType ts) : Cell(ts)
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

bool NumberTsCell::setCellValue(const String& value)
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
RefreshTsCell::RefreshTsCell(TimeSeriesType ts) : Cell(ts)
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

bool RefreshTsCell::setCellValue(const String& value)
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
RefreshSpanCell::RefreshSpanCell(TimeSeriesType ts) : Cell(ts)
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

bool RefreshSpanCell::setCellValue(const String& value)
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
SeasonalCorrelationCell::SeasonalCorrelationCell(TimeSeriesType ts) : Cell(ts)
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

bool SeasonalCorrelationCell::setCellValue(const String& value)
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
        if ((convertToDoubleValid && Math::Equals(valueDouble, -1.)) || s == "monthly"
            || s == "month" || s == "m")
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
storeToInputCell::storeToInputCell(TimeSeriesType ts) : Cell(ts)
{
}

wxString storeToInputCell::cellValue() const
{
    return (0 != (study_->parameters.exportTimeSeriesInInput & tsKind_)) ? wxT("Yes") : wxT("No");
}

double storeToInputCell::cellNumericValue() const
{
    return (0 != (study_->parameters.exportTimeSeriesInInput & tsKind_)) ? 1. : 0.;
}

bool storeToInputCell::setCellValue(const String& value)
{
    double valueDouble;
    if (not convertToDouble(value, valueDouble))
        return false;

    if (Math::Zero(valueDouble))
        study_->parameters.exportTimeSeriesInInput &= ~tsKind_;
    else
        study_->parameters.exportTimeSeriesInInput |= tsKind_;
    return true;
}

IRenderer::CellStyle storeToInputCell::cellStyle() const
{
    // Special case: generation might be forced for some thermal clusters
    if (tsKind_ == timeSeriesThermal)
    {
        return IRenderer::cellStyleDefault;
    }
    return (isTSgeneratorOn() && 0 != (study_->parameters.exportTimeSeriesInInput & tsKind_))
             ? IRenderer::cellStyleDefault
             : IRenderer::cellStyleDefaultDisabled;
}

// ======================
// Store to output cell
// ======================
storeToOutputCell::storeToOutputCell(TimeSeriesType ts) : Cell(ts)
{
}

wxString storeToOutputCell::cellValue() const
{
    return (0 != (study_->parameters.timeSeriesToArchive & tsKind_)) ? wxT("Yes") : wxT("No");
}

double storeToOutputCell::cellNumericValue() const
{
    return (0 != (study_->parameters.timeSeriesToArchive & tsKind_)) ? 1. : 0.;
}

bool storeToOutputCell::setCellValue(const String& value)
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
    // Special case: generation might be forced for some thermal clusters
    if (tsKind_ == timeSeriesThermal)
    {
        return IRenderer::cellStyleDefault;
    }
    return (isTSgeneratorOn() && 0 != (study_->parameters.timeSeriesToArchive & tsKind_))
             ? IRenderer::cellStyleDefault
             : IRenderer::cellStyleDefaultDisabled;
}

// ======================
// Intra modal cell
// ======================
intraModalCell::intraModalCell(TimeSeriesType ts) : Cell(ts)
{
}

wxString intraModalCell::cellValue() const
{
    return (0 != (study_->parameters.intraModal & tsKind_)) ? wxT("Yes") : wxT("No");
}

double intraModalCell::cellNumericValue() const
{
    return (0 != (study_->parameters.intraModal & tsKind_)) ? 1. : 0.;
}

bool intraModalCell::setCellValue(const String& value)
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
    return (0 != (study_->parameters.intraModal & tsKind_)) ? IRenderer::cellStyleDefault
                                                            : IRenderer::cellStyleDefaultDisabled;
}

// ======================
// Inter modal cell
// ======================
interModalCell::interModalCell(TimeSeriesType ts) : Cell(ts)
{
}

wxString interModalCell::cellValue() const
{
    return (0 != (study_->parameters.interModal & tsKind_)) ? wxT("Yes") : wxT("No");
}

double interModalCell::cellNumericValue() const
{
    return (0 != (study_->parameters.interModal & tsKind_)) ? 1. : 0.;
}

bool interModalCell::setCellValue(const String& value)
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
    return (0 != (study_->parameters.interModal & tsKind_)) ? IRenderer::cellStyleDefault
                                                            : IRenderer::cellStyleDefaultDisabled;
}

// ======================
// Thermal-specific cells
// ======================
// Constructors
NumberTsCellThermal::NumberTsCellThermal() : NumberTsCell(timeSeriesThermal)
{
}
RefreshTsCellThermal::RefreshTsCellThermal() : RefreshTsCell(timeSeriesThermal)
{
}
RefreshSpanCellThermal::RefreshSpanCellThermal() : RefreshSpanCell(timeSeriesThermal)
{
}
SeasonalCorrelationCellThermal::SeasonalCorrelationCellThermal() :
 SeasonalCorrelationCell(timeSeriesThermal)
{
}

// Style
IRenderer::CellStyle NumberTsCellThermal::cellStyle() const
{
    // default style
    return IRenderer::cellStyleDefault;
}

IRenderer::CellStyle RefreshTsCellThermal::cellStyle() const
{
    // default style
    return IRenderer::cellStyleDefault;
}

IRenderer::CellStyle RefreshSpanCellThermal::cellStyle() const
{
    // Special case: generation might be forced for some thermal clusters
    return IRenderer::cellStyleDefault;
}

IRenderer::CellStyle SeasonalCorrelationCellThermal::cellStyle() const
{
    return IRenderer::cellStyleDefaultDisabled;
}

// Value
wxString SeasonalCorrelationCellThermal::cellValue() const
{
    return wxT("n/a");
}
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

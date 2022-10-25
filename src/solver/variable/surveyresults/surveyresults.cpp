/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <algorithm>
#include <yuni/yuni.h>
#include <antares/study/study.h>
#include <antares/study/memory-usage.h>
#include "surveyresults.h"
#include <antares/logs.h>
#include <yuni/io/file.h>
#include <antares/io/file.h>

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

namespace Antares
{
namespace Solver
{
namespace Variable
{
namespace Private
{
void InternalExportDigestLinksMatrix(const Data::Study& study,
                                     const char* title,
                                     std::string& buffer,
                                     const Matrix<>& matrix)
{
    // THIS FILE IS DEPRECATED !!!
    buffer.append("\n\n\n\n\t").append(title).append("\n\t\tFrom...\n\t...To");

    for (uint y = 0; y != study.areas.size(); ++y)
    {
        buffer.append("\t").append(study.areas.byIndex[y]->id.c_str());
    }
    buffer.append("\n");

    char conversionBuffer[256];
    conversionBuffer[0] = '\t';
    int sizePrintf;

    uint count = study.areas.size();
    buffer.reserve(10 + count * (1 /*tab*/ + 7));

    double v;
    for (uint y = 0; y != count; ++y)
    {
        buffer.append("\t").append(study.areas.byIndex[y]->id.c_str());
        for (uint x = 0; x < count; ++x)
        {
            if (x == y)
            {
                buffer.append("\tX");
            }
            else
            {
                v = matrix[x][y];
                if (Math::NaN(v))
                    buffer.append("\t--");
                else
                {
                    if (Math::Zero(v))
                    {
                        buffer.append("\t0");
                    }
                    else
                    {
// The snprintf routine is required since we may not have the ending zero
// with the standard printf. The conversion may require a bigger buffer.
#ifdef YUNI_OS_MSVC
                        sizePrintf = ::sprintf_s(
                          conversionBuffer + 1, sizeof(conversionBuffer) - 2, "%.0f", v);
#else
                        sizePrintf = ::snprintf(
                          conversionBuffer + 1, sizeof(conversionBuffer) - 2, "%.0f", v);
#endif
                        if (sizePrintf >= 0)
                            buffer.append((const char*)conversionBuffer);
                        else
                            buffer.append("\tERR");
                    }
                }
            }
        }
        buffer.append("\n");
    }
}

static void ExportGridInfosAreas(const Data::Study& study,
                                 const Yuni::String& originalOutput,
                                 IResultWriter::Ptr writer)
{
    Clob out;
    Clob outLinks;
    Clob outThermal;

    out << "id\tname\n";
    outLinks << "upstream\tdownstream\n";
    outThermal << "area id\tid\tname\tgroup\tunit count\tnominal capacity\t"
                  "min stable power\tmin up/down time\tspinning\tco2\t"
                  "marginal cost\tfixed cost\tstartup cost\tmarket bid cost\tspread cost\n";

    study.areas.each([&](const Data::Area& area) {
        out << area.id << '\t';
        out << area.name << '\n';

        // Areas
        {
            auto end = area.links.end();
            for (auto i = area.links.begin(); i != end; ++i)
                outLinks << area.id << '\t' << i->second->with->id << '\n';
        }

        // Thermal clusters
        for (uint i = 0; i != area.thermal.clusterCount(); ++i)
        {
            assert(NULL != area.thermal.clusters[i]);
            auto& cluster = *(area.thermal.clusters[i]);

            outThermal << area.id << '\t';
            outThermal << cluster.id() << '\t';
            outThermal << cluster.name() << '\t';
            outThermal << Data::ThermalCluster::GroupName(cluster.groupID) << '\t';
            outThermal << cluster.unitCount << '\t';
            outThermal << cluster.nominalCapacity << '\t';
            outThermal << cluster.minStablePower << '\t';
            outThermal << cluster.minUpTime << '\t';
            outThermal << cluster.minDownTime << '\t';
            outThermal << cluster.spinning << '\t';
            outThermal << cluster.co2 << '\t';
            outThermal << cluster.marginalCost << '\t';
            outThermal << cluster.fixedCost << '\t';
            outThermal << cluster.startupCost << '\t';
            outThermal << cluster.marketBidCost << '\t';
            outThermal << cluster.spreadCost << '\n';

        } // each thermal cluster
    });   // each area
    auto add = [&writer, &originalOutput](const YString& filename, Clob&& buffer) {
        YString path;
        path << originalOutput << SEP << "grid" << SEP << filename;
        writer->addEntryFromBuffer(path.c_str(), buffer);
    };

    add("areas.txt", std::move(out));
    add("links.txt", std::move(outLinks));
    add("thermal.txt", std::move(outThermal));
}

SurveyResultsData::SurveyResultsData(const Data::Study& s, const String& o) :
 columnIndex((uint)-1),
 thermalCluster(nullptr),
 area(nullptr),
 link(nullptr),
 setOfAreasIndex((uint)-1),
 study(s),
 nbYears(s.parameters.nbYears),
 effectiveNbYears(s.parameters.effectiveNbYears),
 originalOutput(o)
{
}

void SurveyResultsData::initialize(uint maxVariables)
{
    filename.reserve(1024);
    output.reserve(1024);
    fileBuffer.reserve(4096);

    rowIndex = (uint)-1; // invalid index

    logs.debug() << "  :: initializing survey results (max: " << maxVariables << " variables)";

    switch (study.parameters.mode)
    {
    case Data::stdmEconomy:
    {
        matrix.resize(study.areas.size(), study.areas.size());
        // The initialization will be done later
        //::MatrixFill(&matrix, std::numeric_limits<double>::quiet_NaN());
        break;
    }
    case Data::stdmAdequacy:
    {
        matrix.resize(study.areas.size(), study.areas.size());
        // The initialization will be done later
        //::MatrixFill(&matrix, std::numeric_limits<double>::quiet_NaN());
        break;
    }
    case Data::stdmAdequacyDraft:
    {
        matrix.resize(maxVariables, study.runtime->rangeLimits.year[Data::rangeCount]);
        break;
    }
    case Data::stdmUnknown:
    case Data::stdmExpansion:
    case Data::stdmMax:
        break;
    }
}

void SurveyResultsData::exportGridInfos(IResultWriter::Ptr writer)
{
    output.clear();
    Solver::Variable::Private::ExportGridInfosAreas(study, originalOutput, writer);
}
} // namespace Private
} // namespace Variable
} // namespace Solver
} // namespace Antares

namespace Antares
{
namespace Solver
{
namespace Variable
{
static inline uint GetRangeLimit(const Data::Study& study,
                                 int fileLevel,
                                 int precisionLevel,
                                 int index)
{
    assert(study.runtime && "invalid runtime data");
    switch (precisionLevel)
    {
    case Category::hourly:
        return study.runtime->rangeLimits.hour[index];
    case Category::daily:
        return study.runtime->rangeLimits.day[index];
    case Category::weekly:
        return study.runtime->rangeLimits.week[index];
    case Category::monthly:
        return study.runtime->rangeLimits.month[index];
    case Category::annual:
    {
        if (fileLevel & Category::mc)
            return study.runtime->rangeLimits.year[index];
        return 0;
    }
    default:
        return 0;
    }
}

// inline : only used in this cpp file
inline void SurveyResults::writeDateToFileDescriptor(uint row, int fileLevel, int precisionLevel)
{
    auto& out = data.fileBuffer;
    auto& calendar = data.study.calendarOutput; // data.study.calendar;

    switch (precisionLevel)
    {
    case Category::hourly:
    {
        assert(row - 1 < calendar.maxHoursInYear);
        auto& hourinfo = calendar.hours[row - 1];
        out << '\t' << row << '\t';
        uint d = 1 + hourinfo.dayMonth; // 1 + h.day;
        if (d < 10)
            out << '0' << d;
        else
            out << d;
        out << '\t';
        out << calendar.text.months[hourinfo.month].shortUpperName << '\t';
        if (hourinfo.dayHour < 10)
            out << '0' << (uint)hourinfo.dayHour;
        else
            out << (uint)hourinfo.dayHour;
        out.append(":00", 3);
        break;
    }
    case Category::daily:
    {
        assert(row - 1 < calendar.maxDaysInYear);
        auto& dayinfo = calendar.days[row - 1];
        out << '\t';
        out << row;
        out << '\t';
        uint dp1 = 1 + dayinfo.dayMonth;
        if (dp1 < 10)
            out << '0' << dp1;
        else
            out << dp1;
        out << '\t' << calendar.text.months[dayinfo.month].shortUpperName;
        break;
    }
    case Category::weekly:
    {
        out << '\t';
        if (row - 1 < calendar.maxWeeksInYear)
            out << calendar.weeks[row - 1].userweek;
        else
            out << '?';
        break;
    }
    case Category::monthly:
    {
        assert(row - 1 < 12);
        if (row - 1 < 12)
            out << '\t' << row << '\t' << calendar.text.months[row - 1].shortUpperName;
        else
            out.append("\t\t", 2);
        break;
    }
    case Category::annual:
    {
        if (fileLevel & Category::mc)
            out << '\t' << row;
        else
            out.append("\tAnnual", 7);
        break;
    }
    default:
        out << '\t' << row;
        break;
    }
}

template<class StringT, class ConvertT, class PrecisionT>
inline void SurveyResults::AppendDoubleValue(uint& error,
                                             double v,
                                             StringT& buffer,
                                             ConvertT& conversionBuffer,
                                             const PrecisionT& precision,
                                             const bool isNotApplicable)
{
    if (isNotApplicable)
    {
        buffer.append("\tN/A", 4);
        return;
    }

    if (not Math::Zero(v))
    {
        if (Math::NaN(v))
        {
            buffer.append("\tNaN", 4);
            if (++error == 1)
            {
                // We should disabled errors on NaN if the quadratic optimization has failed
                if (not data.study.runtime->quadraticOptimizationHasFailed)
                    logs.error() << "'NaN' value detected";
            }
        }
        else
        {
            if (Math::Infinite(v))
            {
                buffer.append((v > 0) ? "\t+inf" : "\t-inf", 5);
                if (++error == 1)
                    logs.error() << "'infinite' value detected";
            }
            else
            {
#ifdef YUNI_OS_MSVC
                int sizePrintf = ::sprintf_s(
                  conversionBuffer + 1, sizeof(conversionBuffer) - 2, precision.c_str(), v);
#else
                int sizePrintf = ::snprintf(
                  conversionBuffer + 1, sizeof(conversionBuffer) - 2, precision.c_str(), v);
#endif

                if (sizePrintf >= 0)
                {
                    // +1 is related to the tab character already present in the
                    // conversion buffer
                    buffer.append((const char*)conversionBuffer, 1 + sizePrintf);
                }
                else
                    buffer += "\tERR";
            }
        }
    }
    else
        buffer.append("\t0", 2);
}

/*!
** \brief Write into a file all column names
**
** \param fd A file Descriptor
** \param areaname Name of the current area. This variable is required because we may have
**   a virtual area, like the "whole system", or a group.
** \tparam PrecisionLevel A precision level (hourly, daily, monthly...)
** \tparam Row The row index (zero-based)
*/
template<int Row, class S, class StringT, class CaptionT>
static inline void WriteIndexHeaderToFileDescriptor(int precisionLevel,
                                                    S& s,
                                                    const StringT& areaname,
                                                    const CaptionT& captions,
                                                    uint columnIndex)
{
    switch (precisionLevel)
    {
    case Category::hourly:
    {
        switch (Row)
        {
        case 0:
            s << areaname << "\thourly\t\t\t";
            break;
        case 2:
            s << "\tindex\tday\tmonth\thour";
            break;
        default:
            s << "\t\t\t\t";
        }
        break;
    }
    case Category::daily:
    {
        switch (Row)
        {
        case 0:
            s << areaname << "\tdaily\t\t";
            break;
        case 2:
            s << "\tindex\tday\tmonth";
            break;
        default:
            s << "\t\t\t";
        }
        break;
    }
    case Category::weekly:
    {
        switch (Row)
        {
        case 0:
            s << areaname << "\tweekly";
            break;
        case 2:
            s << "\tweek";
            break;
        default:
            s << '\t';
        }
        break;
    }
    case Category::monthly:
    {
        switch (Row)
        {
        case 0:
            s << areaname << "\tmonthly\t";
            break;
        case 2:
            s << "\tindex\tmonth";
            break;
        default:
            s << "\t\t";
        }
        break;
    }
    case Category::annual:
    {
        switch (Row)
        {
        case 0:
            s << areaname << "\tannual";
            break;
        case 2:
            s << '\t';
            break;
        default:
            s << '\t';
        }
        break;
    }
    default:
        break;
    }

    assert(Row < SurveyResults::captionCount);

    for (uint x = 0; x != columnIndex; ++x)
        s << '\t' << captions[Row][x];
    s += '\n';
}

uint initializeMaxVariables(uint maxVars, const Data::StudyRuntimeInfos* runtime)
{
    if (!runtime)
        return maxVars;

    // Adding new files / variables ? Change the values below to avoid maxVariables being too small

    // TODO: count those variables at compile time / runtime
    // using e.g VCardT::categoryDataLevel
    const uint nbVariablesPerDetailThermalCluster = 4;
    /*
      - Production
      - NODU,
      - NP Costs
      - Net profit
    */
    const uint nbVariablesPerDetailRenewableCluster = 1;
    // Production

    const uint nbVariablesPerInequalityBindingConstraint = 1;
    // Marginal price

    const auto max = [](uint a, uint b, uint c, uint d) { return std::max({a, b, c, d}); };

    return max(maxVars,
               static_cast<uint>(nbVariablesPerDetailThermalCluster
                                 * runtime->maxThermalClustersForSingleArea),
               static_cast<uint>(nbVariablesPerDetailRenewableCluster
                                 * runtime->maxRenewableClustersForSingleArea),
               nbVariablesPerInequalityBindingConstraint
                 * runtime->getNumberOfInequalityBindingConstraints());
}

// TOFIX - MBO 02/06/2014 nombre de colonnes fonction du nombre de variables
SurveyResults::SurveyResults(uint maxVars,
                             const Data::Study& s,
                             const String& o,
                             IResultWriter::Ptr writer) :
 data(s, o),
 maxVariables(initializeMaxVariables(maxVars, s.runtime)),
 yearByYearResults(false),
 isCurrentVarNA(nullptr),
 isPrinted(nullptr),
 pResultWriter(writer)
{
    variableCaption.reserve(10);

    data.initialize(maxVariables);
    // logs.debug() << "  :: survey results: allocating "
    //	<< (uint64)((data.matrix.memoryUsage() + sizeof(values))
    //		+ sizeof(double) * maxHoursInAYear * 3
    //		+ sizeof(PrecisionType) * maxVariables) / 1024
    //	<< " Ko";

    // values
    typedef double* ValueType;
    values = new ValueType[maxVariables];
    for (uint i = 0; i != maxVariables; ++i)
    {
        values[i] = new double[maxHoursInAYear];
        memset(values[i], 0, sizeof(double) * maxHoursInAYear);
    }

    // captions
    for (uint i = 0; i != captionCount; ++i)
        captions[i] = new CaptionType[maxVariables];

    // precision
    precision = new PrecisionType[maxVariables];
    for (uint i = 0; i != maxVariables; ++i)
        precision[i] = PrecisionToPrintfFormat<0>::Value();

    // non applicable status
    nonApplicableStatus = new bool[maxVariables];
    for (uint i = 0; i != maxVariables; ++i)
        nonApplicableStatus[i] = false;

    uint nbAreas = s.areas.size();
    uint nbSetsOfAreas = s.areas.size();
    digestSize = (nbAreas > nbSetsOfAreas) ? nbAreas : nbSetsOfAreas;
    digestNonApplicableStatus = new bool*[digestSize];
    for (uint i = 0; i < digestSize; i++)
    {
        digestNonApplicableStatus[i] = new bool[maxVariables];
        for (uint v = 0; v < maxVariables; v++)
            digestNonApplicableStatus[i][v] = false;
    }
}

SurveyResults::~SurveyResults()
{
    if (values)
    {
        for (uint i = 0; i != maxVariables; ++i)
            delete[] values[i];
        delete[] values;
    }

    for (uint i = 0; i != captionCount; ++i)
        delete[] captions[i];
    delete[] precision;
    delete[] nonApplicableStatus;
    for (uint i = 0; i < digestSize; i++)
        delete[] digestNonApplicableStatus[i];
    delete[] digestNonApplicableStatus;
}

void SurveyResults::resetValuesAtLine(uint j)
{
    for (uint i = 0; i < maxVariables; i++)
        values[i][j] = 0.;
}

void SurveyResults::exportDigestAllYears(std::string& buffer)
{
    // Main Header
    {
        buffer.append("\tdigest\n\tVARIABLES\tAREAS\tLINKS\n")
          .append("\t")
          .append(std::to_string(data.columnIndex))
          .append("\t")
          .append(std::to_string(data.rowCaptions.size()))
          .append("\t0\n\n");
    }
    // Header - All columns
    for (uint rowIndex = 0; rowIndex != captionCount; ++rowIndex)
    {
        buffer.append("\t");
        for (uint i = 0; i != data.columnIndex; ++i)
        {
            assert(i < maxVariables);
            buffer.append("\t").append(captions[rowIndex][i].c_str());
        }
        buffer.append("\n");
    }

    char conversionBuffer[128];
    conversionBuffer[0] = '\t';
    int sizePrintf;

    auto end = data.rowCaptions.end();
    uint y = 0;
    for (auto j = data.rowCaptions.begin(); j != end; ++j, ++y)
    {
        // asserts
        assert(y < maxHoursInAYear);

        buffer.append("\t").append(j->c_str());

        // Loop over results matrix columns
        for (uint i = 0; i != data.columnIndex; ++i)
        {
            assert(i < maxVariables && "i greater can not be greater than maxVariables");
            assert(y < maxHoursInAYear && "y can not be greater than maxHoursInAYear");

            if (digestNonApplicableStatus[y][i])
            {
                buffer.append("\tN/A");
                continue;
            }

            if (Math::Zero(values[i][y]))
            {
                buffer.append("\t0");
            }
            else
            {
// The snprintf routine is required since we may not have the ending zero
// with the standard printf. The conversion may require a bigger buffer.
#ifdef YUNI_OS_MSVC
                sizePrintf = ::sprintf_s(conversionBuffer + 1,
                                         sizeof(conversionBuffer) - 2,
                                         precision[i].c_str(),
                                         values[i][y]);
#else
                sizePrintf = ::snprintf(conversionBuffer + 1,
                                        sizeof(conversionBuffer) - 2,
                                        precision[i].c_str(),
                                        values[i][y]);
#endif
                if (sizePrintf >= 0)
                    buffer.append((const char*)conversionBuffer);
                else
                    buffer.append("\tERR");
            }
        }

        // End of line
        buffer.append("\n");
    }
    buffer.append("\n\n");
}

void SurveyResults::exportDigestMatrix(const char* title, std::string& buffer)
{
    Private::InternalExportDigestLinksMatrix(data.study, title, buffer, data.matrix);
}

void SurveyResults::saveToFile(int dataLevel, int fileLevel, int precisionLevel)
{
    logs.debug() << " :: survey writing `" << data.filename << "`";

    // Clearing the buffer
    data.fileBuffer.clear();
    data.fileBuffer.reserve(2 * 1024 * 1024);

    // How many rows have we got ?
    const uint heightBegin = GetRangeLimit(data.study, fileLevel, precisionLevel, Data::rangeBegin);
    // (+1 for condition stop)
    const uint heightEnd = GetRangeLimit(data.study, fileLevel, precisionLevel, Data::rangeEnd) + 1;

    // Big header
    if (data.area)
        data.fileBuffer << data.area->name;
    else
        data.fileBuffer << "system";

    data.fileBuffer << '\t';
    Category::DataLevelToStream(data.fileBuffer, dataLevel);
    data.fileBuffer << '\t';
    Category::FileLevelToStreamShort(data.fileBuffer, fileLevel);
    data.fileBuffer << '\t';
    Category::PrecisionLevelToStream(data.fileBuffer, precisionLevel);
    data.fileBuffer << '\n';

    if (data.link)
        data.fileBuffer << data.link->with->name;
    data.fileBuffer << "\tVARIABLES\tBEGIN\tEND\n";
    data.fileBuffer << '\t' << data.columnIndex << '\t' << (heightBegin + 1) << '\t' << heightEnd
                    << '\n';

    // Space
    data.fileBuffer += '\n';

    if (data.area)
    {
        auto& areaname = data.area->name;
        WriteIndexHeaderToFileDescriptor<0>(
          precisionLevel, data.fileBuffer, areaname, captions, data.columnIndex);
        WriteIndexHeaderToFileDescriptor<1>(
          precisionLevel, data.fileBuffer, areaname, captions, data.columnIndex);
        WriteIndexHeaderToFileDescriptor<2>(
          precisionLevel, data.fileBuffer, areaname, captions, data.columnIndex);
    }
    else
    {
        WriteIndexHeaderToFileDescriptor<0>(
          precisionLevel, data.fileBuffer, "system", captions, data.columnIndex);
        WriteIndexHeaderToFileDescriptor<1>(
          precisionLevel, data.fileBuffer, "system", captions, data.columnIndex);
        WriteIndexHeaderToFileDescriptor<2>(
          precisionLevel, data.fileBuffer, "system", captions, data.columnIndex);
    }

    char conversionBuffer[256];
    conversionBuffer[0] = '\t';
    uint error = 0;

#ifndef NDEBUG
    // A few preliminary checks
    for (uint x = 0; x != data.columnIndex; ++x)
        assert(not precision[x].empty() && "invalid precision");
#endif

    if (fileLevel & Category::mc)
    {
        // Each row
        for (uint y = heightBegin; y < heightEnd; ++y)
        {
            // Asserts
            assert(y < data.matrix.height);

            // Index
            writeDateToFileDescriptor(y + 1, fileLevel, precisionLevel);

            // Each column
            assert(data.columnIndex <= data.matrix.width);

            for (uint x = 0; x != data.columnIndex; ++x)
                AppendDoubleValue(
                  error, data.matrix[x][y], data.fileBuffer, conversionBuffer, precision[x], false);

            // End of line
            data.fileBuffer += '\n';
        }
    }
    else
    {
        // Each row
        for (uint y = heightBegin; y < heightEnd; ++y)
        {
            // Asserts
            assert(y < maxHoursInAYear);
            // Index
            writeDateToFileDescriptor(y + 1, fileLevel, precisionLevel);

            // Each column
            assert(data.columnIndex <= maxVariables);

            for (uint x = 0; x != data.columnIndex; ++x)
                AppendDoubleValue(error,
                                  values[x][y],
                                  data.fileBuffer,
                                  conversionBuffer,
                                  precision[x],
                                  nonApplicableStatus[x]);

            // End of line
            data.fileBuffer += '\n';
        }
    }

    // mc-ind & mc-all
    pResultWriter->addEntryFromBuffer(data.filename.c_str(), data.fileBuffer);
}

void SurveyResults::EstimateMemoryUsage(uint maxVars, Data::StudyMemoryUsage& u)
{
    if (u.study.parameters.synthesis
        || (u.study.parameters.yearByYear && u.mode != Data::stdmAdequacyDraft))
    {
        // TODO : We may have more thermal cluster for an area than the max total of vars
        //   So we should take into consideration the maximum total of thermal clusters for
        //   a single area
        Yuni::uint64 temporaryMemoryAmount = 0;
        // Base
        temporaryMemoryAmount += sizeof(SurveyResults);
        // values
        temporaryMemoryAmount += sizeof(double) * 8760 * maxVars;
        temporaryMemoryAmount += sizeof(CaptionType) * 3 * maxVars;
        temporaryMemoryAmount += sizeof(PrecisionType) * maxVars;
        // data.fileBuffer
        temporaryMemoryAmount += 4 * 1024 * 1024; // 4 Mib
        // temporary buffers for numeric conversion
        temporaryMemoryAmount += 256;

        // Il y a un thread par année MC et chaque thread construit dynamiquement un objet de type
        // SurveyResults pour ses outputs (voir container.hxx : void
        // List<NextT>::exportSurveyResults(...))
        u.requiredMemoryForOutput += temporaryMemoryAmount * u.nbYearsParallel;
    }
}

void SurveyResults::exportGridInfos()
{
    data.exportGridInfos(pResultWriter);
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

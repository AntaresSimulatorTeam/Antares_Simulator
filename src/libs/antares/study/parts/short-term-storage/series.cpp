// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/short-term-storage/series.h"

#include <fstream>
#include <iomanip>

#include <yuni/io/file.h>

#include <antares/logs/logs.h>
#include "antares/antares/constants.h"

#define SEP Yuni::IO::Separator

namespace Antares::Data::ShortTermStorage
{

namespace fs = std::filesystem;

Series::Series():
    inflows(inflowsTSNumbers)
{
}

bool loadFile(const fs::path& path, std::vector<double>& vect)
{
    logs.debug() << "  :: loading file " << path;

    vect.reserve(HOURS_PER_YEAR);

    std::ifstream file;
    file.open(path);

    if (!file.is_open())
    {
        logs.info() << "Optional file not found: " << path
                    << ", default values will be used if needed";
        return true;
    }

    unsigned int lineCount = 0;
    std::string line;
    try
    {
        while (getline(file, line) && lineCount < HOURS_PER_YEAR)
        {
            double d = std::stod(line);
            vect.push_back(d);
            lineCount++;
        }
        if (lineCount > 0 && lineCount < HOURS_PER_YEAR)
        {
            logs.warning() << "File too small: " << path;
            return false;
        }
    }
    catch (const std::ios_base::failure& ex)
    {
        logs.error() << "Failed reading file: " << path << " (I/O error)";
        return false;
    }
    catch (const std::invalid_argument& ex)
    {
        logs.error() << "Failed reading file: " << path << " conversion to double failed at line "
                     << lineCount + 1 << "  value: " << line;
        return false;
    }
    catch (const std::out_of_range& ex)
    {
        logs.error() << "Failed reading file: " << path << " value is out of bounds at line "
                     << lineCount + 1 << "  value: " << line;
        return false;
    }

    return true;
}

bool loadFile(const std::filesystem::path& file, TimeSeries& series)
{
    logs.debug() << "  :: loading file " << file;
    if (std::filesystem::is_regular_file(file))
    {
        return series.loadFromFile(file, false);
    }
    logs.info() << "Optional file not found: " << file << ", default values will be used if needed";
    return true;
}

bool Series::loadFromFolder(const fs::path& folder, StudyVersion studyVersion)
{
    bool ret = true;

    ret = loadFile(folder / "PMAX-injection.txt", maxInjectionModulation) && ret;
    ret = loadFile(folder / "PMAX-withdrawal.txt", maxWithdrawalModulation) && ret;

    if (auto path = folder / "inflows.txt"; std::filesystem::exists(path))
    {
        ret = inflows.loadFromFile(path, false, Matrix<>::optImmediate) && ret;
    }
    else
    {
        logs.info() << "Optional file not found: " << path
                    << ", default values will be used if needed";
    }

    ret = loadFile(folder / "lower-rule-curve.txt", lowerRuleCurve) && ret;
    ret = loadFile(folder / "upper-rule-curve.txt", upperRuleCurve) && ret;
    if (studyVersion >= StudyVersion(9, 2))
    {
        ret = loadFile(folder / "cost-injection.txt", costInjection) && ret;
        ret = loadFile(folder / "cost-withdrawal.txt", costWithdrawal) && ret;
        ret = loadFile(folder / "cost-level.txt", costLevel) && ret;

        ret = loadFile(folder / "cost-variation-injection.txt", costVariationInjection) && ret;

        ret = loadFile(folder / "cost-variation-withdrawal.txt", costVariationWithdrawal) && ret;
    }

    return ret;
}

void fillIfEmpty(std::vector<double>& v, double value)
{
    if (v.empty())
    {
        v.resize(HOURS_PER_YEAR, value);
    }
}

void fillIfEmpty(TimeSeries& series, double value)
{
    if (series.timeSeries.empty())
    {
        series.reset(1, HOURS_PER_YEAR);
        series.fill(value);
    }
}

void Series::fillDefaultSeriesIfEmpty()
{
    fillIfEmpty(maxInjectionModulation, 1.0);
    fillIfEmpty(maxWithdrawalModulation, 1.0);
    fillIfEmpty(inflows, 0.0);
    fillIfEmpty(lowerRuleCurve, 0.0);
    fillIfEmpty(upperRuleCurve, 1.0);

    fillIfEmpty(costInjection, 0.0);
    fillIfEmpty(costWithdrawal, 0.0);
    fillIfEmpty(costLevel, -1.e-6);

    fillIfEmpty(costVariationInjection, 0.0);

    fillIfEmpty(costVariationWithdrawal, 0.0);
}

bool Series::validate(const std::string& id, StudyVersion studyVersion) const
{
    return validateSizes(id, studyVersion) && validateMaxInjection(id) && validateMaxWithdrawal(id)
           && validateRuleCurves(id);
}

static bool checkVectBetweenZeroOne(const std::string& name,
                                    const std::string& id,
                                    const std::vector<double>& v)
{
    if (!std::all_of(v.begin(), v.end(), [](double d) { return (d >= 0.0 && d <= 1.0); }))
    {
        logs.warning() << "Short-term storage " << id << " Values for " << name
                       << " values should be between 0 and 1";
        return false;
    }
    return true;
}

static bool checkSize(const std::string& seriesFilename,
                      const std::string& id,
                      const std::vector<double>& v)
{
    if (v.size() != HOURS_PER_YEAR)
    {
        logs.warning() << "Short-term storage " << id
                       << " Invalid size for file: " << seriesFilename << ". Got " << v.size()
                       << " lines, expected " << HOURS_PER_YEAR;
        return false;
    }

    return true;
}

static bool checkSize(const std::string& seriesFilename, const std::string& id, const TimeSeries& v)
{
    if (v.timeSeries.height != HOURS_PER_YEAR)
    {
        logs.warning() << "Short-term storage " << id
                       << " Invalid size for file: " << seriesFilename << ". Got "
                       << v.timeSeries.height << " lines, expected " << HOURS_PER_YEAR;
        return false;
    }

    return true;
}

bool Series::validateSizes(const std::string& id, StudyVersion studyVersion) const
{
    bool ret = checkSize("PMAX-injection.txt", id, maxInjectionModulation)
               && checkSize("PMAX-withdrawal.txt", id, maxWithdrawalModulation)
               && checkSize("inflows.txt", id, inflows)
               && checkSize("lower-rule-curve.txt", id, lowerRuleCurve)
               && checkSize("upper-rule-curve.txt", id, upperRuleCurve);
    // Some elements were introduced in version 9.2.0
    if (studyVersion >= StudyVersion(9, 2))
    {
        ret = checkSize("cost-injection.txt", id, costInjection)
              && checkSize("cost-withdrawal.txt", id, costWithdrawal)
              && checkSize("cost-level.txt", id, costLevel)
              && checkSize("cost-variation-injection.txt", id, costVariationInjection)
              && checkSize("cost-variation-withdrawal.txt", id, costVariationWithdrawal) && ret;
    }
    return ret;
}

bool Series::validateMaxInjection(const std::string& id) const
{
    return checkVectBetweenZeroOne("PMAX injection", id, maxInjectionModulation);
}

bool Series::validateMaxWithdrawal(const std::string& id) const
{
    return checkVectBetweenZeroOne("PMAX withdrawal", id, maxWithdrawalModulation);
}

bool Series::validateRuleCurves(const std::string& id) const
{
    if (!validateUpperRuleCurve(id) || !validateLowerRuleCurve(id))
    {
        return false;
    }

    for (unsigned int i = 0; i < HOURS_PER_YEAR; i++)
    {
        if (lowerRuleCurve[i] > upperRuleCurve[i])
        {
            logs.warning() << "Short-term storage " << id
                           << " Lower rule curve greater than upper at line: " << i + 1;
            return false;
        }
    }
    return true;
}

bool Series::validateUpperRuleCurve(const std::string& id) const
{
    return checkVectBetweenZeroOne("upper rule curve", id, upperRuleCurve);
}

bool Series::validateLowerRuleCurve(const std::string& id) const
{
    return checkVectBetweenZeroOne("lower rule curve", id, maxInjectionModulation);
}

} // namespace Antares::Data::ShortTermStorage

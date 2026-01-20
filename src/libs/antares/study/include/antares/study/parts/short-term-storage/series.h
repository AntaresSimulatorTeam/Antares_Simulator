// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <antares/series/series.h>
#include <antares/study/version.h>

namespace Antares::Data::ShortTermStorage
{
class Series
{
public:
    Series();
    // check if series values are valid
    bool validate(const std::string& id, StudyVersion studyVersion) const;

    // load all series files with folder path
    bool loadFromFolder(const std::filesystem::path& folder, StudyVersion studyVersion);
    void fillDefaultSeriesIfEmpty();

    bool saveToFolder(const std::string& folder) const;

    std::vector<double> maxInjectionModulation;
    std::vector<double> maxWithdrawalModulation;
    TimeSeriesNumbers inflowsTSNumbers;
    TimeSeries inflows;
    std::vector<double> lowerRuleCurve;
    std::vector<double> upperRuleCurve;

    std::vector<double> costInjection;
    std::vector<double> costWithdrawal;
    std::vector<double> costLevel;
    std::vector<double> costVariationInjection;
    std::vector<double> costVariationWithdrawal;

private:
    bool validateSizes(const std::string&, StudyVersion studyVersion) const;
    bool validateMaxInjection(const std::string&) const;
    bool validateMaxWithdrawal(const std::string&) const;
    bool validateRuleCurves(const std::string&) const;
    bool validateUpperRuleCurve(const std::string&) const;
    bool validateLowerRuleCurve(const std::string&) const;
};

bool loadFile(const std::filesystem::path& file, TimeSeries& series);
void fillIfEmpty(TimeSeries& series, double value);
} // namespace Antares::Data::ShortTermStorage

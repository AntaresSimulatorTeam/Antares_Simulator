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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Antares::Data::ShortTermStorage
{

class Series
{
public:
    Series() = default;
    // check if series values are valid
    bool validate(const std::string& id = "") const;

    // load all series files with folder path
    bool loadFromFolder(const std::filesystem::path& folder);
    void fillDefaultSeriesIfEmpty();

    bool saveToFolder(const std::string& folder) const;

    std::vector<double> maxInjectionModulation;
    std::vector<double> maxWithdrawalModulation;
    std::vector<double> inflows;
    std::vector<double> lowerRuleCurve;
    std::vector<double> upperRuleCurve;

    std::vector<double> costInjection;
    std::vector<double> costWithdrawal;
    std::vector<double> costLevel;
    std::vector<double> costVariationInjection;
    std::vector<double> costVariationWithdrawal;

private:
    bool validateSizes(const std::string&) const;
    bool validateMaxInjection(const std::string&) const;
    bool validateMaxWithdrawal(const std::string&) const;
    bool validateRuleCurves(const std::string&) const;
    bool validateUpperRuleCurve(const std::string&) const;
    bool validateLowerRuleCurve(const std::string&) const;
};

bool loadFile(const std::filesystem::path& folder, std::vector<double>& vect);
bool writeVectorToFile(const std::string& path, const std::vector<double>& vect);
void fillIfEmpty(std::vector<double>& v, double value);
} // namespace Antares::Data::ShortTermStorage

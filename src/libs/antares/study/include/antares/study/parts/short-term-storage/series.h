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
#include <string>
#include <vector>

namespace Antares::Data::ShortTermStorage
{
class Series
{
public:
    // check if series values are valid
    bool validate() const;

    // load all series files with folder path
    bool loadFromFolder(const std::string& folder);
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

private:
    bool validateSizes() const;
    bool validateMaxInjection() const;
    bool validateMaxWithdrawal() const;
    bool validateRuleCurves() const;
    bool validateUpperRuleCurve() const;
    bool validateLowerRuleCurve() const;
};

bool loadFile(const std::string& folder, std::vector<double>& vect);
bool writeVectorToFile(const std::string& path, const std::vector<double>& vect);

} // namespace Antares::Data::ShortTermStorage

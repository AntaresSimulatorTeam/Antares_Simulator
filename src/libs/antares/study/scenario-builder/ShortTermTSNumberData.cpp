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

#include "antares/study/scenario-builder/ShortTermTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
uint ShortTermTSNumberData::get_tsGenCount(const Study& study) const
{
    return 0;
}

bool ShortTermTSNumberData::apply(Study& study)
{
    bool ret = true;
    auto tsGenMax = get_tsGenCount(study);
    uint errors = 0;
    for (auto& sts: pArea->shortTermStorage.storagesByIndex)
    {
        auto& rule = rules_[sts.id];
        std::string logprefix = "Short term storage: area '" + pArea->name + "', sts: '" + sts.id
                                + "': ";
        ret = ApplyToMatrix(errors, logprefix, sts.series->inflows, rule[0], tsGenMax) && ret;
    }

    return ret;
}

void ShortTermTSNumberData::setTSnumber(const std::string& cluster_id,
                                        unsigned year,
                                        unsigned value)
{
    auto& ts_numbers = rules_[cluster_id];
    if (year < ts_numbers.height)
    {
        ts_numbers[0][year] = value;
    }
}

unsigned ShortTermTSNumberData::get_value(const std::string& cluster_id, unsigned year) const
{
    return rules_.at(cluster_id)[0][year];
}

bool ShortTermTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    for (const auto& sts: pArea->shortTermStorage.storagesByIndex)
    {
        auto& ts_numbers = rules_[sts.id];
        ts_numbers.reset(1, nbYears);
    }

    return true;
}

void ShortTermTSNumberData::saveToINIFile(Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

    if (!pArea)
    {
        return;
    }

    for (const auto& sts: pArea->shortTermStorage.storagesByIndex)
    {
        for (uint year = 0; year < sts.series->inflows.timeseriesNumbers.height(); ++year)
        {
            const uint val = get_value(sts.id, year);

            // Equals to zero means 'auto', which is the default mode
            if (!val)
            {
                continue;
            }
            file << prefix << pArea->id << "," << year << ',' << sts.id << " = " << val << '\n';
        }
    }
}

} // namespace Antares::Data::ScenarioBuilder

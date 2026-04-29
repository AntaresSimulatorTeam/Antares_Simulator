// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/ShortTermAdditionalConstraintsTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
uint ShortTermAdditionalConstraintsTSNumberData::get_tsGenCount(const Study&) const
{
    return 0;
}

bool ShortTermAdditionalConstraintsTSNumberData::apply(Study& study)
{
    bool ret = true;
    auto tsGenMax = get_tsGenCount(study);
    uint errors = 0;
    for (auto& sts: pArea->shortTermStorage.storagesByIndex)
    {
        for (auto& ct: sts.additionalConstraints)
        {
            auto& rule = rules_[ct.get()];

            std::string logprefix = "Short term storage additional constraints: area '" + pArea->id
                                    + "', sts: '" + sts.id + "': " + ct->id;
            ret = ApplyToMatrix(errors, logprefix, ct->timeSeries, rule[0], tsGenMax) && ret;
        }
    }
    return ret;
}

void ShortTermAdditionalConstraintsTSNumberData::setTSnumber(
  const ShortTermStorage::AdditionalConstraints* sts,
  unsigned year,
  unsigned value)
{
    auto& ts_numbers = rules_[sts];
    if (year < ts_numbers.height)
    {
        ts_numbers[0][year] = value;
    }
}

unsigned ShortTermAdditionalConstraintsTSNumberData::get(
  const ShortTermStorage::AdditionalConstraints* sts,
  unsigned year) const
{
    return rules_.at(sts)[0][year];
}

bool ShortTermAdditionalConstraintsTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    for (const auto& sts: pArea->shortTermStorage.storagesByIndex)
    {
        for (const auto& c: sts.additionalConstraints)
        {
            auto& ts_numbers = rules_[c.get()];
            ts_numbers.reset(1, nbYears);
        }
    }

    return true;
}
} // namespace Antares::Data::ScenarioBuilder

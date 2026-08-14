// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 03/07/23.
//

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class renewableTSNumberData final: public TSNumberData
{
public:
    renewableTSNumberData() = default;

    ~renewableTSNumberData() override = default;

    bool reset(const Study& study) override;

    void attachArea(const Area* area)
    {
        pArea = area;
    }

    void setTSnumber(const Antares::Data::RenewableCluster* cluster,
                     const unsigned int year,
                     unsigned int value);
    unsigned int get(const Antares::Data::RenewableCluster* cluster, const unsigned int year) const;
    bool apply(Study& study) override;
    std::string get_prefix() const override;
    unsigned int get_tsGenCount(const Study& study) const override;

private:
    //! The attached area, if any
    const Area* pArea = nullptr;
};

inline unsigned int renewableTSNumberData::get(const Antares::Data::RenewableCluster* cluster,
                                               const unsigned int year) const
{
    assert(cluster != nullptr);
    if (year < pTSNumberRules.height && cluster->areaWideIndex < pTSNumberRules.width)
    {
        const unsigned int index = cluster->areaWideIndex;
        return pTSNumberRules[index][year];
    }
    return 0;
}

inline std::string renewableTSNumberData::get_prefix() const
{
    return "r,";
}
} // namespace Antares::Data::ScenarioBuilder

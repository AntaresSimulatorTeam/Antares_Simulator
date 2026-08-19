// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 04/07/23.
//

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class thermalTSNumberData final: public TSNumberData
{
public:
    thermalTSNumberData() = default;
    ~thermalTSNumberData() override = default;

    bool reset(const Study& study) override;

    void attachArea(const Area* area)
    {
        pArea = area;
    }

    void setTSnumber(const Antares::Data::ThermalCluster* cluster,
                     const unsigned int year,
                     unsigned int value);
    unsigned int get(const Antares::Data::ThermalCluster* cluster, const unsigned int year) const;
    bool apply(Study& study) override;
    std::string get_prefix() const override;
    unsigned int get_tsGenCount(const Study& study) const override;

private:
    //! The attached area, if any
    const Area* pArea = nullptr;
};

inline unsigned int thermalTSNumberData::get(const Antares::Data::ThermalCluster* cluster,
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

inline std::string thermalTSNumberData::get_prefix() const
{
    return "t,";
}
} // namespace Antares::Data::ScenarioBuilder

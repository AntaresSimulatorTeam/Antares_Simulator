// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 03/07/23.
//

#pragma once
#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class ntcTSNumberData final: public TSNumberData
{
public:
    ntcTSNumberData() = default;
    ~ntcTSNumberData() override = default;

    bool reset(const Study& study) override;

    void attachArea(const Area* area)
    {
        pArea = area;
    }

    void setTSnumber(const Antares::Data::AreaLink* link,
                     const unsigned int year,
                     unsigned int value);
    unsigned int get(const Antares::Data::AreaLink* link, const unsigned int year) const;
    bool apply(Study& study) override;
    std::string get_prefix() const override;
    unsigned int get_tsGenCount(const Study& study) const override;

private:
    //! The attached area, if any
    const Area* pArea = nullptr;
};

inline unsigned int ntcTSNumberData::get(const Antares::Data::AreaLink* link,
                                         const unsigned int year) const
{
    assert(link != nullptr);
    if (year < pTSNumberRules.height && link->indexForArea < pTSNumberRules.width)
    {
        const unsigned int index = link->indexForArea;
        return pTSNumberRules[index][year];
    }
    return 0;
}

inline std::string ntcTSNumberData::get_prefix() const
{
    return "ntc,";
}
} // namespace Antares::Data::ScenarioBuilder

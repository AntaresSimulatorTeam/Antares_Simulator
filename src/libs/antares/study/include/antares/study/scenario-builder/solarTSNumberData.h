// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 04/07/23.
//

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class solarTSNumberData final: public TSNumberData
{
public:
    bool apply(Study& study) override;
    std::string get_prefix() const override;
    unsigned int get_tsGenCount(const Study& study) const override;
};

inline std::string solarTSNumberData::get_prefix() const
{
    return "s,";
}
} // namespace Antares::Data::ScenarioBuilder

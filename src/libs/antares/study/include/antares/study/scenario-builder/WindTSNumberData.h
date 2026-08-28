// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class windTSNumberData final: public TSNumberData
{
public:
    bool apply(Study& study) override;
    std::string get_prefix() const override;
    unsigned int get_tsGenCount(const Study& study) const override;
};

inline std::string windTSNumberData::get_prefix() const
{
    return "w,";
}
} // namespace Antares::Data::ScenarioBuilder

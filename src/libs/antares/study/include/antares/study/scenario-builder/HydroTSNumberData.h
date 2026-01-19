// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class hydroTSNumberData final: public TSNumberData
{
public:
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;
};

inline CString<512, false> hydroTSNumberData::get_prefix() const
{
    return "h,";
}
} // namespace Antares::Data::ScenarioBuilder

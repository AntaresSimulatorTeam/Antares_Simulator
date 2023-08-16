#pragma once

#include "TSnumberData.h"

// =====================
// Hydro Power Credits...
// =====================

namespace Antares::Data::ScenarioBuilder
{
class hydroPowerCreditsTSNumberData : public TSNumberData
{
public:
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;
};

inline CString<512, false> hydroPowerCreditsTSNumberData::get_prefix() const
{
    return "hgp,";
}
}
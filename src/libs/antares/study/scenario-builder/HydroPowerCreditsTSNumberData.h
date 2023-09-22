#pragma once

#include "TSnumberData.h"

// =====================
// Hydro Max Power...
// =====================

namespace Antares::Data::ScenarioBuilder
{
class hydroMaxPowerTSNumberData : public TSNumberData
{
public:
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;

    virtual ~hydroMaxPowerTSNumberData() = default;
};

inline CString<512, false> hydroMaxPowerTSNumberData::get_prefix() const
{
    return "hgp,";
}
}
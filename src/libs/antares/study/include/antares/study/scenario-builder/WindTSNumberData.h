//
// Created by marechaljas on 04/07/23.
//

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class windTSNumberData : public TSNumberData
{
public:
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;
};

inline CString<512, false> windTSNumberData::get_prefix() const
{
    return "w,";
}
}
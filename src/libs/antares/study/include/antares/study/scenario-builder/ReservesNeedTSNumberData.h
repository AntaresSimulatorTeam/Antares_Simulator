// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/study/area/capacityReservation.h"

#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class ReservesNeedTSNumberData final: public TSNumberData
{
public:
    ReservesNeedTSNumberData() = default;
    virtual ~ReservesNeedTSNumberData() = default;

    bool reset(const Study& study) override;

    void attachArea(const Area* area)
    {
        pArea = area;
    }

    void setAreaTSnumber(const uint year, uint value);
    uint get(const uint year) const;
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;

    uint get_tsGenCount(const Study& study) const override
    {
        return 0; // This time series is not generated, so we return 0
    }

private:
    //! The attached area, if any
    const Area* pArea = nullptr;
};

inline uint ReservesNeedTSNumberData::get(const uint year) const
{
    if (year < pTSNumberRules.height)
    {
        return pTSNumberRules[0][year];
    }
    return 0;
}

inline CString<512, false> ReservesNeedTSNumberData::get_prefix() const
{
    return "res,";
}
} // namespace Antares::Data::ScenarioBuilder

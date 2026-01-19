// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ShortTermStorage
{
class AdditionalConstraints;
}

namespace Antares::Data::ScenarioBuilder
{
class ShortTermAdditionalConstraintsTSNumberData final: public TSNumberData
{
public:
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;

    bool reset(const Study& study) override;

    void attachArea(Area* area)
    {
        pArea = area;
    }

    void saveToINIFile(Yuni::IO::File::Stream& file) const;

    void setTSnumber(const ShortTermStorage::AdditionalConstraints* ct,
                     unsigned year,
                     unsigned value);
    unsigned get(const ShortTermStorage::AdditionalConstraints* ct, unsigned year) const;

private:
    std::map<const ShortTermStorage::AdditionalConstraints*, MatrixType> rules_;
    Area* pArea;
};

inline CString<512, false> ShortTermAdditionalConstraintsTSNumberData::get_prefix() const
{
    return "sta,";
}

} // namespace Antares::Data::ScenarioBuilder

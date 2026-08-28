// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "TSnumberData.h"

namespace Antares::Data::ShortTermStorage
{
class STStorageCluster;
}

namespace Antares::Data::ScenarioBuilder
{
class ShortTermInflowsTSNumberData final: public TSNumberData
{
public:
    bool apply(Study& study) override;
    std::string get_prefix() const override;
    unsigned int get_tsGenCount(const Study& study) const override;

    bool reset(const Study& study) override;

    void attachArea(const Area* area)
    {
        pArea = area;
    }

    void setTSnumber(const ShortTermStorage::STStorageCluster* sts, unsigned year, unsigned value);
    unsigned get(const ShortTermStorage::STStorageCluster* sts, unsigned year) const;

private:
    std::map<const ShortTermStorage::STStorageCluster*, MatrixType> rules_;
    const Area* pArea{nullptr};
};

inline std::string ShortTermInflowsTSNumberData::get_prefix() const
{
    return "sts,";
}

} // namespace Antares::Data::ScenarioBuilder

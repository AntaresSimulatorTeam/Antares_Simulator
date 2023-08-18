//
// Created by marechaljas on 04/07/23.
//

#pragma once

#include "TSnumberData.h"
namespace Antares::Data::ScenarioBuilder
{
class thermalTSNumberData : public TSNumberData
{
public:
    thermalTSNumberData() = default;

    bool reset(const Study& study) override;
    void saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const override;

    void attachArea(const Area* area)
    {
        pArea = area;
    }

    void setTSnumber(const Antares::Data::ThermalCluster* cluster, const uint year, uint value);
    uint get(const Antares::Data::ThermalCluster* cluster, const uint year) const;
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;

private:
    //! The attached area, if any
    const Area* pArea = nullptr;
};

inline uint thermalTSNumberData::get(const Antares::Data::ThermalCluster* cluster,
                                     const uint year) const
{
    assert(cluster != nullptr);
    if (year < pTSNumberRules.height && cluster->areaWideIndex < pTSNumberRules.width)
    {
        const uint index = cluster->areaWideIndex;
        return pTSNumberRules[index][year];
    }
    return 0;
}

inline CString<512, false> thermalTSNumberData::get_prefix() const
{
    return "t,";
}
}
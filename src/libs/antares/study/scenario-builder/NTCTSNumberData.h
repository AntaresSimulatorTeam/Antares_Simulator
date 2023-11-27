//
// Created by marechaljas on 03/07/23.
//

#pragma once
#include "TSnumberData.h"

namespace Antares::Data::ScenarioBuilder
{
class ntcTSNumberData : public TSNumberData
{
public:
    ntcTSNumberData() = default;
    virtual ~ntcTSNumberData() = default;

    bool reset(const Study& study) override;
    void saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const override;

    void attachArea(const Area* area)
    {
        pArea = area;
    }

    void setDataForLink(const Antares::Data::AreaLink* link, const uint year, uint value);
    uint get(const Antares::Data::AreaLink* link, const uint year) const;
    bool apply(Study& study) override;
    CString<512, false> get_prefix() const override;
    uint get_tsGenCount(const Study& study) const override;

private:
    //! The attached area, if any
    const Area* pArea = nullptr;
};

inline uint ntcTSNumberData::get(const Antares::Data::AreaLink* link, const uint year) const
{
    assert(link != nullptr);
    if (year < pTSNumberRules.height && link->indexForArea < pTSNumberRules.width)
    {
        const uint index = link->indexForArea;
        return pTSNumberRules[index][year];
    }
    return 0;
}

inline CString<512, false> ntcTSNumberData::get_prefix() const
{
    return "ntc,";
}
}
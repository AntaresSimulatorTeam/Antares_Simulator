//
// Created by marechaljas on 03/07/23.
//

#include "NTCTSNumberData.h"
#include "applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
bool ntcTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    auto linkCount = (uint)pArea->links.size();

    // Resize
    pTSNumberRules.reset(linkCount, nbYears);
    return true;
}

void ntcTSNumberData::saveToINIFile(const Study& /* study */, Yuni::IO::File::Stream& file) const
{
    if (!pArea)
        return;

    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

#ifndef NDEBUG
    if (pTSNumberRules.width)
    {
        assert(pTSNumberRules.width == pArea->links.size());
    }
#endif

    for (const auto& i : pArea->links)
    {
        const Data::AreaLink* link = i.second;
        /*
          When renaming an area, it may happen that i.first is not the name
          of the supporting area. We only trust from->id and to->id.
        */
        const Data::AreaName& fromID = link->from->id;
        const Data::AreaName& withID = link->with->id;
        for (uint y = 0; y != pTSNumberRules.height; ++y)
        {
            const uint val = pTSNumberRules[link->indexForArea][y];
            // Equals to zero means 'auto', which is the default mode
            if (!val)
                continue;
            file << prefix << fromID << "," << withID << "," << y << " = " << val << "\n";
        }
    }
}

void ntcTSNumberData::setDataForLink(const Antares::Data::AreaLink* link,
                                     const uint year,
                                     uint value)
{
    assert(link != nullptr);
    if (year < pTSNumberRules.height && link->indexForArea < pTSNumberRules.width)
        pTSNumberRules[link->indexForArea][year] = value;
}

bool ntcTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // Alias to the current area
    assert(pArea != nullptr);
    assert(pArea->index < study.areas.size());
    const Area& area = *(study.areas.byIndex[pArea->index]);

    const uint ntcGeneratedTScount = get_tsGenCount(study);

    for (const auto& i : pArea->links)
    {
        auto* link = i.second;
        uint linkIndex = link->indexForArea;
        assert(linkIndex < pTSNumberRules.width);
        const auto& col = pTSNumberRules[linkIndex];
        logprefix.clear() << "NTC: area '" << area.name << "', link: '" << link->getName() << "': ";
        ret = ApplyToMatrix(errors, logprefix, *link, col, ntcGeneratedTScount) && ret;
    }
    return ret;
}

uint ntcTSNumberData::get_tsGenCount(const Study& /* study */) const
{
    return 0;
}
} // namespace Antares::Data::ScenarioBuilder

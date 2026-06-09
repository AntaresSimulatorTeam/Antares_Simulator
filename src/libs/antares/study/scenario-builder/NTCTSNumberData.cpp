// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 03/07/23.
//

#include "antares/study/scenario-builder/NTCTSNumberData.h"

#include "antares/study/scenario-builder/applyToMatrix.hxx"

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

void ntcTSNumberData::setTSnumber(const Antares::Data::AreaLink* link, const uint year, uint value)
{
    assert(link != nullptr);
    if (year < pTSNumberRules.height && link->indexForArea < pTSNumberRules.width)
    {
        pTSNumberRules[link->indexForArea][year] = value;
    }
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

    for (const auto& i: pArea->links)
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

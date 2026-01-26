// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/config/config.h"
#include "antares/solver/constraints-builder/cbuilder.h"
#include "antares/study/area/constants.h"

namespace Antares
{
bool CBuilder::completeFromStudy()
{
    uint nCount(1);
    const Data::Area::Map::iterator end = pStudy.areas.end();
    for (Data::Area::Map::iterator i = pStudy.areas.begin(); i != end; ++i)
    {
        Data::Area& area = *(i->second);
        const Data::AreaLink::Map::iterator end = area.links.end();
        for (Data::AreaLink::Map::iterator j = area.links.begin(); j != end; ++j)
        {
            // for all links of the study
            // check if it has been enabled in the INI File
            auto k = findLinkInfoFromNodeNames(j->second->from->id, j->second->with->id);
            if (!k)
            {
                auto infos = std::make_shared<linkInfo>();

                logs.info() << "Read data (link " << nCount++ << ")";
                // if Yes, complete the linkInfo
                // load the pointer
                infos->ptr = j->second;
                infos->type = infos->ptr->assetType;

                pLink.push_back(infos);
            }
        }
    }

    return true;
}

} // namespace Antares

/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#include <yuni/io/file.h>

#include <antares/inifile/inifile.h>
#include "antares/config/config.h"
#include "antares/solver/constraints-builder/cbuilder.h"
#include "antares/study/area/constants.h"

using namespace Yuni;

#define SEP Yuni::IO::Separator

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

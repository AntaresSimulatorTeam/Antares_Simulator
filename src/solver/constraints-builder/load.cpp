/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#include "cbuilder.h"
#include <antares/inifile.h>
#include "../../config.h"
#include "../../libs/antares/study/area/constants.h"
#include <yuni/io/file.h>
//#include <wx/wx.h> // bourrin -> ? modifier
//#include "../../ui/simulator/application/study.h"

using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace Antares
{
bool CBuilder::completeFromStudy()
{
    uint nCount(1);
    const Data::Area::Map::iterator end = pStudy->areas.end();
    for (Data::Area::Map::iterator i = pStudy->areas.begin(); i != end; ++i)
    {
        Data::Area& area = *(i->second);
        const Data::AreaLink::Map::iterator end = area.links.end();
        for (Data::AreaLink::Map::iterator j = area.links.begin(); j != end; ++j)
        {
            // for all links of the study
            // check if it has been enabled in the INI File
            linkInfo* k = findLinkInfoFromNodeNames(j->second->from->id, j->second->with->id);
            if (!k)
            {
                k = new linkInfo();

                logs.info() << "Read data (link " << nCount++ << ")";
                // if Yes, complete the linkInfo
                // load the pointer
                k->ptr = j->second;
                k->type = k->ptr->assetType;

                pLink.push_back(k);
            }
        }
    }

    return true;
}

} // namespace Antares

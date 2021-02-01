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

#include "addtools.h"
#include "../tools/connectioncreator.h"
#include "../tools/remover.h"

namespace Antares
{
namespace Map
{
namespace Private
{
AddingToolsHelper::AddingToolsHelper(Manager& manager,
                                     Tool::List& list,
                                     const int selectedCount,
                                     const int connectionSelectedCount,
                                     const wxPoint& top,
                                     const wxPoint& bottom) :
 pManager(manager),
 pList(list),
 pSelectedCount(selectedCount),
 pConnectionsSelectedCount(connectionSelectedCount),
 pTop(top),
 pBottom(bottom)
{
}

void AddingToolsHelper::operator()()
{
    const bool haveConnections = (pConnectionsSelectedCount != 0);
    const bool haveRealNodes = (pConnectionsSelectedCount != pSelectedCount);

    // The common tools
    createCommonTools(haveRealNodes, haveConnections);

    if (haveRealNodes)
        createToolsForRealNodes();
    if (haveConnections)
        createToolsForConnections();
}

void AddingToolsHelper::createToolsForRealNodes()
{
    // To make new connections between items
    createToolToTheLeft<Tool::ConnectionCreator>();
}

void AddingToolsHelper::createToolsForConnections()
{
}

void AddingToolsHelper::createCommonTools(const bool haveRealNodes, const bool haveConnections)
{
    // To delete selected items
    Tool::Remover* t = createToolToTheBottom<Tool::Remover>();
    if (haveRealNodes && haveConnections)
        t->icon("images/16x16/cancel.png");
    else
    {
        if (haveRealNodes)
            t->icon("images/16x16/zone_minus.png");
        else
        {
            if (haveConnections)
                t->icon("images/16x16/interco_minus.png");
        }
    }

    pBottom.x += 5;
}

} // namespace Private
} // namespace Map
} // namespace Antares

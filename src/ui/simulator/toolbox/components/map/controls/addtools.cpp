// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "addtools.h"
#include "../tools/connectioncreator.h"
#include "../tools/remover.h"

namespace Antares::Map::Private
{
AddingToolsHelper::AddingToolsHelper(Manager& manager,
                                     Tool::List& list,
                                     const int selectedCount,
                                     const int connectionSelectedCount,
                                     const wxPoint& top,
                                     const wxPoint& bottom):
    pManager(manager),
    pList(list),
    pSelectedCount(selectedCount),
    pConnectionsSelectedCount(connectionSelectedCount),
    pTop(top),
    pBottom(bottom)
{
}

Tool::Tool* AddingToolsHelper::operator()()
{
    const bool haveConnections = (pConnectionsSelectedCount != 0);
    const bool haveRealNodes = (pConnectionsSelectedCount != pSelectedCount);

    // The common tools
    Tool::Tool* t = createCommonTools(haveRealNodes, haveConnections);

    if (haveRealNodes)
    {
        createToolsForRealNodes();
    }
    if (haveConnections)
    {
        createToolsForConnections();
    }
    return t;
}

void AddingToolsHelper::createToolsForRealNodes()
{
    // To make new connections between items
    createToolToTheLeft<Tool::ConnectionCreator>();
}

void AddingToolsHelper::createToolsForConnections()
{
}

Tool::Tool* AddingToolsHelper::createCommonTools(const bool haveRealNodes,
                                                 const bool haveConnections)
{
    // To delete selected items
    Tool::Remover* t = createToolToTheBottom<Tool::Remover>();
    if (haveRealNodes && haveConnections)
    {
        t->icon("images/16x16/cancel.png");
    }
    else
    {
        if (haveRealNodes)
        {
            t->icon("images/16x16/zone_minus.png");
        }
        else
        {
            if (haveConnections)
            {
                t->icon("images/16x16/interco_minus.png");
            }
        }
    }

    pBottom.x += 5;
    return t;
}

} // namespace Antares::Map::Private

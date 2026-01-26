// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_MAP_CONTROLS_ADD_TOOLS_H__
#define __ANTARES_TOOLBOX_MAP_CONTROLS_ADD_TOOLS_H__

#include "../tools/tool.h"
#include "../manager.h"

namespace Antares::Map::Private
{
class AddingToolsHelper final
{
public:
    AddingToolsHelper(Manager& manager,
                      Tool::List& list,
                      const int selectedCount,
                      const int connectionSelectedCount,
                      const wxPoint& top,
                      const wxPoint& bottom);

    Tool::Tool* operator()();

private:
    template<class T>
    T* createInstance()
    {
        T* t = new T(pManager);
        pList.push_back(t);
        return t;
    }

    template<class T>
    T* createToolToTheLeft()
    {
        T* t = createInstance<T>();
        t->x(pTop.x - t->width());
        t->y(pTop.y);
        pTop.y += t->height();
        return t;
    }

    template<class T>
    T* createToolToTheBottom()
    {
        T* t = createInstance<T>();
        t->x(pBottom.x);
        t->y(pBottom.y);
        pBottom.x += t->width();
        return t;
    }

    void createToolsForRealNodes();
    void createToolsForConnections();
    Tool::Tool* createCommonTools(const bool haveRealNodes, const bool haveConnections);

private:
    Manager& pManager;
    Tool::List& pList;
    const int pSelectedCount;
    const int pConnectionsSelectedCount;
    wxPoint pTop;
    wxPoint pBottom;
};

} // namespace Antares::Map::Private

#endif // __ANTARES_TOOLBOX_MAP_CONTROLS_ADD_TOOLS_H__

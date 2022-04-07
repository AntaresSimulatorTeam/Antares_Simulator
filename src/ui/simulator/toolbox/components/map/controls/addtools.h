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
#ifndef __ANTARES_TOOLBOX_MAP_CONTROLS_ADD_TOOLS_H__
#define __ANTARES_TOOLBOX_MAP_CONTROLS_ADD_TOOLS_H__

#include <antares/wx-wrapper.h>
#include "../tools/tool.h"
#include "../manager.h"

namespace Antares
{
namespace Map
{
namespace Private
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

} // namespace Private
} // namespace Map
} // namespace Antares

#endif // __ANTARES_TOOLBOX_MAP_CONTROLS_ADD_TOOLS_H__

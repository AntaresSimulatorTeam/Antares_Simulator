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
#ifndef __ANTARES_TOOLBOX_MAP_TOOLS_NEW_CONNECTION_H__
#define __ANTARES_TOOLBOX_MAP_TOOLS_NEW_CONNECTION_H__

#include "tool.h"

namespace Antares
{
namespace Map
{
namespace Tool
{
class ConnectionCreator : public Tool
{
public:
    ConnectionCreator(Manager& manager);
    virtual ~ConnectionCreator();

    virtual void draw(DrawingContext& dc,
                      const bool mouseDown,
                      const wxPoint& position,
                      const wxPoint& absolute) const override;

    virtual bool actionIsImmediate() const override
    {
        return false;
    }
    virtual bool onMouseUp(const int mx, const int my) override;
};

} // namespace Tool
} // namespace Map
} // namespace Antares

#endif // __ANTARES_TOOLBOX_MAP_TOOLS_NEW_CONNECTION_H__

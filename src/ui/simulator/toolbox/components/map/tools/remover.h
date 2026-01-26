// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_MAP_TOOLS_DELETE_H__
#define __ANTARES_TOOLBOX_MAP_TOOLS_DELETE_H__

#include "tool.h"

namespace Antares::Map::Tool
{
class Remover: public Tool
{
public:
    Remover(Manager& manager);
    virtual ~Remover();

    virtual void draw(DrawingContext& dc,
                      const bool mouseDown,
                      const wxPoint& position,
                      const wxPoint& absolute) const;

    virtual bool onMouseUp(const int mx, const int my);
};

} // namespace Antares::Map::Tool

#endif // __ANTARES_TOOLBOX_MAP_TOOLS_DELETE_H__

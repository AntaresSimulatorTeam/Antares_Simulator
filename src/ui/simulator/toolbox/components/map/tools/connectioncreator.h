// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_MAP_TOOLS_NEW_CONNECTION_H__
#define __ANTARES_TOOLBOX_MAP_TOOLS_NEW_CONNECTION_H__

#include "tool.h"

namespace Antares::Map::Tool
{
class ConnectionCreator: public Tool
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

} // namespace Antares::Map::Tool

#endif // __ANTARES_TOOLBOX_MAP_TOOLS_NEW_CONNECTION_H__

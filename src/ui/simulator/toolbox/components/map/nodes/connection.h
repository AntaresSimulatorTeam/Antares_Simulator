/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_MAP_CONNECTION_H__
#define __ANTARES_TOOLBOX_MAP_CONNECTION_H__

#include "item.h"
#include "node.h"
#include "../tools/tool.h"
#include <antares/study/area/area.h>

namespace Antares
{
namespace Map
{
// Forward declaration
namespace Tool
{
class Tool;
}

class Connection : public Item
{
public:
    enum Direction
    {
        dirNone,
        dirDirect,
        dirIndirect,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Connection(Manager& manager, Item* a, Item* b);
    //! Destructor
    virtual ~Connection();
    //@}

    //! \name layerVisibility
    //@{
    /*!
    ** \brief Get the visivility for a layerId
    */
    bool isVisibleOnLayer(const size_t& layerID) const;
    //@}

    //! Type of the node
    virtual Type type() const;

    Item* leftSide()
    {
        return pA;
    }
    const Item* leftSide() const
    {
        return pA;
    }

    Item* rightSide()
    {
        return pB;
    }
    const Item* rightSide() const
    {
        return pB;
    }

    Direction arrowDirection() const
    {
        return pArrowDirection;
    }
    void arrowDirection(const Direction& d);

    Direction direction() const
    {
        return pDirection;
    }
    void direction(const Direction& d);

    /*!
    ** \brief Refresh the cache (even if not invalidated)
    */
    virtual void refreshCache(wxDC& dc);

    virtual bool contains(const int x, const int y, double& distance);

    virtual bool isContained(const int x1, const int y1, const int x2, const int y2) const;

    virtual void draw(DrawingContext& dc);

    virtual void extendBoundingBox(wxPoint& topLeft, wxPoint& bottomRight);

    //! \name Selection
    //@{
    //! Get if the item is selected
    virtual bool selected() const;
    //! Select or unselect the item
    virtual void selected(bool v);
    //@}

    //! \name Link
    //@{
    //! Get the attached link
    Data::AreaLink* attachedAreaLink() const;
    //! Set the attached link
    void attachedAreaLink(Data::AreaLink* a);

    /*!
    ** \brief Create a link if the internal pointers are nil
    */
    void createANewConnectionIfNeeded();
    //@}

private:
    void drawArrow(wxDC& dc, const Direction direction, const wxPoint& rA, const wxPoint& rB);

    void destroyTools();
    void createTools();

private:
    Item* pA;
    Item* pB;
    Direction pDirection;
    Direction pArrowDirection;

    wxPoint pCachedAbsolutePosition;
    wxPoint pCachedSize;
    wxPoint pCachedMiddlePoint;
    wxPoint pCachedTextPos;
    //! Bad line orientation
    bool pCachedError;

    bool pHaveTools;
    Tool::Tool* pToolDirection;

    Data::AreaLink* pAttachedAreaLink;

    wxPen pDefaultPen;
    wxPen pErrorPen;

}; // class Connection

} // namespace Map
} // namespace Antares

#include "connection.hxx"

#endif // __ANTARES_TOOLBOX_MAP_CONNECTION_H__

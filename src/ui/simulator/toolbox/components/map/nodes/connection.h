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
#ifndef __ANTARES_TOOLBOX_MAP_CONNECTION_H__
#define __ANTARES_TOOLBOX_MAP_CONNECTION_H__

#include <antares/wx-wrapper.h>
#include "item.h"
#include "node.h"
#include "../tools/tool.h"
#include <antares/study/area.h>

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

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
#ifndef __ANTARES_TOOLBOX_MAP_ITEM_H__
#define __ANTARES_TOOLBOX_MAP_ITEM_H__

#include <antares/wx-wrapper.h>
#include <vector>
#include <set>
#include <map>
#include <wx/gdicmn.h>
#include <wx/colour.h>
#include <wx/dc.h>

namespace Antares
{
namespace Map
{
class DrawingContext;
class Manager;
class Connection;
class BindingConstraint;

class Item
{
public:
    struct SetCompare
    {
        bool operator()(const Item* s1, const Item* s2) const
        {
            return s1 < s2;
        }
    };

    typedef std::vector<Item*> Vector;

    typedef std::set<Item*, SetCompare> Set;

    typedef std::map<Item*, bool> Links;

    enum Type
    {
        tyUnknown,
        tyNode,
        tyConnection,
        tyBindingConstraint,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Item(Manager& manager, const int zPos = 0);
    /*!
    ** \brief Destructor
    */
    virtual ~Item();
    //@}

    /*!
    ** \brief Type
    */
    virtual Type type() const
    {
        return tyUnknown;
    }

    //! \name Manager
    //@{
    /*!
    ** \brief Get the manager
    */
    Manager& manager()
    {
        return pManager;
    };
    const Manager& manager() const
    {
        return pManager;
    };
    //@}

    //! \name layerVisibility
    //@{
    /*!
    ** \brief Get the visivility for a layerId
    */
    virtual bool isVisibleOnLayer(const size_t& /* layerID */) const
    {
        return false;
    }
    //@}

    //! \name Caption
    //@{
    const wxString& caption() const
    {
        return pCaption;
    }
    void caption(const wxString& v)
    {
        pCaption = v;
        invalidate();
        captionHasChanged();
    }
    //@}

    //! \name X-Coordinate
    //@{
    int x() const
    {
        return pX;
    }
    void x(const int v)
    {
        pX = v;
        invalidate();
        positionHasChanged();
    }
    //@}

    //! \name Y-Coordinate
    //@{
    int y() const
    {
        return pY;
    }
    void y(const int v)
    {
        pY = v;
        invalidate();
        positionHasChanged();
    }
    //@}

    virtual wxPoint absolutePosition(DrawingContext& dc) const;

    //! \name Color
    //@{
    const wxColour& color() const
    {
        return pColor;
    }
    void color(const wxColour& c);
    void color(const wxString& s);
    void color(const int r, const int g, const int b);
    void color(const int r, const int g, const int b, const int alpha);
    //@}

    //! \name Selection
    //@{
    virtual bool selected() const
    {
        return pSelected;
    }
    virtual void selected(bool v);
    //@}

    //! \name Z-Position
    //@{
    int zPosition() const
    {
        return pZPosition;
    }
    //@}

    //! \name Cache
    //@{
    /*!
    ** \brief Refresh the cache (even if not invalidated)
    */
    virtual void refreshCache(wxDC& dc) = 0;

    /*!
    ** \brief Mark the node as invalidated (to force its refresh for the next canvas update)
    */
    void invalidate();

    /*!
    ** \brief Get the node is invalidated
    */
    bool isInvalidated() const
    {
        return pInvalidated;
    }
    //@}

    //! \name Misc
    //@{
    /*!
    ** \brief Get if the drawing representation of the node contains the point (x,y)
    **
    ** This method is used to know if the mouse if over a node or not
    */
    virtual bool contains(const int x, const int y, double& distance) = 0;

    /*!
    ** \brief Get if the drawing representation of the node is contained inside a bounding box
    **
    ** This method is used to know if the mouse if over a node or not
    */
    virtual bool isContained(const int x1, const int y1, const int x2, const int y2) const = 0;

    /*!
    ** \brief Draw the node
    */
    virtual void draw(DrawingContext& dc) = 0;

    virtual void drawExternalDrawer(DrawingContext&)
    {
    }
    //@}

    virtual void move(const int x, const int y);

    /*!
    ** \brief Extends the bounding box
    */
    virtual void extendBoundingBox(wxPoint& topLeft, wxPoint& bottomRight) = 0;

    //! \name Mouse handling
    //@{
    /*!
    ** \brief Mouse double click
    */
    virtual void mouseDblClick()
    {
    }
    //@}

protected:
    void internalClearAllLinks();

    virtual void captionHasChanged()
    {
    }
    virtual void positionHasChanged()
    {
    }
    virtual void colorHasChanged()
    {
    }

protected:
    Manager& pManager;
    //! Color
    wxColour pColor;
    //! Selected
    bool pSelected;
    //! invalidated
    bool pInvalidated;

    //! Y-Coordinate
    int pX;
    //! X-Coordinate
    int pY;
    //! Caption
    wxString pCaption;

    //! Linked with other items
    Links* pLinks;

    int pZPosition;

    // Friends
    friend class ::Antares::Map::Manager;
    friend class ::Antares::Map::Connection;
    friend class ::Antares::Map::BindingConstraint;
}; // class Item

} // namespace Map
} // namespace Antares

#include "../drawingcontext.h"
#include "../manager.h"
#include "connection.h"

#endif // __ANTARES_TOOLBOX_MAP_ITEM_H__

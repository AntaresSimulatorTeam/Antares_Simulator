// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_MAP_DRAWING_CONTEXT_H__
#define __ANTARES_TOOLBOX_MAP_DRAWING_CONTEXT_H__

#include <wx/dc.h>
#include <map>
#include "nodes/node.h"

namespace Antares::Map
{
// Forward declaration
class Node;

/*!
** \brief Drawing Context
**
** This structure is used when drawing the entire map on screen. It is
** a temporary structure, which is only used to broadcast the required
** informations to properly draw each component.
*/
class DrawingContext
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    **
    ** \param dc The Device context (where to draw)
    ** \param origin The origin point (where is (0,0) in the device context)
    ** \param offset The offset for selected items
    ** \param scroll The scroll values, in pixels for the horiz and vert scrollbar
    ** \param clientSize The client size
    */
    DrawingContext(wxDC& dc,
                   const wxPoint& origin,
                   const wxPoint& offset,
                   const wxPoint& scroll,
                   const wxPoint& clientSize,
                   const wxPoint& btRight,
                   const size_t& layer = -1,
                   bool forFileExport = false);

    //! Destructor
    ~DrawingContext()
    {
    }

    //@}

    /*!
    ** \brief Get the origin point on the device context
    */
    const wxPoint& origin() const
    {
        return pOrigin;
    }

    /*!
    ** \brief Get the layer id of the device context
    */
    const size_t& getLayerId() const
    {
        return layerId;
    }

    /*!
    ** \brief Get the device context
    */
    bool isForFileExport()
    {
        return pForFileExport;
    }

    /*!
    ** \brief Get the device context
    */
    wxDC& device()
    {
        return pDC;
    }

    /*!
    ** \brief Get the offset for selected items
    **
    ** When an item is selected, the user may move it. This offset must be
    ** added to the original position of the item to have its real position.
    ** Note that offset is temporary, and will become null again at the next
    ** mouse release.
    */
    const wxPoint& offsetForSelectedNodes() const
    {
        return pOffsetForSelectedNodes;
    }

    /*!
    ** \brief Get the scrolling values for the horizontal and the vertical scrollbars
    */
    const wxPoint& scroll() const
    {
        return pScroll;
    }

    /*!
    ** \brief Get the client size
    */
    const wxPoint& clientSize() const
    {
        return pClientSize;
    }

    /*!
    ** \brief Get the absolute position of the point at the bottom right
    */
    const wxPoint& bottomRight() const
    {
        return pBottomRight;
    }

private:
    //! The wx device context
    wxDC& pDC;
    //! The origin point
    const wxPoint& pOrigin;
    //! Offset for selected items
    const wxPoint& pOffsetForSelectedNodes;
    //! Scrolling values
    const wxPoint& pScroll;
    //! The client size
    const wxPoint& pClientSize;
    //! Scroll + client size
    const wxPoint& pBottomRight;
    //! Drawing layer id if any
    const size_t& layerId;
    //! For an image file output?
    bool pForFileExport;

}; // class DrawingContext

} // namespace Antares::Map

#endif // __ANTARES_TOOLBOX_MAP_DRAWING_CONTEXT_H__

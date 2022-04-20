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
#ifndef __ANTARES_TOOLBOX_MAP_CONTROL_H__
#define __ANTARES_TOOLBOX_MAP_CONTROL_H__

#include <antares/wx-wrapper.h>
#include <wx/scrolwin.h>
#include "nodes/node.h"
#include "manager.h"
#include "tools/tool.h"
#include "settings.h"
#include "../notebook/notebook.h"

#define DEFAULT_MAP_WIDTH 1680
#define DEFAULT_MAP_HEIGHT 1050

namespace Antares
{
namespace Map
{
// Forward declaration
class Component;

/*!
** \brief Sub-Component for drawing the map and handle user-events
*/
class Control final : public wxScrolledWindow
{
    friend class Component;

public:
    enum
    {
        gridSize = 75,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    **
    ** \param parent The parent window
    ** \param component The parent component (Map::Component)
    */
    Control(wxWindow* parent, Component& component);

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    **
    ** \param parent The parent window
    ** \param component The parent component (Map::Component)
    ** \param id The unique id for this control
    */
    Control(wxWindow* parent, Component& component, size_t uID);

    //! Destructor
    virtual ~Control();
    //@}

    /*!
    ** \brief get Control id
    */
    const size_t& getUid() const
    {
        return this->uid;
    }

    //! \name Zoom
    //@{
    double zoom() const
    {
        return pZoom;
    }
    void zoom(const double v);
    //@}

    void invalidate()
    {
        pInvalidated = true;
    }
    const wxColour& getBackgroundColor()
    {
        return pBackgroundColor;
    }
    void setBackgroundColor(const wxColour& newColor)
    {
        pBackgroundColor = newColor;
    }

    //! \name Rendering the map
    //@{
    /*!
    ** \brief Renders the map on screen.
    **
    ** \param evt A wxPaintEvent
    */
    void render(wxPaintEvent& evt);

    /*!
    ** \brief Renders the map in an image object for further on-disk storage.
    **
    ** \param options Structure containing the image rendering options
    */
    wxBitmap getRenderedMapImage(const MapRenderOptions& mapOptions);
    //@}

    //! \name Drawing the map graph elements
    //@{
    /*!
    ** \brief Paints the
    **
    ** \param evt A wxPaintEvent
    */
    void paintGraph(wxDC& dc, bool inFile = false);

    void refresh()
    {
        this->Refresh();
    }

    void reset();

    void mouseMoved(wxMouseEvent& evt);

    void mouseLeftDown(wxMouseEvent& evt);

    void mouseWheelMoved(wxMouseEvent& evt);

    void mouseLeftUp(wxMouseEvent& evt);

    void rightClick(wxMouseEvent& evt);

    void mouseLeftWindow(wxMouseEvent& evt);

    void keyPressed(wxKeyEvent& evt);

    void keyReleased(wxKeyEvent& evt);

    void keyChar(wxKeyEvent& evt);

    void mouseDblClick(wxMouseEvent& evt);

    /*!
    ** \brief Get how many area we have
    */
    uint areasCount() const;

    void setpInfosAreaCount(wxStaticText* infosAreaCount)
    {
        pInfosAreaCount = infosAreaCount;
    }
    wxStaticText* getpInfosAreaCount()
    {
        return pInfosAreaCount;
    }

    /*!
    ** \brief Get how many connections we have
    */
    uint connectionsCount() const;
    void setpInfosConnxCount(wxStaticText* infosConnxCount)
    {
        pInfosConnxCount = infosConnxCount;
    }
    wxStaticText* getpInfosConnxCount()
    {
        return pInfosConnxCount;
    }

    void setPointX(wxTextCtrl* textCtrl)
    {
        pointX = textCtrl;
    }
    wxTextCtrl* getPointX()
    {
        return pointX;
    }
    void setPointY(wxTextCtrl* textCtrl)
    {
        pointY = textCtrl;
    }
    wxTextCtrl* getPointY()
    {
        return pointY;
    }

    void addNewNode(uint count = 1);
    void addNewNodeWhereTheMouseIs();
    void addNewLoadNodeWhereTheMouseIs();
    void addNewThermalNodeWhereTheMouseIs();
    void addNewNuclearNodeWhereTheMouseIs();
    void addNewCCGTNodeWhereTheMouseIs();
    void addNewCoalNodeWhereTheMouseIs();
    void addNewOilNodeWhereTheMouseIs();
    void addNewHydroNodeWhereTheMouseIs();
    void addNewPSPPNodeWhereTheMouseIs();
    void addNewRENWNodeWhereTheMouseIs();
    void addNewWindNodeWhereTheMouseIs();
    void addNewSolarNodeWhereTheMouseIs();
    void addNewBioMNodeWhereTheMouseIs();
    void addNewVoidNodeWhereTheMouseIs();
    void addNewVirtualNodeWhereTheMouseIs();

    void recenter();
    void recenter(int x, int y);

    void computeBoundingBoxOfSelectedNodes();

public:
    //! All nodes
    Manager& nodes;

    //! The toggle button for selecting areas with the mouse
    Antares::Component::Button* pBtnSelectionArea;
    //! The toggle button for selecting links with the mouse
    Antares::Component::Button* pBtnSelectionLink;
    //! The toggle button for selecting clusters with the mouse
    // Antares::Component::Button* pBtnSelectionPlant;
    //! The toggle button for selecting constraints with the mouse
    Antares::Component::Button* pBtnSelectionConstraint;

    //! Popup event
    Yuni::Event<void(int, int)> onPopupEvent;

    //! Control id management
    static size_t newUID;

private:
    /*!
    ** \brief Refresh values in the cache even if it has not been invalidated
    */
    void refreshCache();

    void updateMouseInfosFromEvent(wxMouseEvent& evt);

    /*!
    ** \brief Draws the backgroung grid for an on-screen display
    **
    */
    void drawGrid(wxDC& dc);
    /*!
    ** \brief Draws the selection box around the map elements for an on-screen display
    **
    */
    void drawSelectionBox(wxDC& dc);
    /*!
    ** \brief Draws the nodes
    **
    */
    void drawTools(DrawingContext& drawingContext);
    //@}

    void preDrawBoundingBoxOfSelectedNodes(wxDC& dc, const BoundingBox& bb);

    void drawSingleNode(wxDC& dc, Node* node);

    void removeTools();
    void removeTools(const Tool::LifeSpan filter);
    Tool::Tool* findTool(const int x, const int y);

    void performActionForSelectedTool();

    void addToolsForCurrentSelection();

    void onScroll(wxScrollWinEvent& evt);

    void onSize(wxSizeEvent& evt);

private:
    enum MouseAction
    {
        mouseActionNone,
        mouseActionSelectionBox,
        mouseActionMoveSelection,
        mouseActionTool,
    };

private:
    Component& pComponent;
    bool pInvalidated;
    bool pInvalidatedInnerCacheSize;

    wxTextCtrl* pointX;
    wxTextCtrl* pointY;

    wxStaticText* pInfosAreaCount;
    wxStaticText* pInfosConnxCount;
    //! Zoom to apply
    double pZoom;
    wxColour pBackgroundColor;

    wxPoint pCachedRealSize;
    wxPoint pCachedBoxSize;
    wxPoint pCachedOrigin;
    wxPoint pOffsetForSelectedNodes;
    wxPoint pBottomRight;

    bool pKeyShift;
    bool pKeyCtrl;
    wxPoint pCurrentScroll;
    wxPoint pCurrentMousePosition;
    wxPoint pCurrentMousePositionGraph;
    wxPoint pCurrentClientSize;
    wxPoint pLastMousePosition;
    Tool::Tool* pLastSelectedTool;
    Tool::Tool* pRemoverForSelection = nullptr;

    MouseAction pMouseAction;
    BoundingBox pSelectionBox;

    Tool::List pTools;

    int previousMaxSizeX;
    int previousMaxSizeY;

    const size_t uid;

    Antares::Component::Notebook::Page* pPage;

    bool wasDrawn;

    // Event table
    DECLARE_EVENT_TABLE()

}; // class Control

} // namespace Map
} // namespace Antares

#endif // __ANTARES_TOOLBOX_MAP_CONTROL_H__

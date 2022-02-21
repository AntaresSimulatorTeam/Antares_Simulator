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

#include <algorithm>
#include "control.h"
#include <wx/dcbuffer.h>
#include <wx/settings.h>
#include "drawingcontext.h"
#include <wx/wupdlock.h>
#include "../../resources.h"
#include "settings.h"
#include "nodes/bindingconstraint.h"

#include "controls/addtools.h"
#include "tools/remover.h"
#include "component.h"
#include "../../../application/main/main.h"
#include "../../../windows/inspector.h"
#include <ui/common/component/panel.h>
#include <wx/dcsvg.h>

using namespace Yuni;

namespace Antares
{
namespace Map
{
BEGIN_EVENT_TABLE(Control, wxScrolledWindow)

EVT_PAINT(Control::render)

EVT_MOTION(Control::mouseMoved)
EVT_LEFT_DOWN(Control::mouseLeftDown)
EVT_LEFT_UP(Control::mouseLeftUp)
EVT_RIGHT_DOWN(Control::rightClick)
EVT_LEAVE_WINDOW(Control::mouseLeftWindow)
EVT_LEFT_DCLICK(Control::mouseDblClick)

EVT_KEY_DOWN(Control::keyPressed)
EVT_KEY_UP(Control::keyReleased)
EVT_CHAR(Control::keyChar)

EVT_MOUSEWHEEL(Control::mouseWheelMoved)

EVT_SCROLLWIN(Control::onScroll)
EVT_SIZE(Control::onSize)

END_EVENT_TABLE()

Control::Control(wxWindow* parent, Component& component) :
 wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
 nodes(*Manager::Instance()),
 pComponent(component),
 pInvalidated(true),
 pInvalidatedInnerCacheSize(true),
 pZoom(1.),
 pBackgroundColor(Settings::background),
 pCachedBoxSize(0, 0),
 pCachedOrigin(0, 0),
 pOffsetForSelectedNodes(0, 0),
 pKeyShift(false),
 pKeyCtrl(false),
 pCurrentScroll(0, 0),
 pCurrentMousePosition(INT_MAX, INT_MAX),
 pCurrentMousePositionGraph(INT_MAX, INT_MAX),
 pCurrentClientSize(0, 0),
 pLastMousePosition(INT_MAX, INT_MAX),
 pLastSelectedTool(nullptr),
 pMouseAction(mouseActionNone),
 pSelectionBox(),
 pTools(),
 previousMaxSizeX(-1),
 previousMaxSizeY(-1),
 uid(newUID++),
 pInfosAreaCount(nullptr),
 pInfosConnxCount(nullptr),
 wasDrawn(false)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // GTK
    SetBackgroundColour(pBackgroundColor);
    SetForegroundColour(pBackgroundColor);

    pCachedOrigin.x = 0;
    pCachedOrigin.y = 0;

    // The scroll rate : 10 pixels
    SetScrollRate(12, 12);
}

Control::Control(wxWindow* parent, Component& component, size_t uID) :
 wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
 nodes(*Manager::Instance()),
 pComponent(component),
 pInvalidated(true),
 pInvalidatedInnerCacheSize(true),
 pZoom(1.),
 pBackgroundColor(Settings::background),
 pCachedBoxSize(0, 0),
 pCachedOrigin(0, 0),
 pOffsetForSelectedNodes(0, 0),
 pKeyShift(false),
 pKeyCtrl(false),
 pCurrentScroll(0, 0),
 pCurrentMousePosition(INT_MAX, INT_MAX),
 pCurrentMousePositionGraph(INT_MAX, INT_MAX),
 pCurrentClientSize(0, 0),
 pLastMousePosition(INT_MAX, INT_MAX),
 pLastSelectedTool(nullptr),
 pMouseAction(mouseActionNone),
 pSelectionBox(),
 pTools(),
 previousMaxSizeX(-1),
 previousMaxSizeY(-1),
 uid(uID),
 pInfosAreaCount(nullptr),
 pInfosConnxCount(nullptr),
 wasDrawn(false)
{
    while (newUID <= uID)
        newUID++;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // GTK
    SetBackgroundColour(pBackgroundColor);
    SetForegroundColour(pBackgroundColor);

    pCachedOrigin.x = 0;
    pCachedOrigin.y = 0;

    // The scroll rate : 10 pixels
    SetScrollRate(12, 12);
}

Control::~Control()
{
    removeTools();
}

void Control::zoom(const double v)
{
    pZoom = v;
    refresh();
}

void Control::reset()
{
    // Invalidate all caches
    pMouseAction = mouseActionNone;
    pInvalidated = true;
    pInvalidatedInnerCacheSize = true;

    // Removed the old position of the mouse to prevent against bad re-selection
    pSelectionBox.second.x = 0;
    pSelectionBox.second.y = 0;
    pOffsetForSelectedNodes.x = 0;
    pOffsetForSelectedNodes.y = 0;

    // Delete all tools to avoid persistent tools after reloading a study
    pLastSelectedTool = nullptr;
    removeTools();
}

void Control::render(wxPaintEvent&)
{
    // The DC
    wxAutoBufferedPaintDC dc(this);
    // Shifts the device origin so we don't have to worry
    // about the current scroll position ourselves
    DoPrepareDC(dc);

    // Paint the map
    paintGraph(dc);

    if (!wasDrawn)
    {
        wasDrawn = true;
        Yuni::Bind<void()> callback;
        callback.bind(this, &Control::recenter);
        Dispatcher::GUI::Post(callback, 20); // arbitrary
    }
}

wxBitmap Control::getRenderedMapImage(const MapRenderOptions& mapOptions)
{
    wxMemoryDC tmpDC;
    wxColor tmpColor;

    // The image should contain no more than the areas boxing
    BoundingBox box = nodes.boundingBox(getUid());
    wxRect boundsRect(box.first, box.second);

    // Drawing in a bitmap
    wxBitmap bmp(boundsRect.GetWidth() + 80,
                 boundsRect.GetHeight() + 80); // 40 more pixels on each side to allow drawing the
                                               // shadow(down and right) and some space around

    tmpDC.SelectObject(bmp);

    if (!mapOptions.transparentBackground)
        pBackgroundColor = mapOptions.backgroundColor;

    paintGraph(tmpDC, true);

    tmpDC.SelectObject(wxNullBitmap);

    if (mapOptions.transparentBackground
        && mapOptions.fileFormat != Antares::Map::mapImageFormat::mfJPG)
        bmp.SetMask(
          new wxMask(bmp, pBackgroundColor)); // suppress the background color to let it transparent

    pBackgroundColor = Settings::background; // reset the color to the default

    return bmp;
}

void Control::drawGrid(wxDC& dc)
{
    // Grid
    dc.SetPen(wxPen(Settings::grid, 1, wxPENSTYLE_DOT));
    // Horizontal lines
    for (int y = pCachedOrigin.y - gridSize; y > 0; y -= gridSize)
        dc.DrawLine(0, y, pCachedRealSize.x, y);
    for (int y = pCachedOrigin.y + gridSize; y < (pCachedOrigin.y << 1); y += gridSize)
        dc.DrawLine(0, y, pCachedRealSize.x, y);
    // Vertical lines
    for (int x = pCachedOrigin.x - gridSize; x > 0; x -= gridSize)
        dc.DrawLine(x, 0, x, pCachedRealSize.y);
    for (int x = pCachedOrigin.x + gridSize; x < (pCachedOrigin.y << 1); x += gridSize)
        dc.DrawLine(x, 0, x, pCachedRealSize.y);

    // Major lines
    dc.SetPen(wxPen(Settings::gridCenter, 1, wxPENSTYLE_SOLID));
    dc.DrawLine(0, pCachedOrigin.y, pCachedRealSize.x, pCachedOrigin.y);
    dc.DrawLine(pCachedOrigin.x, 0, pCachedOrigin.x, pCachedRealSize.y);
}

void Control::drawSelectionBox(wxDC& dc)
{
    // Selection box
    switch (pMouseAction)
    {
    case mouseActionTool:
    case mouseActionNone:
    {
        if (nodes.selectedItemsCount())
        {
            if (not pSelectionBox.second.x)
                computeBoundingBoxOfSelectedNodes();
            preDrawBoundingBoxOfSelectedNodes(dc, pSelectionBox);
        }
        break;
    }
    case mouseActionSelectionBox:
    {
        // Colors
        dc.SetPen(wxPen(Settings::mouseSelectionBorder));
        dc.SetBrush(wxBrush(Settings::mouseSelectionBackground));
        // New coordinates
        const wxPoint a(pSelectionBox.first.x + pCachedOrigin.x,
                        pCachedOrigin.y - pSelectionBox.first.y);
        const wxPoint b(pSelectionBox.second.x + pCachedOrigin.x - a.x,
                        pCachedOrigin.y - pSelectionBox.second.y - a.y);
        // Draw
        dc.DrawRectangle(a.x, a.y, b.x, b.y);

        break;
    }
    default:
        break;
    }
}

void Control::drawTools(DrawingContext& drawingContext)
{
    // Drawing all tools buttons
    if (not pTools.empty() && (mouseActionNone == pMouseAction || mouseActionTool == pMouseAction))
    {
        bool mouseDown = (mouseActionTool == pMouseAction);
        auto end = pTools.end();
        for (auto i = pTools.begin(); i != end; ++i)
            (*i)->draw(drawingContext,
                       mouseDown && (pLastSelectedTool == *i),
                       pCurrentMousePositionGraph,
                       pCurrentMousePosition);
    }
}

void Control::paintGraph(wxDC& dc, bool inFile)
{
    // A null point
    static const wxPoint nullPoint = wxPoint(0, 0);
    // An empty bounding box
    wxPoint topLeft(INT_MAX, INT_MIN);
    wxPoint bottomRight(INT_MIN, INT_MAX);
    BoundingBox emptyBB(topLeft, bottomRight);

    // Initial settings for the device context
    dc.SetUserScale(pZoom, pZoom);
    dc.SetFont(*wxSWISS_FONT);

    // Refresh the cache of all nodes if needed
    nodes.refreshCacheForAllNodes(dc);

    if (pInvalidated) // Cache update
        refreshCache();

    // To have the size of the control : (wxPoint)GetClientSize()
    wxRect windowRect(nullPoint, pCachedBoxSize);

    // Default Colors
    dc.SetBrush(wxBrush(pBackgroundColor));
    dc.SetPen(wxPen(pBackgroundColor));
    // current version of wxSVGFileDC doesn't support the Clear() function!

    if (dynamic_cast<wxSVGFileDC*>(&dc) == nullptr)
        dc.Clear();

    // We need to shift the client rectangle to take into account
    // scrolling, converting device to logical coordinates
    CalcUnscrolledPosition(windowRect.x, windowRect.y, &windowRect.x, &windowRect.y);

    // Fill the background
    if (pBackgroundColor != wxNullColour)
        dc.DrawRectangle(wxRect(nullPoint, pCachedRealSize));

    // Fixing the drawing zone coordinates
    wxPoint cxtOrigin;
    wxPoint cxtOffsetForSelectedNodes;
    wxPoint cxtCurrentScroll;
    wxPoint cxtClientSize;
    wxPoint cxtBottomRight; // the opposite limit from the origin

    cxtOrigin = pCachedOrigin;
    cxtOffsetForSelectedNodes = pOffsetForSelectedNodes;
    cxtCurrentScroll = pCurrentScroll;
    cxtClientSize = pCurrentClientSize;
    cxtBottomRight = pBottomRight;

    // Display on screen
    if (not inFile)
    {
        // Drawing background
        drawGrid(dc);

        // Drawing the selection box if needed
        drawSelectionBox(dc);
    }
    else
    {
        // draw in an image file

        // no background grid to draw

        // The image should contain no more than the areas boxing and a small padding, let say 40 px
        BoundingBox box = nodes.boundingBox(getUid());
        wxRect boundsRect(box.first, box.second);

        // changing the drawing origin so the exported picture will have the bounding box drawn in
        // the upper-left corner
        cxtOrigin = wxPoint(1 - (boundsRect.x - 40), boundsRect.GetHeight() + boundsRect.y + 40);
        cxtOffsetForSelectedNodes = nullPoint;
        cxtCurrentScroll = nullPoint;
        // cxtCurrentScroll			= cxtOrigin;
        cxtClientSize = wxPoint(boundsRect.GetWidth() + 80, boundsRect.GetHeight() + 80);
        cxtBottomRight = cxtClientSize;
    }

    // A drawing context
    DrawingContext drawingContext(dc,
                                  cxtOrigin,
                                  cxtOffsetForSelectedNodes,
                                  cxtCurrentScroll,
                                  cxtClientSize,
                                  cxtBottomRight,
                                  getUid(),
                                  inFile);

    // Draw all nodes
    nodes.draw(drawingContext);

    // draw tools
    if (not inFile)
        drawTools(drawingContext);
}

void Control::refreshCache()
{
    // Computing the bounding box
    BoundingBox box = nodes.boundingBox(getUid());
    pCachedBoxSize.x = Math::Max<int>(Math::Max<int>(abs(box.first.x), Math::Abs(box.second.x)) * 2,
                                      DEFAULT_MAP_WIDTH);
    pCachedBoxSize.y = Math::Max<int>(Math::Max<int>(abs(box.first.y), Math::Abs(box.second.y)) * 2,
                                      DEFAULT_MAP_HEIGHT);

    // Updating the scrollbars
    const int newSizeX = ((int)Math::Round((double)pCachedBoxSize.x * pZoom)) << 1; // * 2
    const int newSizeY = ((int)Math::Round((double)pCachedBoxSize.y * pZoom)) << 1; // * 2
    if (newSizeX > previousMaxSizeX || newSizeY > previousMaxSizeY)
    {
        pInvalidatedInnerCacheSize = false;
        // Grabbing infos for the scrolling
        // 1 Pixels per scroll unit
        int scrollX;
        int scrollY;
        GetViewStart(&scrollX, &scrollY);
        scrollX = pCachedOrigin.x - scrollX;
        scrollY = pCachedOrigin.y - scrollY;

        // Computing the origin point
        pCachedOrigin.x = newSizeX >> 1; // div 2
        pCachedOrigin.y = newSizeY >> 1; // div 2

        previousMaxSizeX = newSizeX;
        previousMaxSizeY = newSizeY;
        pCachedRealSize.x = newSizeX;
        pCachedRealSize.y = newSizeY;
        SetScrollbars(1, 1, newSizeX, newSizeY);
        Scroll(pCachedOrigin.x - scrollX, pCachedOrigin.y - scrollY);
        GetViewStart(&pCurrentScroll.x, &pCurrentScroll.y);
        GetClientSize(&pCurrentClientSize.x, &pCurrentClientSize.y);
        pBottomRight.x = pCurrentScroll.x + pCurrentClientSize.x;
        pBottomRight.y = pCurrentScroll.y + pCurrentClientSize.y;
    }
    else
    {
        if (pInvalidatedInnerCacheSize)
        {
            pInvalidatedInnerCacheSize = false;
            GetViewStart(&pCurrentScroll.x, &pCurrentScroll.y);
            GetClientSize(&pCurrentClientSize.x, &pCurrentClientSize.y);
            pBottomRight.x = pCurrentScroll.x + pCurrentClientSize.x;
            pBottomRight.y = pCurrentScroll.y + pCurrentClientSize.y;
        }
    }

    // The cache has been updated
    pInvalidated = false;
}

void Control::addToolsForCurrentSelection()
{
    // Remove all deprecated tools
    removeTools(Tool::lifeSpanMouseSelection);

    const bool haveConnection = nodes.selectedItemsAsConnectionCount() != 0;
    const bool haveNodes = (nodes.selectedItemsAsConnectionCount() != nodes.selectedItemsCount());
    if (haveNodes || haveConnection)
    {
        Private::AddingToolsHelper helper(
          nodes,
          pTools,
          nodes.selectedItemsCount(),
          nodes.selectedItemsAsConnectionCount(),
          wxPoint(pSelectionBox.first.x + 1, pSelectionBox.first.y + 2),
          wxPoint(pSelectionBox.first.x + 4, pSelectionBox.first.y + pSelectionBox.second.y - 1));
        pRemoverForSelection = helper();
    }
}

void Control::computeBoundingBoxOfSelectedNodes()
{
    // An empty bounding box
    wxPoint topLeft(INT_MAX, INT_MIN);
    wxPoint bottomRight(INT_MIN, INT_MAX);
    BoundingBox emptyBB(topLeft, bottomRight);

    enum
    {
        space = 7,
    };
    pSelectionBox = nodes.boundingBoxOfSelectedNodes(getUid());
    if (pSelectionBox == emptyBB)
    {
        removeTools();
        return;
    }

    // Size
    pSelectionBox.second.x = abs(pSelectionBox.second.x - pSelectionBox.first.x);
    pSelectionBox.second.y = abs(pSelectionBox.second.y - pSelectionBox.first.y);

    // Offset - origin
    pSelectionBox.first.x += pCachedOrigin.x;
    pSelectionBox.first.y = pCachedOrigin.y - pSelectionBox.first.y;

    // Borders
    pSelectionBox.first.x -= space;
    pSelectionBox.second.x += (space << 1);
    pSelectionBox.first.y -= space;
    pSelectionBox.second.y += (space << 1);

    // Need some tools !
    addToolsForCurrentSelection();
}

void Control::preDrawBoundingBoxOfSelectedNodes(wxDC& dc, const BoundingBox& bb)
{
    if (pMouseAction == mouseActionTool)
    {
        dc.SetPen(wxPen(Settings::selectionBoxBorderHighlighted));
        dc.SetBrush(wxBrush(Settings::selectionBoxBackgroundHighlighted));
    }
    else
    {
        dc.SetPen(wxPen(Settings::selectionBoxBorder));
        dc.SetBrush(wxBrush(Settings::selectionBoxBackground));
    }
    // Global bounding box for all selected nodes
    dc.DrawRoundedRectangle(bb.first.x, bb.first.y, bb.second.x, bb.second.y, 5);
}

Tool::Tool* Control::findTool(const int x, const int y)
{
    Tool::List::iterator end = pTools.end();
    for (Tool::List::iterator i = pTools.begin(); i != end; ++i)
    {
        if ((*i)->contains(x, y))
            return *i;
    }
    return nullptr;
}

void Control::removeTools()
{
    if (not pTools.empty())
    {
        auto end = pTools.end();
        for (auto i = pTools.begin(); i != end; ++i)
            delete *i;
        pTools.clear();
    }
}

class RemovalToolPredicate
{
public:
    RemovalToolPredicate(Tool::LifeSpan p) : pFilter(p)
    {
    }

    bool operator()(const Tool::Tool* t) const
    {
        if (t->lifeSpan() == pFilter)
        {
            delete t;
            return true;
        };
        return false;
    }

private:
    Tool::LifeSpan pFilter;
};

void Control::removeTools(const Tool::LifeSpan filter)
{
    if (!pTools.empty())
    {
        pTools.erase(std::remove_if(pTools.begin(), pTools.end(), RemovalToolPredicate(filter)),
                     pTools.end());
    }
}

void Control::updateMouseInfosFromEvent(wxMouseEvent& evt)
{
    if (pInvalidated)
        refreshCache();

    // Computing the new Mouse pCurrentMousePositionGraph with the map's referencial
    GetViewStart(&pCurrentScroll.x, &pCurrentScroll.y);
    GetClientSize(&pCurrentClientSize.x, &pCurrentClientSize.y);

    pCurrentMousePosition = evt.GetPosition();
    pCurrentMousePosition.x += pCurrentScroll.x;
    pCurrentMousePosition.y += pCurrentScroll.y;

    pCurrentMousePositionGraph.x = pCurrentMousePosition.x - pCachedOrigin.x;
    pCurrentMousePositionGraph.y = -(pCurrentMousePosition.y - pCachedOrigin.y);
}

void Control::mouseMoved(wxMouseEvent& evt)
{
    // Notify other components as well
    Antares::Component::Panel::OnMouseMoveFromExternalComponent();

    // Update informations about the mouse position
    this->updateMouseInfosFromEvent(evt);

    switch (pMouseAction)
    {
    case mouseActionNone:
        return;
        break;
    case mouseActionMoveSelection:
    {
        pOffsetForSelectedNodes.x = -pLastMousePosition.x + pCurrentMousePositionGraph.x;
        pOffsetForSelectedNodes.y = pLastMousePosition.y - pCurrentMousePositionGraph.y;
        break;
    }
    case mouseActionSelectionBox:
    {
        // Selection box
        pSelectionBox.second = pCurrentMousePositionGraph;
        nodes.selectFromBoundingBox(pSelectionBox.first, pSelectionBox.second, getUid());
        break;
    }
    case mouseActionTool:
        break;
    }
    refresh();
}

void Control::performActionForSelectedTool()
{
    // Perform action
    if (pLastSelectedTool->onMouseUp(pCurrentMousePositionGraph.x, pCurrentMousePositionGraph.y))
    {
        pSelectionBox.second.x = 0;
        pSelectionBox.second.y = 0;
        removeTools(Tool::lifeSpanMouseSelection);
        refresh();
    }
    pLastSelectedTool = nullptr;
    pMouseAction = mouseActionNone;
}

void Control::mouseLeftDown(wxMouseEvent&)
{
    pLastMousePosition = pCurrentMousePositionGraph;
    pOffsetForSelectedNodes.x = 0;
    pOffsetForSelectedNodes.y = 0;

    if (pLastSelectedTool)
    {
        Dispatcher::GUI::Post(this, &Control::performActionForSelectedTool);
    }
    else
    {
        if ((pLastSelectedTool = findTool(pCurrentMousePosition.x, pCurrentMousePosition.y)))
        {
            // The user has clicked on a tool
            pMouseAction = mouseActionTool;
        }
        else
        {
            // Looking for a node
            Item* item = nodes.find(pCurrentMousePositionGraph.x, pCurrentMousePositionGraph.y);
            if (item && item->isVisibleOnLayer(getUid()))
            {
                // A node has been clicked
                if (!pKeyShift)
                {
                    if (!item->selected())
                        nodes.selectOnly(item);
                }
                else
                {
                    nodes.changeItemSelectionState(item);
                }
                pMouseAction = mouseActionMoveSelection;
            }
            else
            {
                // Nowhere...
                if (!pKeyShift)
                {
                    nodes.unselectAll();
                    pSelectionBox.first = pCurrentMousePositionGraph;
                    pSelectionBox.second = pCurrentMousePositionGraph;
                    pRemoverForSelection = nullptr;
                    pMouseAction = mouseActionSelectionBox;
                }
            }
        }

        // Force the refresh
        refresh();
    }
}

void Control::mouseWheelMoved(wxMouseEvent&)
{
}

void Control::mouseLeftUp(wxMouseEvent&)
{
    if (mouseActionMoveSelection == pMouseAction)
    {
        // Save the changes
        nodes.moveAllSelected(pOffsetForSelectedNodes.x, -pOffsetForSelectedNodes.y);
        pInvalidated = true;
    }

    // Remove old tools
    if (pLastSelectedTool)
    {
        if (pLastSelectedTool->actionIsImmediate())
            performActionForSelectedTool();
    }
    else
    {
        pOffsetForSelectedNodes.x = 0;
        pOffsetForSelectedNodes.y = 0;

        pMouseAction = mouseActionNone;
        // If some nodes are selected, we would have a bounding box with a
        // direct access to the tools
        // The size of the bounding are set to 0, to force a refresh of cached values
        // before drawing
        pSelectionBox.second.x = 0;
        pSelectionBox.second.y = 0;
        removeTools(Tool::lifeSpanMouseSelection);
        SetFocus();
    }

    // Refresh the canvas
    refresh();
}

void Control::addNewNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption();
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewLoadNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("LOAD");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(0, 0, 0);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewThermalNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("THER");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(128, 64, 64);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewNuclearNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("NUCL");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(255, 255, 0);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewCCGTNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("CCGT");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(255, 128, 128);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewCoalNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("COAL");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(255, 128, 128);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewOilNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("OILT");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(255, 128, 128);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewHydroNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("HYDR");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(0, 0, 255);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewPSPPNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("PSPP");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(0, 128, 255);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewRENWNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("RENW");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(0, 128, 128);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewWindNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("WIND");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(0, 255, 128);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewSolarNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("SOLA");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(0, 255, 128);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewBioMNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("BIOM");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(0, 255, 128);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewVoidNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("void");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(192, 192, 192);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewVirtualNodeWhereTheMouseIs()
{
    Data::AreaName newName = nodes.findNewCaption("&vir");
    if (not newName.empty())
    {
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        Node* newNode = nodes.addNode(wxStringFromUTF8(id),
                                      wxStringFromUTF8(newName),
                                      pCurrentMousePositionGraph.x,
                                      pCurrentMousePositionGraph.y);
        newNode->addLayerVisibility(getUid());
        newNode->color(255, 0, 0);
        // select the new node
        reset();
        nodes.selectOnly(newNode);
        refresh();
    }
}

void Control::addNewNode(uint count)
{
    if (!count)
        return;

    reset();
    uint offsetX = 0;
    uint offsetY = 0;
    uint nbreak = 0;
    if (count > 2)
        nbreak = count / 2;

    GetViewStart(&pCurrentScroll.x, &pCurrentScroll.y);
    GetClientSize(&pCurrentClientSize.x, &pCurrentClientSize.y);
    int x = pCurrentScroll.x + pCurrentClientSize.x / 2 - pCachedOrigin.x;
    int y = -(pCurrentScroll.y + pCurrentClientSize.y / 2 - pCachedOrigin.y);

    Item::Vector selection;

    for (uint i = 1; i <= count; ++i)
    {
        Data::AreaName newName = nodes.findNewCaption();
        if (newName.empty())
            continue;

        Data::AreaName id;
        TransformNameIntoID(newName, id);
        auto* newNode = nodes.addNode(
          wxStringFromUTF8(id), wxStringFromUTF8(newName), x + offsetX, y + offsetY);

        newNode->addLayerVisibility(getUid());
        // select the new node
        selection.push_back(newNode);

        if (i == nbreak)
        {
            offsetY += 35;
            offsetX = 0;
        }
        else
            offsetX += 70;
    }
    nodes.selectOnly(selection);
    refresh();
    pComponent.refresh();
}

void Control::recenter()
{
    //! VB changes to be validate
    /* */
    // Computing the bounding box
    BoundingBox box = nodes.boundingBox(getUid());
    pCachedBoxSize.x = Math::Max<int>(Math::Max<int>(abs(box.first.x), Math::Abs(box.second.x)) * 2,
                                      DEFAULT_MAP_WIDTH);
    pCachedBoxSize.y = Math::Max<int>(Math::Max<int>(abs(box.first.y), Math::Abs(box.second.y)) * 2,
                                      DEFAULT_MAP_HEIGHT);

    // Updating the scrollbars
    const int newSizeX = ((int)Math::Round((double)pCachedBoxSize.x * pZoom)) << 1; // * 2
    const int newSizeY = ((int)Math::Round((double)pCachedBoxSize.y * pZoom)) << 1; // * 2
    {
        // Grabbing infos for the scrolling
        // 1 Pixels per scroll unit
        int scrollX;
        int scrollY;
        GetViewStart(&scrollX, &scrollY);
        scrollX = pCachedOrigin.x - scrollX;
        scrollY = pCachedOrigin.y - scrollY;

        // Computing the origin point
        pCachedOrigin.x = newSizeX >> 1; // div 2
        pCachedOrigin.y = newSizeY >> 1; // div 2

        // previousMaxSizeX = newSizeX;
        // previousMaxSizeY = newSizeY;
        pCachedRealSize.x = newSizeX;
        pCachedRealSize.y = newSizeY;
        SetScrollbars(1, 1, newSizeX, newSizeY);
        Scroll(pCachedOrigin.x - scrollX, pCachedOrigin.y - scrollY);
        GetViewStart(&pCurrentScroll.x, &pCurrentScroll.y);
        GetClientSize(&pCurrentClientSize.x, &pCurrentClientSize.y);
        pBottomRight.x = pCurrentScroll.x + pCurrentClientSize.x;
        pBottomRight.y = pCurrentScroll.y + pCurrentClientSize.y;
    }
    /* */

    int w, h;
    GetVirtualSize(&w, &h);
    int cw, ch;
    GetClientSize(&cw, &ch);

    Scroll((w - cw) / 2, (h - ch) / 2);

    pInvalidated = true;
    pInvalidatedInnerCacheSize = true;
    Dispatcher::GUI::Refresh(this);
}

void Control::recenter(int x, int y)
{
    // refreshCache();
    int w, h;
    GetVirtualSize(&w, &h);
    int cw, ch;
    GetClientSize(&cw, &ch);

    // wxRect windowRect(wxPoint(0, 0), GetClientSize());
    // We need to shift the client rectangle to take into account
    // scrolling, converting device to logical coordinates
    // CalcUnscrolledPosition(windowRect.x, windowRect.y, & windowRect.x, & windowRect.y);

    int posX0 = (w - cw) / 2;
    int posY0 = (h - ch) / 2;
    int decalageX = (x);
    int decalageY = (y);
    int coordX = posX0 + decalageX;
    int coordY = posY0 - decalageY;

    /*
    if((w < Math::Abs((coordX*2+cw))) || (h <  Math::Abs((coordY*2+ch))))
            SetVirtualSize(Math::Abs((coordX*2+cw)), Math::Abs((coordY*2+ch)));
    */

    Scroll(wxMax(0, coordX), wxMax(0, coordY)); // coordX, coordY

    pInvalidated = true;
    pInvalidatedInnerCacheSize = true;
    // refreshCache();
    Dispatcher::GUI::Refresh(this);
}

void Control::rightClick(wxMouseEvent& evt)
{
    // Do nothing
    if (nodes.selectedItemsCount() > 0)
        onPopupEvent(evt.GetX(), evt.GetY());
}

void Control::mouseLeftWindow(wxMouseEvent&)
{
    // Do nothing
}

void Control::keyPressed(wxKeyEvent& evt)
{
    long keyCode = evt.GetKeyCode();
    switch (keyCode)
    {
    case WXK_SHIFT:
    {
        pKeyShift = true;
        break;
    }
    case WXK_ESCAPE:
    {
        pMouseAction = mouseActionNone;
        pLastSelectedTool = nullptr;
        refresh();
        break;
    }
    case WXK_DELETE:
    {
        if (pRemoverForSelection
            && pRemoverForSelection->onMouseUp(pCurrentMousePositionGraph.x,
                                               pCurrentMousePositionGraph.y))
        {
            pSelectionBox.second.x = 0;
            pSelectionBox.second.y = 0;
            removeTools(Tool::lifeSpanMouseSelection);
            pMouseAction = mouseActionNone;
            pLastSelectedTool = nullptr;
            refresh();
        }
        break;
    }
    case WXK_CONTROL:
    {
        pKeyCtrl = true;
        break;
    }
    default:
    {
        if (keyCode >= 256 || keyCode < 0)
            break;
        if (pKeyCtrl)
        {
            switch (keyCode)
            {
            case 'c':
            case 'C':
            {
                // The main form
                logs.debug() << "[map] 'ctrl+c': copy to clipboard";
                auto* mainFrm = Antares::Forms::ApplWnd::Instance();
                if (mainFrm)
                    mainFrm->copyToClipboard();
                break;
            }
            case 'v':
            case 'V':
            {
                // The main form
                logs.debug() << "[map] 'ctrl+v': paste from clipboard";
                auto* mainFrm = Antares::Forms::ApplWnd::Instance();
                if (mainFrm)
                    mainFrm->pasteFromClipboard();
                break;
            }
            }
            break;
        }
        if (pKeyShift)
            break;
        switch (keyCode)
        {
        case 'n':
        case 'N':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewNodeWhereTheMouseIs);
            break;
        }
        case 'l':
        case 'L':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewLoadNodeWhereTheMouseIs);
            break;
        }
        case 't':
        case 'T':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewThermalNodeWhereTheMouseIs);
            break;
        }
        case 'u':
        case 'U':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewNuclearNodeWhereTheMouseIs);
            break;
        }
        case 'g':
        case 'G':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewCCGTNodeWhereTheMouseIs);
            break;
        }
        case 'c':
        case 'C':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewCoalNodeWhereTheMouseIs);
            break;
        }
        case 'o':
        case 'O':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewOilNodeWhereTheMouseIs);
            break;
        }
        case 'h':
        case 'H':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewHydroNodeWhereTheMouseIs);
            break;
        }
        case 'p':
        case 'P':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewPSPPNodeWhereTheMouseIs);
            break;
        }
        case 'r':
        case 'R':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewRENWNodeWhereTheMouseIs);
            break;
        }
        case 'w':
        case 'W':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewWindNodeWhereTheMouseIs);
            break;
        }
        case 's':
        case 'S':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewSolarNodeWhereTheMouseIs);
            break;
        }
        case 'b':
        case 'B':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewBioMNodeWhereTheMouseIs);
            break;
        }
        case 'e':
        case 'E':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewVoidNodeWhereTheMouseIs);
            break;
        }
        case 'v':
        case 'V':
        {
            logs.debug() << "[map] 'n': creating new node";
            Dispatcher::GUI::Post(this, &Control::addNewVirtualNodeWhereTheMouseIs);
            break;
        }
        }
    }
    }
    evt.Skip();
}

void Control::keyChar(wxKeyEvent& evt)
{
    // Do nothing
    evt.Skip();
}

void Control::keyReleased(wxKeyEvent& evt)
{
    const long keyCode = evt.GetKeyCode();
    switch (keyCode)
    {
    case WXK_SHIFT:
    {
        pKeyShift = false;
        break;
    }
    case WXK_CONTROL:
    {
        pKeyCtrl = false;
        break;
    }
    default:
    {
    }
    }

    evt.Skip();
}

void Control::mouseDblClick(wxMouseEvent&)
{
    if ((pLastSelectedTool = findTool(pCurrentMousePosition.x, pCurrentMousePosition.y)))
    {
        // Do nothing
    }
    else
    {
        // Looking for a node
        Item* item = nodes.find(pCurrentMousePositionGraph.x, pCurrentMousePositionGraph.y);
        if (item)
        {
            item->mouseDblClick();
            pComponent.onDblClick(pComponent);
        }
    }
}

uint Control::areasCount() const
{
    return nodes.areasCount(this->getUid());
}

uint Control::connectionsCount() const
{
    return nodes.connectionsCount(this->getUid());
}

void Control::onScroll(wxScrollWinEvent& evt)
{
    pInvalidatedInnerCacheSize = true;
    pInvalidated = true;
    pComponent.Refresh();
    evt.Skip();
}

void Control::onSize(wxSizeEvent& evt)
{
    pInvalidatedInnerCacheSize = true;
    pInvalidated = true;
    pComponent.Refresh();
    evt.Skip();
}

size_t Control::newUID = 0;

} // namespace Map
} // namespace Antares

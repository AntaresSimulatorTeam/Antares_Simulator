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

#include "node.h"
#include "../manager.h"
#include "../settings.h"
#include "../../../../application/study.h"
#include "../../../../windows/inspector.h"
#include <yuni/core/math.h>

#define NODE_DRAW_COLOR_VARIATION_LIGHT 30
#define NODE_DRAW_COLOR_VARIATION_DARK 15

using namespace Yuni;

namespace Antares
{
namespace Map
{
enum // constants
{
    nodeDrawBorderspaceX = 8,
    nodeDrawBorderspaceY = 1,
};
enum
{
    fontSize = 8
};
static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));

template<typename T>
static inline bool IsInRange(const T value, const T min, const T max)
{
    return (value >= min) && (value <= max);
}

Node::Node(Manager& manager) : Item(manager, 100), pId(), pAttachedArea(nullptr)
{
}

Node::Node(Manager& manager, const wxString& id) :
 Item(manager, 100), pId(id), pAttachedArea(nullptr)
{
}

void Node::createANewAreaIfNotAlreadyAttached()
{
    auto study = pManager.study();
    if (!(!study) && not pCaption.IsEmpty())
    {
        // Creating a new Area
        Data::AreaName sFl;
        wxStringToString(pCaption, sFl);
        pAttachedArea = study->areaAdd(sFl);
        pCaption = wxStringFromUTF8(pAttachedArea->name);
        study->uiinfo->reload();
        MarkTheStudyAsModified();
        GUIFlagInvalidateAreas = true;
        OnStudyAreaAdded(pAttachedArea);
    }
}

Node::~Node()
{
    auto study = pManager.study();

    // Force its deselection to update counters
    this->selected(false);
    // The connections must be removed before the area itself
    internalClearAllLinks();
    // Notify the manager that we are dead, if it does not already know
    pManager.internalRemoveItem(this);

    if (!(!study) && pAttachedArea)
    {
        pAttachedArea->ui->mapLayersVisibilityList.clear();
        pManager.pendingDeleteArea(pAttachedArea);
        pAttachedArea = nullptr;
    }
}

//! \name layerVisibility
//@{
/*!
** \brief Get the visivility for a layerId
*/
bool Node::isVisibleOnLayer(const size_t& layerID) const
{
    if (pAttachedArea == nullptr)
        return false;
    return pAttachedArea->isVisibleOnLayer(layerID);
}
//@}

void Node::refreshCache(wxDC& dc)
{
    dc.SetFont(font);
    if (pAttachedArea && isVisibleOnLayer(pManager.getActiveLayerId()))
    {
        size_t activeLayerId = pManager.getActiveLayerId();
        Data::AreaUI& ui = *(pAttachedArea->ui);
        if (ui.layerX.find(activeLayerId) != ui.layerX.end()
            && ui.layerY.find(activeLayerId) != ui.layerY.end()
            && ui.layerColor.find(activeLayerId) != ui.layerColor.end())
        {
            if (pX != ui.layerX[activeLayerId] || pY != ui.layerY[activeLayerId]
                || (int)pColor.Red() != ui.layerColor[activeLayerId][0])
            {
                pX = ui.x = ui.layerX[activeLayerId];
                pY = ui.y = ui.layerY[activeLayerId];

                ui.color[0] = ui.layerColor[activeLayerId][0];
                ui.color[1] = ui.layerColor[activeLayerId][1];
                ui.color[2] = ui.layerColor[activeLayerId][2];
                pColor.Set((unsigned char)Yuni::Math::MinMax<int>(ui.color[0], 0, 255),
                           (unsigned char)Yuni::Math::MinMax<int>(ui.color[1], 0, 255),
                           (unsigned char)Yuni::Math::MinMax<int>(ui.color[2], 0, 255),
                           wxALPHA_OPAQUE);
                // Refresh the inspector
                if (Window::Inspector::SelectionAreaCount() == 1)
                {
                    // It is useless to perform a costly refresh when several areas
                    // are selected. In this case the coordinates are not displayed
                    Window::Inspector::Refresh();
                }
                OnLayerNodeUIChanged();
            }
        }
        else
        {
            ui.layerX[activeLayerId] = ui.x;
            ui.layerY[activeLayerId] = ui.y;
            ui.layerColor[activeLayerId][0] = (int)pColor.Red();
            ui.layerColor[activeLayerId][1] = (int)pColor.Green();
            ui.layerColor[activeLayerId][2] = (int)pColor.Blue();
        }
    }

    const wxSize size = dc.GetTextExtent(pCaption);
    // Size
    pCachedSize.x = size.GetWidth() + nodeDrawBorderspaceX * 2;
    pCachedSize.y = size.GetHeight() + nodeDrawBorderspaceY * 2;
    // Position
    pCachedPosition.x = pX - (pCachedSize.x >> 1);
    pCachedPosition.y = pY - (pCachedSize.y >> 1);

    double y = pColor.Red() * 0.299 + pColor.Green() * 0.587 + pColor.Blue() * 0.114;
    double u = (pColor.Blue() - y) * 0.565;
    double v = (pColor.Red() - y) * 0.713;
    if (y < 50.)
        y = 50.;
    else
    {
        if (y > 225.)
            y = 225.;
    }

    const int r = Math::MinMax<int>((int)(y + 1.403 * v), 0, 255);
    const int g = Math::MinMax<int>((int)(y - 0.344 * u - 0.714 * v), 0, 255);
    const int b = Math::MinMax<int>((int)(y + 1.770 * u), 0, 255);

    pCachedColorGradientStart.Set(
      (unsigned char)Math::MinMax<int>(r + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255),
      (unsigned char)Math::MinMax<int>(g + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255),
      (unsigned char)Math::MinMax<int>(b + NODE_DRAW_COLOR_VARIATION_LIGHT / 2, 0, 255));
    pCachedColorGradientEnd.Set(
      (unsigned char)Math::MinMax<int>(r - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255),
      (unsigned char)Math::MinMax<int>(g - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255),
      (unsigned char)Math::MinMax<int>(b - NODE_DRAW_COLOR_VARIATION_DARK / 2, 0, 255));
    pCachedColorGradientStart2.Set(
      (unsigned char)Math::MinMax<int>(r + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255),
      (unsigned char)Math::MinMax<int>(g + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255),
      (unsigned char)Math::MinMax<int>(b + NODE_DRAW_COLOR_VARIATION_LIGHT * 2, 0, 255));
    pCachedColorGradientEnd2.Set(
      (unsigned char)Math::MinMax<int>(r - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255),
      (unsigned char)Math::MinMax<int>(g - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255),
      (unsigned char)Math::MinMax<int>(b - NODE_DRAW_COLOR_VARIATION_DARK, 0, 255));

    double yy;
    yy = (y <= 147.) ? 255. : 60.;
    pCachedColorText.Set(
      (unsigned char)Math::MinMax<int>((int)(yy + 1.403 * v), 0, 255),
      (unsigned char)Math::MinMax<int>((int)(yy - 0.344 * u - 0.714 * v), 0, 255),
      (unsigned char)Math::MinMax<int>((int)(yy + 1.770 * u), 0, 255));

    // Border color
    wxColour border;
    border.Set((unsigned char)Math::MinMax<int>(r - NODE_DRAW_COLOR_VARIATION_DARK * 2, 0, 255),
               (unsigned char)Math::MinMax<int>(g - NODE_DRAW_COLOR_VARIATION_DARK * 2, 0, 255),
               (unsigned char)Math::MinMax<int>(b - NODE_DRAW_COLOR_VARIATION_DARK * 2, 0, 255));

    pBorderPen.SetColour(border); // pColor);
    // Shadow pen
    if (pSelected)
        pShadowPen.SetColour(Settings::selectionNodeBorder);
    else
        pShadowPen.SetColour(Settings::defaultNodeShadow);

    pInvalidated = false;
}

bool Node::isContained(const int x1, const int y1, const int x2, const int y2) const
{
    if (!pInvalidated)
    {
        bool xOverlap = IsInRange<int>(pCachedPosition.x, x1, x2)
                        or IsInRange<int>(x1, pCachedPosition.x, pCachedPosition.x + pCachedSize.x);
        bool yOverlap = IsInRange<int>(pCachedPosition.y, y1, y2)
                        or IsInRange<int>(y1, pCachedPosition.y, pCachedPosition.y + pCachedSize.y);
        return xOverlap && yOverlap;
    }
    return false;
}

bool Node::contains(const int x, const int y, double& distance)
{
    if (!pInvalidated)
        if (x >= pCachedPosition.x && x <= pCachedPosition.x + pCachedSize.x
            && y >= pCachedPosition.y && y <= pCachedPosition.y + pCachedSize.y)
        {
            distance = 0;
            return true;
        }
    return false;
}

void Node::position(const int x, const int y)
{
    pX = x;
    pY = y;
    pInvalidated = true;
    positionHasChanged();
}

void Node::move(const int x, const int y)
{
    if (x || y)
    {
        pX += x;
        pY += y;
        pInvalidated = true;
        positionHasChanged();
    }
}

void Node::draw(DrawingContext& dc)
{
    if (!isVisibleOnLayer(dc.getLayerId()))
        return;

    // The device context
    wxDC& device = dc.device();
    refreshCache(device);
    wxPoint rTopLeft(dc.origin().x + pCachedPosition.x,
                     dc.origin().y - pCachedPosition.y - pCachedSize.y);
    wxPoint rCenter = this->absolutePosition(dc);
    if (pSelected)
    {
        rTopLeft.x += dc.offsetForSelectedNodes().x;
        rTopLeft.y += dc.offsetForSelectedNodes().y;
        rCenter.x += dc.offsetForSelectedNodes().x;
        rCenter.y += dc.offsetForSelectedNodes().y;
    }

    // Do not draw nodes outside the current view port
    if (rTopLeft.x < dc.scroll().x - pCachedSize.x
        || rTopLeft.x > dc.bottomRight().x + pCachedSize.x)
        return;
    if (rTopLeft.y < dc.scroll().y - pCachedSize.y
        || rTopLeft.y > dc.bottomRight().y + pCachedSize.y)
        return;

    // Background
    // Shadow
    device.SetPen(pShadowPen);
    device.SetBrush(wxBrush(Settings::selectionNodeBackground));
    if (pSelected)
        device.DrawRoundedRectangle(
          rTopLeft.x - 3, rTopLeft.y - 3, pCachedSize.x + 6, pCachedSize.y + 6, 2);
    else
        device.DrawRectangle(rTopLeft.x, rTopLeft.y, pCachedSize.x + 2, pCachedSize.y + 2);

    // Border
    device.SetPen(pBorderPen);
    device.DrawRectangle(rTopLeft.x - 1, rTopLeft.y - 1, pCachedSize.x + 2, pCachedSize.y + 2);
    // Gradient inside
    device.GradientFillLinear(wxRect(rTopLeft.x, rTopLeft.y, pCachedSize.x, pCachedSize.y / 2),
                              pCachedColorGradientStart,
                              pCachedColorGradientEnd,
                              wxSOUTH);
    device.GradientFillLinear(
      wxRect(rTopLeft.x, rTopLeft.y + pCachedSize.y / 2, pCachedSize.x, pCachedSize.y / 2 + 1),
      pCachedColorGradientEnd2,
      pCachedColorGradientStart2,
      wxSOUTH);

    // Draw the text
    if (!pCaption.empty())
    {
        device.SetFont(font);

        rTopLeft.x += nodeDrawBorderspaceX; // Offset for the text
        rTopLeft.y += nodeDrawBorderspaceY;

        // Text itself
        device.SetTextForeground(pCachedColorText);
        device.DrawText(pCaption, rTopLeft.x, rTopLeft.y);
    }
}

void Node::extendBoundingBox(wxPoint& topLeft, wxPoint& bottomRight)
{
    const wxPoint p(pCachedPosition.x, pCachedPosition.y + pCachedSize.y);
    if (topLeft.x > p.x)
        topLeft.x = p.x;
    if (topLeft.y < p.y)
        topLeft.y = p.y;
    const int px = pCachedSize.x + p.x;
    const int py = -pCachedSize.y + p.y;
    if (bottomRight.x < px)
        bottomRight.x = px;
    if (bottomRight.y > py)
        bottomRight.y = py;
}

void Node::addLayerVisibility(size_t id)
{
    std::vector<size_t>& layerList = pAttachedArea->ui->mapLayersVisibilityList;
    std::vector<size_t>::iterator layerPosition = std::find(layerList.begin(), layerList.end(), id);
    if (layerPosition == layerList.end())
        layerList.push_back(id);
    pAttachedArea->ui->markAsModified();
}

void Node::removeLayerVisibility(size_t id)
{
    pAttachedArea->ui->layerColor.erase(id);
    std::vector<size_t>& layerList = pAttachedArea->ui->mapLayersVisibilityList;
    std::vector<size_t>::iterator layerPosition = std::find(layerList.begin(), layerList.end(), id);
    if (layerPosition != layerList.end())
        layerList.erase(layerPosition);
    pAttachedArea->ui->markAsModified();
}

void Node::captionHasChanged()
{
    // deprecated code

    if (!(!pManager.study()) && pAttachedArea)
    {
        // Renmamming of the area
        Data::AreaName newName;
        wxStringToString(pCaption, newName);
        pManager.study()->areaRename(pAttachedArea, newName);
        MarkTheStudyAsModified();
        GUIFlagInvalidateAreas = true;
    }
    else
    {
        if (!pAttachedArea)
            createANewAreaIfNotAlreadyAttached();
    }
}

void Node::positionHasChanged()
{
    if (pAttachedArea)
    {
        Data::AreaUI& ui = *(pAttachedArea->ui);
        if (ui.x != pX || ui.y != pY)
        {
            ui.layerX[pManager.getActiveLayerId()] = ui.x = pX;
            ui.layerY[pManager.getActiveLayerId()] = ui.y = pY;

            ui.markAsModified();
            MarkTheStudyAsModified();

            // Refresh the inspector
            if (Window::Inspector::SelectionAreaCount() == 1)
            {
                // It is useless to perform a costly refresh when several areas
                // are selected. In this case the coordinates are not displayed
                Window::Inspector::Refresh();
            }
        }
    }
}

void Node::colorHasChanged()
{
    if (pAttachedArea)
    {
        Data::AreaUI& ui = *(pAttachedArea->ui);
        ui.layerColor[pManager.getActiveLayerId()][0] = ui.color[0] = (int)pColor.Red();
        ui.layerColor[pManager.getActiveLayerId()][1] = ui.color[1] = (int)pColor.Green();
        ui.layerColor[pManager.getActiveLayerId()][2] = ui.color[2] = (int)pColor.Blue();
        ui.markAsModified();
        ui.rebuildCache();
        GUIFlagInvalidateAreas = true;
    }
}

} // namespace Map
} // namespace Antares

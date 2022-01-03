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
#include "connection.h"
#include "../settings.h"
#include "../tools/remover.h"
#include "../../../../application/study.h"

#include <yuni/core/math.h>
#include <yuni/core/math/distance.h>

#include <antares/study/scenario-builder/updater.hxx>

using namespace Yuni;

namespace Antares
{
namespace Map
{
namespace // anonymous
{
enum
{
    fontSize = 7,
    connectMouseSensibility = 3,
};
static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));
}; // anonymous namespace

static inline wxPoint ComputeBarycentre(const wxPoint& a,
                                        const wxPoint& b,
                                        const int coeff1,
                                        const int coeff2)
{
    if (!coeff1 && !coeff2)
        return wxPoint();
    return wxPoint((coeff1 * a.x + coeff2 * b.x) / (coeff1 + coeff2),
                   (coeff1 * a.y + coeff2 * b.y) / (coeff1 + coeff2));
}

/*!
** \brief Compute the distance from a point to a segment
*/
static int DistanceBetweenAPointAndASegment(const double pointX,
                                            const double pointY,
                                            const double x1,
                                            const double y1,
                                            const double x2,
                                            const double y2)
{
    double diffX = x2 - x1;
    double diffY = y2 - y1;
    if ((diffX == 0) && (diffY == 0))
    {
        diffX = pointX - x1;
        diffY = pointY - y1;
        return sqrt(diffX * diffX + diffY * diffY);
    }

    double t = ((pointX - x1) * diffX + (pointY - y1) * diffY) / (diffX * diffX + diffY * diffY);

    if (t < 0)
    {
        // point is nearest to the first point i.e x1 and y1
        diffX = pointX - x1;
        diffY = pointY - y1;
    }
    else if (t > 1)
    {
        // point is neare-30st to the end point i.e x2 and y2
        diffX = pointX - x2;
        diffY = pointY - y2;
    }
    else
    {
        // if perpendicular line intersect the line segment.
        diffX = pointX - (x1 + t * diffX);
        diffY = pointY - (y1 + t * diffY);
    }

    // returning shortest distance
    return (int)sqrt(diffX * diffX + diffY * diffY);
}

Connection::Connection(Manager& manager, Item* a, Item* b) :
 Item(manager),
 pA(nullptr),
 pB(nullptr),
 pDirection(dirDirect),
 pArrowDirection(dirNone),
 pCachedAbsolutePosition(0, 0),
 pHaveTools(false),
 pAttachedAreaLink(nullptr),
 pDefaultPen(wxPen(wxColour(225, 225, 225), 6, wxPENSTYLE_SOLID)),
 pErrorPen(wxPen(wxColour(255, 200, 200), 6, wxPENSTYLE_SOLID))
{
    if (a && b)
    {
        if (a->pCaption.Lower() > b->pCaption.Lower())
        {
            pA = a;
            pB = b;
        }
        else
        {
            pA = b;
            pB = a;
        }
    }
    if (pA)
        pA->pLinks->insert(std::pair<Item*, bool>(this, true));
    if (pB)
        pB->pLinks->insert(std::pair<Item*, bool>(this, true));

    // apply the standard color for this type of item
    this->color(Settings::connection);
}

Connection::~Connection()
{
    // Remove the link if not already done
    if (pManager.study() && pAttachedAreaLink)
        pManager.pendingDeleteLink(pAttachedAreaLink);
    pAttachedAreaLink = nullptr;

    this->selected(false);
    pManager.internalRemoveItem(this);

    internalClearAllLinks();

    if (pA && pA->pLinks)
    {
        Links::iterator i = pA->pLinks->find(this);
        if (i != pA->pLinks->end())
            pA->pLinks->erase(i);
    }
    if (pB && pB->pLinks)
    {
        Links::iterator i = pB->pLinks->find(this);
        if (i != pB->pLinks->end())
            pB->pLinks->erase(i);
    }
}

//! \name layerVisibility
//@{
/*!
** \brief Get the visivility for a layerId
*/
const bool Connection::isVisibleOnLayer(const size_t& layerID) const
{
    return pA->isVisibleOnLayer(layerID) && pB->isVisibleOnLayer(layerID);
}
//@}

void Connection::arrowDirection(const Connection::Direction& d)
{
    pArrowDirection = d;
    MarkTheStudyAsModified();
    invalidate();
}

void Connection::direction(const Connection::Direction& d)
{
    pDirection = d;
    MarkTheStudyAsModified();
    invalidate();
    GUIFlagInvalidateAreas = true;
}

void Connection::refreshCache(wxDC& dc)
{
    // The new comments
    if (pAttachedAreaLink && pAttachedAreaLink->displayComments)
        pCaption = wxStringFromUTF8(pAttachedAreaLink->comments);
    else
        pCaption.clear();

    // Detecting bad line orientation
    pCachedError = (pAttachedAreaLink && pAttachedAreaLink->from->id > pAttachedAreaLink->with->id);

    pCachedAbsolutePosition.x = 0;
    pCachedAbsolutePosition.y = 0;
    if (!pCaption.IsEmpty())
    {
        dc.SetFont(font);

        const wxSize size = dc.GetTextExtent(pCaption);
        // Size
        pCachedSize.x = size.GetWidth() + 6;
        pCachedSize.y = size.GetHeight();
    }

    if (pA && pB)
    {
        if (pA->x() < pB->x())
            pCachedMiddlePoint.x = (pB->x() - pA->x()) / 2;
        else
            pCachedMiddlePoint.x = (pA->x() - pB->x()) / 2;

        if (pA->y() < pB->y())
            pCachedMiddlePoint.y = (pB->y() - pA->y()) / 2;
        else
            pCachedMiddlePoint.y = (pA->y() - pB->y()) / 2;
    }

    pInvalidated = false;
}

void Connection::createANewConnectionIfNeeded()
{
    if (!pAttachedAreaLink && pA && pB)
    {
        auto study = pManager.study();
        if (!study)
            return;

        const Node* aa = dynamic_cast<Node*>(pA);
        const Node* bb = dynamic_cast<Node*>(pB);
        if (aa && bb && aa->attachedArea() && bb->attachedArea())
        {
            ScenarioBuilderUpdater updaterSB(*study);
            // Making a copy to avoid unexpected behavior when working on
            // the same string.
            const Data::AreaName sA = aa->attachedArea()->id;
            const Data::AreaName sB = bb->attachedArea()->id;
            if (sA < sB)
                pAttachedAreaLink = Data::AreaListAddLink(&study->areas, sA.c_str(), sB.c_str());
            else
                pAttachedAreaLink = Data::AreaListAddLink(&study->areas, sB.c_str(), sA.c_str());

            // Force the refresh of runtime data
            logs.debug() << "  Asking to reload UI runtime data";
            auto* info = Data::Study::Current::Get()->uiinfo;
            if (info)
            {
                info->reload();
            }

            pAttachedAreaLink->resetToDefaultValues();
            OnStudyLinkAdded(pAttachedAreaLink);
            study->uiinfo->reload();
            MarkTheStudyAsModified();
            GUIFlagInvalidateAreas = true;
        }
    }
}

bool Connection::contains(const int x, const int y, double& distance)
{
    return (pA && pB)
           && (distance
               = (DistanceBetweenAPointAndASegment(x, y, pA->x(), pA->y(), pB->x(), pB->y()))
                 < connectMouseSensibility);
}

bool Connection::isContained(const int, const int, const int, const int) const
{
    // A connection should never be directly highlighted when selected an entire zone
    // wit the mouse
    return false;
}

void Connection::drawArrow(wxDC& dc,
                           const Connection::Direction direction,
                           const wxPoint& rA,
                           const wxPoint& rB)
{
    wxPoint b1;
    wxPoint b2;
    wxPoint b3;
    if (direction == dirDirect)
    {
        b1 = ComputeBarycentre(rA, rB, 20, 40);
        b2 = ComputeBarycentre(rA, rB, 12, 40);
        b3 = ComputeBarycentre(rA, rB, 70, 40);
    }
    else
    {
        b1 = ComputeBarycentre(rB, rA, 20, 40);
        b2 = ComputeBarycentre(rB, rA, 12, 40);
        b3 = ComputeBarycentre(rB, rA, 70, 40);
    }

    const float nWidth = 9.f;
    const float fTheta = 0.68f;

    // set to point
    wxPoint pCachedArrow[3];
    pCachedArrow[0].x = b1.x;
    pCachedArrow[0].y = b1.y;

    // build the line vector
    float vecLine[2];
    vecLine[0] = (float)pCachedArrow[0].x - b2.x;
    vecLine[1] = (float)pCachedArrow[0].y - b2.y;

    // build the arrow base vector - normal to the line
    float vecLeft[2];
    vecLeft[0] = -vecLine[1];
    vecLeft[1] = vecLine[0];

    // setup length parameters
    const float fLength = (float)sqrt(vecLine[0] * vecLine[0] + vecLine[1] * vecLine[1]);
    const float th = nWidth / (2.0f * fLength);
    const float ta = nWidth / (2.0f * (tanf(fTheta) / 2.0f) * fLength);

    // find the base of the arrow
    const wxPoint pBase((int)(pCachedArrow[0].x + -ta * vecLine[0]),
                        (int)(pCachedArrow[0].y + -ta * vecLine[1]));

    // build the points on the sides of the arrow
    pCachedArrow[1].x = (int)(pBase.x + th * vecLeft[0]) + 1;
    pCachedArrow[1].y = (int)(pBase.y + th * vecLeft[1]) + 1;
    pCachedArrow[2].x = (int)(pBase.x + -th * vecLeft[0]) + 1;
    pCachedArrow[2].y = (int)(pBase.y + -th * vecLeft[1]) + 1;

    dc.SetBrush(wxBrush(wxColour(0, 0, 0)));
    dc.SetPen(wxPen(wxColour(0, 0, 0)));
    dc.DrawPolygon(3, pCachedArrow);
    dc.DrawPolygon(3, pCachedArrow, b3.x - b2.x, b3.y - b2.y);
}

void Connection::draw(DrawingContext& dc)
{
    if (!pA || !pB || !isVisibleOnLayer(dc.getLayerId()))
        return;

    // Position for the first node
    wxPoint rCenterA(dc.origin().x + pA->pX, dc.origin().y - pA->pY);
    if (pA->selected())
    {
        rCenterA.x += dc.offsetForSelectedNodes().x;
        rCenterA.y += dc.offsetForSelectedNodes().y;
    }
    // Position for the second node
    wxPoint rCenterB(dc.origin().x + pB->pX, dc.origin().y - pB->pY);
    if (pB->selected())
    {
        rCenterB.x += dc.offsetForSelectedNodes().x;
        rCenterB.y += dc.offsetForSelectedNodes().y;
    }

    // Checking if one of the nedpoints is in the viewport, trivial accept.
    if (!((rCenterA.x > dc.scroll().x && rCenterA.x < dc.bottomRight().x)
          || (rCenterB.x > dc.scroll().x && rCenterB.x < dc.bottomRight().x)
          || (rCenterA.y > dc.scroll().y && rCenterA.y < dc.bottomRight().y)
          || (rCenterB.y > dc.scroll().y && rCenterB.y < dc.bottomRight().y)))
    {
        // None of the points are in the viewport
        // We have to determine with a better algorithm if the segment must be
        // displayed or not

        // Checking if both endpoints are not visible
        if ((rCenterA.x < dc.scroll().x && rCenterB.x < dc.scroll().x)
            || (rCenterA.y < dc.scroll().y && rCenterB.y < dc.scroll().y)
            || (rCenterA.x > dc.bottomRight().x && rCenterB.x > dc.bottomRight().x)
            || (rCenterA.y > dc.bottomRight().y && rCenterB.y > dc.bottomRight().y))
        {
            // Trivial reject
            return;
        }

        // For better line clipping, we could use the Liang-Barsky algorithm
        // but it will be far good enough for now.
    }

    // The device context
    wxDC& device = dc.device();

    // Drawing of the shadow of the line
    if (not dc.isForFileExport())
    {
        device.SetPen(pCachedError ? pErrorPen : pDefaultPen);
        device.DrawLine(rCenterA.x, rCenterA.y, rCenterB.x, rCenterB.y);
    }

    wxColor personalColor(
      pAttachedAreaLink->color[0], pAttachedAreaLink->color[1], pAttachedAreaLink->color[2]);
    // The line itself
    int penWidth = pAttachedAreaLink->linkWidth;
    wxPenStyle myStyle;
    switch (pAttachedAreaLink->style)
    {
    case Data::stPlain:
        myStyle = wxPENSTYLE_SOLID;
        break;
    case Data::stDot:
        myStyle = wxPENSTYLE_DOT;
        break;
    case Data::stDash:
        myStyle = wxPENSTYLE_LONG_DASH;
        break;
    case Data::stDotDash:
        myStyle = wxPENSTYLE_DOT_DASH;
        break;
    default:
        myStyle = wxPENSTYLE_SOLID;
    }
    if (pSelected)
        device.SetPen(wxPen(Settings::connectionHighlighted,
                            penWidth,
                            (pDirection == dirIndirect ? wxPENSTYLE_LONG_DASH : myStyle)));
    else
    {
        wxColor personalColor(
          pAttachedAreaLink->color[0], pAttachedAreaLink->color[1], pAttachedAreaLink->color[2]);
        device.SetPen(wxPen(
          personalColor, penWidth, (pDirection == dirIndirect ? wxPENSTYLE_LONG_DASH : myStyle)));
    }
    // Draw the line
    device.DrawLine(rCenterA.x, rCenterA.y, rCenterB.x, rCenterB.y);

    if (!pCaption.empty())
    {
        device.SetFont(font);

        // Position
        pCachedAbsolutePosition.x
          = rCenterA.x + (rCenterB.x - rCenterA.x) / 2 - pCachedSize.x / 2 + 4;
        pCachedAbsolutePosition.y
          = rCenterA.y + (rCenterB.y - rCenterA.y) / 2 - pCachedSize.y / 2 + 2;

        // Draw the text
        // Its background first
        device.SetPen(wxPen(Settings::background, 1, wxPENSTYLE_SOLID));
        device.SetBrush(wxBrush(Settings::background));
        device.DrawRectangle(wxRect(pCachedAbsolutePosition, wxSize(pCachedSize.x, pCachedSize.y)));
        // The text itself
        device.SetTextForeground(Settings::textLight);
        device.DrawText(pCaption, pCachedAbsolutePosition.x + 3, pCachedAbsolutePosition.y);
    }

    if (pArrowDirection != dirNone)
        drawArrow(device, pArrowDirection, rCenterA, rCenterB);
}

void Connection::extendBoundingBox(wxPoint& topLeft, wxPoint& bottomRight)
{
    if (pA)
        pA->extendBoundingBox(topLeft, bottomRight);
    if (pB)
        pB->extendBoundingBox(topLeft, bottomRight);
}

void Connection::destroyTools()
{
    if (pHaveTools)
    {
        delete pToolDirection;
        pToolDirection = nullptr;
        pHaveTools = false;
    }
}

void Connection::createTools()
{
    if (pHaveTools)
        destroyTools();
    pToolDirection = new Tool::Remover(pManager);
    pToolDirection->x(pCachedAbsolutePosition.x);
    pToolDirection->y(pCachedAbsolutePosition.y);
    pHaveTools = true;
}

void Connection::selected(bool v)
{
    if (v != pSelected)
    {
        pSelected = v;
        if (v)
        {
            pManager.pSelectedItems[this] = true;
            ++pManager.pSelectedItemsAsConnection;
            createTools();
        }
        else
        {
            destroyTools();
            --pManager.pSelectedItemsAsConnection;
            auto i = pManager.pSelectedItems.find(this);
            if (i != pManager.pSelectedItems.end())
                pManager.pSelectedItems.erase(i);
        }
        invalidate();
    }
}

void Connection::attachedAreaLink(Data::AreaLink* a)
{
#ifndef NDEBUG
    if (Logs::Verbosity::Debug::enabled)
    {
        // consistency check
        assert(pA && "The left side must be attached to a node");
        assert(pB && "The right side must be attached to a node");
        assert(a && "The given pointer is null");

        Node* nA = dynamic_cast<Node*>(pA);
        Node* nB = dynamic_cast<Node*>(pB);
        assert(nA && "The left side does not seem to be an object of type Node");
        assert(nB && "The right side does not seem to be an object of type Node");
        assert(a->from == nA->attachedArea() || a->from == nB->attachedArea());
        assert(a->with == nA->attachedArea() || a->with == nB->attachedArea());
    }
#endif
    pAttachedAreaLink = a;
}

} // namespace Map
} // namespace Antares

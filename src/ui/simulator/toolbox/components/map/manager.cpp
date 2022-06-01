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

#include "manager.h"
#include <limits.h>
#include "nodes/connection.h"
#include "component.h"
#include <antares/utils.h>
#include "../../../application/main/main.h"
#include "../../../application/study.h"
#include "../../../windows/inspector.h"
#include "../../clipboard/clipboard.h"

using namespace Yuni;

namespace Antares
{
namespace Map
{
//! Global instance for the main form
static Manager* gInstance = nullptr;

Manager* Manager::Instance()
{
    assert(gInstance != nullptr
           && "No manager Instance was created. Accessing this object will result into error");
    return gInstance;
}

Manager::Manager(Component& component) :
 mouseSelectionArea(true),
 mouseSelectionLinks(true),
 mouseSelectionPlants(false),
 mouseSelectionConstraints(true),
 pComponent(component),
 pStudy(nullptr),
 pAllNodes(),
 pStackDeallocation(),
 pSelectedItemsAsConnection(0),
 pExternalQueue(),
 pHasChanges(true),
 pUpdaterLock(0)
{
    // Setting the global instance
    gInstance = this;
}

Manager::~Manager()
{
    deleteAllPendingData();
    // Avoid useless redrawing
    ++pUpdaterLock;

    // Detach the study to avoid changes
    pStudy = nullptr;
    // Clear all children
    clear();
}

void Manager::attachStudy(Data::Study::Ptr study)
{
    pStudy = study;
}

bool Manager::loadFromStudy(Data::Study& study)
{
    // Saving the attached study
    // The study must be detached to avoid any changes to the study
    Data::Study::Ptr attachedStudy = pStudy;
    pStudy = nullptr;

    // Remove all existing nodes
    clear();

    // An hash map to quickly find the previous node
    using NodeFromID = std::map<Data::AreaName, Node*>;
    NodeFromID nodeFromID;

    // Node creation
    // Browsing all areas in the study
    study.areas.each([&](Data::Area& area) {
        // The new node
        Node* node = this->addNode(
          wxStringFromUTF8(area.id), wxStringFromUTF8(area.name), area.ui->x, area.ui->y);
        // We have to attach the area to the node to perform further updates
        node->attachedArea(&area);
        // Color of the node
        node->color(wxColour(area.ui->color[0], area.ui->color[1], area.ui->color[2]));
        // Keep a reference to the node
        nodeFromID.insert(std::pair<Data::AreaName, Node*>(area.id, node));
    });

    // Interconnections
    const NodeFromID::iterator endNodeID = nodeFromID.end();
    study.areas.each([&](Data::Area& area) {
        const auto end = area.links.end();
        for (auto i = area.links.begin(); i != end; ++i)
        {
            Data::AreaLink* lnk = i->second;

            auto a = nodeFromID.find(area.id);
            auto b = nodeFromID.find(lnk->with->id);
            if (a != endNodeID and b != endNodeID)
            {
                auto* connection = this->addConnection(a->second, b->second);
                if (connection)
                    connection->attachedAreaLink(lnk);
                else
                    logs.error() << "impossible to add a connection";
            }
        }
    });

    // Restoring the previous state of the attached study
    pStudy = attachedStudy;

    return true;
}

BoundingBox Manager::boundingBox(const size_t& layerID) const
{
    wxPoint topLeft(INT_MAX, INT_MIN);
    wxPoint bottomRight(INT_MIN, INT_MAX);

    NodeByZPosition::const_iterator end = pAllNodes.end();
    for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
    {
        NodeMap::const_iterator jEnd = i->second.end();
        for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
            if (j->first->isVisibleOnLayer(layerID))
                j->first->extendBoundingBox(topLeft, bottomRight);
    }
    return BoundingBox(topLeft, bottomRight);
}

BoundingBox Manager::boundingBoxOfSelectedNodes(const size_t& layerID) const
{
    wxPoint topLeft(INT_MAX, INT_MIN);
    wxPoint bottomRight(INT_MIN, INT_MAX);

    if (!pSelectedItems.empty())
    {
        NodeMap::const_iterator end = pSelectedItems.end();
        for (NodeMap::const_iterator j = pSelectedItems.begin(); j != end; ++j)
        {
            if (j->first->isVisibleOnLayer(layerID))
                j->first->extendBoundingBox(topLeft, bottomRight);
        }
    }
    return BoundingBox(topLeft, bottomRight);
}

Node* Manager::addNode(const wxString& id)
{
    auto* n = new Node(*this, id);
    pComponent.refreshHeaderInformations();
    return n;
}

Node* Manager::addNode(const wxString& id, const wxString& caption, const int x, const int y)
{
    auto* result = new Node(*this, id);
    result->caption(caption);
    result->position(x, y);
    pComponent.refreshHeaderInformations();
    return result;
}

bool Manager::remove(const wxString& text)
{
    Item* it = this->find(text);
    if (it)
    {
        delete it;
        if (!pUpdaterLock)
        {
            deleteAllPendingData();
            pComponent.refreshHeaderInformations();
        }
        return true;
    }
    return false;
}

Item* Manager::find(const int x, const int y) const
{
    if (!pAllNodes.empty())
    {
        auto end = pAllNodes.rend();
        double nearestDistance = 6, lastDistance = 0;
        Item* nearestItem = nullptr;
        for (auto i = pAllNodes.rbegin(); i != end; ++i)
        {
            auto jEnd = i->second.end();
            for (auto j = i->second.begin(); j != jEnd; ++j)
            {
                if (j->first->contains(x, y, lastDistance) && lastDistance <= nearestDistance)
                {
                    nearestDistance = lastDistance;
                    nearestItem = j->first;
                }
            }
        }
        return nearestItem;
    }
    return nullptr;
}

Item::Vector* Manager::find(const int x1, const int y1, const int x2, const int y2) const
{
    auto* result = new Item::Vector();

    if (!pAllNodes.empty())
    {
        NodeByZPosition::const_reverse_iterator end = pAllNodes.rend();
        for (NodeByZPosition::const_reverse_iterator i = pAllNodes.rbegin(); i != end; ++i)
        {
            NodeMap::const_iterator jEnd = i->second.end();
            for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
            {
                if (j->first->isContained(x1, y1, x2, y2))
                    result->push_back(j->first);
            }
        }
    }

    return result;
}

Item* Manager::find(const wxString& text) const
{
    if (!pAllNodes.empty())
    {
        NodeByZPosition::const_iterator end = pAllNodes.end();
        for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::const_iterator jEnd = i->second.end();
            for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
            {
                if (dynamic_cast<Node*>(j->first))
                {
                    Node* node = dynamic_cast<Node*>(j->first);
                    if (node->id() == text or node->caption() == text)
                        return node;
                }
            }
        }
    }
    return nullptr;
}

void Manager::invalidateAllNodes()
{
    if (!pAllNodes.empty())
    {
        NodeByZPosition::const_iterator end = pAllNodes.end();
        for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::const_iterator jEnd = i->second.end();
            for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
                j->first->invalidate();
        }
    }
}

/*void Manager::recentreAllNodes(const int offsetX, const int offsetY)
{
        // Getting the bounding box
        const BoundingBox box = this->boundingBox();

        // Computing the offset
        const int oriOffsetX = (box.first.x + box.second.x) / 2;
        const int oriOffsetY = (box.first.y + box.second.y) / 2;

        if (oriOffsetX or oriOffsetY or offsetX or offsetY)
        {
                // The offsets are not null - we have something to do
                // Browsing all nodes
                if (!pAllNodes.empty())
                {
                        NodeByZPosition::const_iterator end = pAllNodes.end();
                        for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
                        {
                                NodeMap::const_iterator jEnd = i->second.end();
                                for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
                                        j->first->move(offsetX + oriOffsetX, offsetY + oriOffsetX);
                        }
                }
        }
}*/

void Manager::refreshCacheForAllNodes(wxDC& dc)
{
    if (!pAllNodes.empty())
    {
        NodeByZPosition::const_iterator end = pAllNodes.end();
        for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::const_iterator jEnd = i->second.end();
            for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
            {
                if (j->first->isInvalidated() || j->first->selected())
                    j->first->refreshCache(dc);
            }
        }
    }
}

void Manager::selectOnly(const Data::Area::Vector& areaList)
{
    if (areaList.empty())
    {
        unselectAll();
        return;
    }
    Window::Inspector::Unselect();
    // Select only real nodes (and not connections for example)
    if (!pAllNodes.empty())
    {
        auto areaEnd = areaList.end();

        NodeByZPosition::iterator end = pAllNodes.end();
        for (NodeByZPosition::iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::iterator jEnd = i->second.end();
            for (NodeMap::iterator j = i->second.begin(); j != jEnd; ++j)
            {
                const Node* node = dynamic_cast<const Node*>(j->first);
                if (node)
                {
                    bool selected = false;
                    Data::Area* attachedArea = node->attachedArea();
                    for (Data::Area::Vector::const_iterator areaI = areaList.begin();
                         areaI != areaEnd;
                         ++areaI)
                    {
                        if ((*areaI) == attachedArea)
                        {
                            selected = true;
                            j->first->selected(true);
                            break;
                        }
                    }
                    if (!selected)
                        j->first->selected(false);
                }
                else
                    j->first->selected(false);
                j->first->invalidate();
            }
        }
        Window::Inspector::AddAreas(areaList);
    }
}

void Manager::selectOnly(const Data::Area::Vector& areaList, const Data::AreaLink::Vector& linkList)
{
    if (areaList.empty() and linkList.empty())
    {
        unselectAll();
        return;
    }
    Window::Inspector::Unselect();
    // Select only real nodes (and not connections for example)
    if (!pAllNodes.empty())
    {
        auto areaEnd = areaList.end();
        auto linkEnd = linkList.end();

        NodeByZPosition::iterator end = pAllNodes.end();
        for (NodeByZPosition::iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::iterator jEnd = i->second.end();
            for (NodeMap::iterator j = i->second.begin(); j != jEnd; ++j)
            {
                {
                    const Node* node = dynamic_cast<const Node*>(j->first);
                    if (node)
                    {
                        bool selected = false;
                        Data::Area* attachedArea = node->attachedArea();
                        for (Data::Area::Vector::const_iterator areaI = areaList.begin();
                             areaI != areaEnd;
                             ++areaI)
                        {
                            if ((*areaI) == attachedArea)
                            {
                                selected = true;
                                j->first->selected(true);
                                break;
                            }
                        }
                        if (selected)
                            continue;
                    }
                }
                {
                    const Connection* node = dynamic_cast<const Connection*>(j->first);
                    if (node)
                    {
                        bool selected = false;
                        Data::AreaLink* attachedAreaLink = node->attachedAreaLink();
                        for (auto areaI = linkList.begin(); areaI != linkEnd; ++areaI)
                        {
                            if ((*areaI) == attachedAreaLink)
                            {
                                selected = true;
                                j->first->selected(true);
                                break;
                            }
                        }
                        if (selected)
                            continue;
                    }
                }

                // We don't care of this item
                j->first->selected(false);
                j->first->invalidate();
            }
        }
        Window::Inspector::AddAreas(areaList);
        Window::Inspector::AddLinks(linkList);
    }
}

void Manager::selectOnly(Item* item)
{
    if (item and !pAllNodes.empty())
    {
        unselectAll();
        const Node* node = dynamic_cast<const Node*>(item);
        if (node && mouseSelectionArea)
        {
            item->selected(true);
            Window::Inspector::SelectArea(node->attachedArea());
            return;
        }
        const Connection* cnx = dynamic_cast<const Connection*>(item);
        if (cnx && mouseSelectionLinks)
        {
            item->selected(true);
            Window::Inspector::SelectLink(cnx->attachedAreaLink());

            if (mouseSelectionConstraints)
            {
                // building list of selected links
                Data::AreaLink::Set linklist = Window::Inspector::getLinks();

                Data::BindingConstraint::Set constraintlist;

                const Data::BindConstList::iterator cEnd = pStudy->bindingConstraints.end();
                for (Data::BindConstList::iterator i = pStudy->bindingConstraints.begin();
                     i != cEnd;
                     ++i)
                {
                    bool stop = false;
                    // alias to the current constraint
                    Data::BindingConstraint* constraint = *i;
                    const Data::BindingConstraint::const_iterator lend = constraint->end();
                    for (Data::BindingConstraint::const_iterator ly = constraint->begin();
                         ly != lend;
                         ++ly)
                    {
                        if (!linklist.count(const_cast<Data::AreaLink*>(ly->first)))
                        {
                            stop = true;
                            break;
                        }
                    }
                    if (!stop)
                        constraintlist.insert(constraint);
                }
                Window::Inspector::AddBindingConstraints(constraintlist);
            }

            return;
        }
        Window::Inspector::Unselect();
    }
}

void Manager::changeItemSelectionState(Item* item)
{
    bool select = !item->selected();
    const Node* node = dynamic_cast<const Node*>(item);
    if (node && mouseSelectionArea)
    {
        if (select)
            Window::Inspector::AddArea(node->attachedArea());
        else
            Window::Inspector::RemoveArea(node->attachedArea());
    }
    const Connection* cnnx = dynamic_cast<const Connection*>(item);
    if (cnnx && mouseSelectionLinks)
    {
        if (select)
            Window::Inspector::AddLink(cnnx->attachedAreaLink());
        else
            Window::Inspector::RemoveLink(cnnx->attachedAreaLink());

        if (mouseSelectionConstraints)
        {
            // building list of selected links
            Data::AreaLink::Set linklist = Window::Inspector::getLinks();

            Data::BindingConstraint::Set constraintlist;

            const Data::BindConstList::iterator cEnd = pStudy->bindingConstraints.end();
            for (Data::BindConstList::iterator i = pStudy->bindingConstraints.begin(); i != cEnd;
                 ++i)
            {
                bool stop = false;
                // alias to the current constraint
                Data::BindingConstraint* constraint = *i;
                const Data::BindingConstraint::const_iterator lend = constraint->end();
                for (Data::BindingConstraint::const_iterator ly = constraint->begin(); ly != lend;
                     ++ly)
                {
                    if (!linklist.count(const_cast<Data::AreaLink*>(ly->first)))
                    {
                        stop = true;
                        break;
                    }
                }
                if (!stop)
                    constraintlist.insert(constraint);
            }
            Window::Inspector::AddBindingConstraints(constraintlist);
        }
    }

    // Select
    item->selected(select);
}

void Manager::selectOnly(const Item::Vector& list)
{
    Window::Inspector::Unselect();
    if (pAllNodes.empty())
        return;

    unselectAll();
    foreach (auto* item, list)
    {
        item->selected(true);
        auto* node = dynamic_cast<const Node*>(item);
        if (node)
        {
            Window::Inspector::AddArea(node->attachedArea());
            continue;
        }
        auto* cnx = dynamic_cast<const Connection*>(item);
        if (cnx)
            Window::Inspector::AddLink(cnx->attachedAreaLink());
    }
}

void Manager::unselectAll()
{
    Window::Inspector::Unselect();
    while (!pSelectedItems.empty())
    {
        NodeMap::const_iterator i = pSelectedItems.begin();
        i->first->selected(false);
    }
}

void Manager::getAllSelectedItems(std::vector<Item*>& list)
{
    NodeMap::const_iterator jEnd = pSelectedItems.end();
    for (NodeMap::const_iterator j = pSelectedItems.begin(); j != jEnd; ++j)
        list.push_back(j->first);
}

void Manager::selectAllNodes(size_t layerID)
{
    Window::Inspector::Unselect();
    // Select only real nodes (and not connections for example)
    if (!pAllNodes.empty())
    {
        Data::Area::Vector list;
        NodeByZPosition::iterator end = pAllNodes.end();
        for (NodeByZPosition::iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::iterator jEnd = i->second.end();
            for (NodeMap::iterator j = i->second.begin(); j != jEnd; ++j)
            {
                const Node* node = dynamic_cast<const Node*>(j->first);
                if (node && node->isVisibleOnLayer(layerID))
                {
                    list.push_back(node->attachedArea());
                    j->first->selected(true);
                }
                else
                    j->first->selected(false);
            }
        }
        Window::Inspector::SelectAreas(list);
    }
}

void Manager::selectAllAreas(size_t layerID)
{
    Window::Inspector::Unselect();
    // Select only real nodes (and not connections for example)
    if (!pAllNodes.empty())
    {
        Data::Area::Vector list;
        NodeByZPosition::iterator end = pAllNodes.end();
        for (NodeByZPosition::iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::iterator jEnd = i->second.end();
            for (NodeMap::iterator j = i->second.begin(); j != jEnd; ++j)
            {
                const Node* node = dynamic_cast<const Node*>(j->first);
                if (node && node->isVisibleOnLayer(layerID))
                {
                    list.push_back(node->attachedArea());
                    j->first->selected(true);
                }
                else
                    j->first->selected(false);
            }
        }
        Window::Inspector::SelectAreas(list);
    }
}

void Manager::selectAllItems(size_t layerID)
{
    wxPoint a(INT_MIN, INT_MIN);
    wxPoint b(INT_MAX, INT_MAX);
    selectFromBoundingBox(a, b, layerID);
}

void Manager::selectAllLinks(size_t layerID)
{
    Window::Inspector::Unselect();
    // Select only real nodes (and not connections for example)
    if (!pAllNodes.empty())
    {
        Data::AreaLink::Vector linklist;
        NodeByZPosition::iterator end = pAllNodes.end();
        for (NodeByZPosition::iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::iterator jEnd = i->second.end();
            for (NodeMap::iterator j = i->second.begin(); j != jEnd; ++j)
            {
                const Connection* link = dynamic_cast<const Connection*>(j->first);
                if (link && link->isVisibleOnLayer(layerID))
                {
                    linklist.push_back(link->attachedAreaLink());
                    j->first->selected(true);
                    continue;
                }

                j->first->selected(false);
            }
        }
        Window::Inspector::SelectLinks(linklist);
    }
}

void Manager::reverseSelection()
{
    Window::Inspector::Unselect();
    // Select only real nodes (and not connections for example)
    if (!pAllNodes.empty())
    {
        Data::Area::Vector list;
        NodeByZPosition::iterator end = pAllNodes.end();
        for (NodeByZPosition::iterator i = pAllNodes.begin(); i != end; ++i)
        {
            NodeMap::iterator jEnd = i->second.end();
            for (NodeMap::iterator j = i->second.begin(); j != jEnd; ++j)
            {
                const Node* node = dynamic_cast<const Node*>(j->first);
                if (node and !node->selected())
                {
                    list.push_back(node->attachedArea());
                    j->first->selected(true);
                }
                else
                    j->first->selected(false);
            }
        }
        if (!list.empty())
            Window::Inspector::SelectAreas(list);
    }
}

void Manager::moveAllSelected(const int x, const int y)
{
    if (!pSelectedItems.empty())
    {
        NodeMap::const_iterator jEnd = pSelectedItems.end();
        for (NodeMap::const_iterator j = pSelectedItems.begin(); j != jEnd; ++j)
            j->first->move(x, y);
    }
}

void Manager::selectFromBoundingBox(const wxPoint& a, const wxPoint& b, const size_t layerID)
{
    Window::Inspector::Unselect();

    if (!pAllNodes.empty())
    {
        const int nx1 = Yuni::Math::Min<int>(a.x, b.x);
        const int ny1 = Yuni::Math::Min<int>(a.y, b.y);
        const int nx2 = Yuni::Math::Max<int>(a.x, b.x);
        const int ny2 = Yuni::Math::Max<int>(a.y, b.y);
        Data::Area::Set arealist;
        Data::AreaLink::Set linklist;
        Data::ThermalCluster::Vector clusterlist;
        Data::BindingConstraint::Set constraintlist;

        {
            NodeByZPosition::const_iterator end = pAllNodes.end();
            for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
            {
                NodeMap::const_iterator jEnd = i->second.end();
                for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
                {
                    bool gotcha = j->first->isContained(nx1, ny1, nx2, ny2)
                                  && j->first->isVisibleOnLayer(layerID);
                    if (!gotcha)
                    {
                        j->first->selected(false);
                        continue;
                    }
                    const Node* node = dynamic_cast<const Node*>(j->first);
                    if (node)
                    {
                        Data::Area* area = node->attachedArea();
                        arealist.insert(area);
                        if (mouseSelectionArea)
                            j->first->selected(true);
                        if (mouseSelectionPlants)
                        {
                            Data::ThermalClusterList::iterator tend = area->thermal.list.end();
                            for (Data::ThermalClusterList::iterator t = area->thermal.list.begin();
                                 t != tend;
                                 ++t)
                                clusterlist.push_back(t->second.get());
                        }
                        continue;
                    }
                    j->first->selected(false);
                }
            }
        }
        if ((mouseSelectionLinks or mouseSelectionConstraints) and !arealist.empty())
        {
            NodeByZPosition::const_iterator end = pAllNodes.end();
            for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
            {
                NodeMap::const_iterator jEnd = i->second.end();
                for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
                {
                    if (j->first->selected())
                        continue;
                    const Connection* link = dynamic_cast<const Connection*>(j->first);
                    if (link)
                    {
                        Data::AreaLink* al = link->attachedAreaLink();
                        if (al and arealist.count(al->from) and arealist.count(al->with))
                        {
                            linklist.insert(al);
                            if (mouseSelectionLinks)
                                j->first->selected(true);
                        }
                    }
                }
            }
        }

        if (mouseSelectionConstraints)
        {
            const Data::BindConstList::iterator end = pStudy->bindingConstraints.end();
            for (Data::BindConstList::iterator i = pStudy->bindingConstraints.begin(); i != end;
                 ++i)
            {
                bool stop = false;
                // alias to the current constraint
                Data::BindingConstraint* constraint = *i;
                const Data::BindingConstraint::const_iterator lend = constraint->end();
                for (Data::BindingConstraint::const_iterator ly = constraint->begin(); ly != lend;
                     ++ly)
                {
                    if (!linklist.count(const_cast<Data::AreaLink*>(ly->first)))
                    {
                        stop = true;
                        break;
                    }
                }
                if (!stop)
                    constraintlist.insert(constraint);
            }
        }

        // remove all items from the selection first
        Window::Inspector::Unselect();
        if (mouseSelectionArea)
            Window::Inspector::AddAreas(arealist);
        if (mouseSelectionLinks)
            Window::Inspector::AddLinks(linklist);
        if (mouseSelectionPlants)
            Window::Inspector::AddThermalClusters(clusterlist);
        if (mouseSelectionConstraints)
            Window::Inspector::AddBindingConstraints(constraintlist);
    }
}

void Manager::internalAddItem(Item* item, bool takeOwnership)
{
    if (item)
    {
        pAllNodes[item->zPosition()][item] = takeOwnership;
        if (!pUpdaterLock)
            pComponent.refreshHeaderInformations();
    }
}

void Manager::clear()
{
    beginUpdate();
    pSelectedItems.clear();
    if (not pAllNodes.empty())
    {
        // A vector of all items to delete will be created
        // This copy increases the general performances and avoid
        // unwanted behavior, because the destruction of a single item
        // may have big consequences on the map `pAllNodes` if not empty.
        // see internalRemoveItem()
        std::vector<Item*> toDelete;
        {
            auto end = pAllNodes.end();
            for (auto i = pAllNodes.begin(); i != end; ++i)
            {
                auto endM = i->second.end();
                for (auto j = i->second.begin(); j != endM; ++j)
                    toDelete.push_back(j->first);
            }
        }
        pAllNodes.clear();

        // Delayed destruction
        if (!toDelete.empty())
        {
            auto vEnd = toDelete.end();
            for (auto i = toDelete.begin(); i != vEnd; ++i)
                delete *i;
        }
    }

    endUpdate();
}

void Manager::internalRemoveItem(Item* item)
{
    if (!pAllNodes.empty())
    {
        if (!deallocationStackExists(item))
        {
            beginUpdate();
            // Item* itemToDelete = nullptr;
            deallocationStackAdd(item);

            NodeByZPosition::iterator i = pAllNodes.find(item->zPosition());
            if (i != pAllNodes.end())
            {
                NodeMap::iterator s = i->second.find(item);
                if (s != i->second.end())
                {
                    i->second.erase(s);
                    if (i->second.empty())
                        pAllNodes.erase(i);
                }
            }

            delete item;
            deallocationStackRemove(item);

            endUpdate();
        }
    }
}

void Manager::draw(DrawingContext& dc)
{
    const NodeByZPosition::iterator end = pAllNodes.end();
    for (NodeByZPosition::iterator i = pAllNodes.begin(); i != end; ++i)
    {
        const NodeMap::iterator endM = i->second.end();
        for (NodeMap::iterator j = i->second.begin(); j != endM; ++j)
            j->first->draw(dc);
    }
}

void Manager::drawExternalDrawer(DrawingContext& dc)
{
    if (!pExternalQueue.empty())
        pExternalQueue.front()->drawExternalDrawer(dc);
}

Connection* Manager::addConnection(Item* a, Item* b)
{
    Connection* c = new Connection(*this, a, b);
    internalAddItem(c, true);
    c->createANewConnectionIfNeeded();
    if (!pUpdaterLock)
        pComponent.refreshHeaderInformations();
    return c;
}

void Manager::addConnectionFromEachSelectedItem(Item* to, Item::Vector* results)
{
    Item::Vector list;

    const NodeMap::const_iterator jEnd = pSelectedItems.end();
    for (NodeMap::const_iterator j = pSelectedItems.begin(); j != jEnd; ++j)
    {
        Item* item = j->first;
        if (item->type() != Item::tyNode)
            continue;

        if (item->pLinks and not item->pLinks->empty())
        {
            bool canContinue = true;

            // Checking for direct dependencies
            const Item::Links::const_iterator end = item->pLinks->end();
            for (Item::Links::const_iterator z = item->pLinks->begin(); z != end; ++z)
            {
                if (z->first->type() == Item::tyConnection)
                {
                    const Connection* cnnx = dynamic_cast<const Connection*>(z->first);
                    if (cnnx and (cnnx->leftSide() == to or cnnx->rightSide() == to))
                    {
                        canContinue = false;
                        break;
                    }
                }
            }

            if (!canContinue)
                continue;
        }
        list.push_back(item);
    }

    if (results)
        results->clear();

    if (not list.empty())
    {
        Item::Vector::const_iterator end = list.end();
        for (Item::Vector::const_iterator j = list.begin(); j != end; ++j)
        {
            if (results)
            {
                Item* a = this->addConnection(*j, to);
                if (a)
                    results->push_back(a);
            }
            else
                this->addConnection(*j, to);
        }
    }
    if (not pUpdaterLock)
        pComponent.refreshHeaderInformations();
}

bool Manager::removeTheFirstSelectedItem()
{
    if (!pSelectedItems.empty())
    {
        NodeMap::const_iterator j = pSelectedItems.begin();
        internalRemoveItem(j->first);
        if (!pUpdaterLock)
            deleteAllPendingData();
        return true;
    }
    return false;
}

uint Manager::removeAllSelected()
{
    Window::Inspector::Unselect();
    uint ret = 0;
    if (!pSelectedItems.empty())
    {
        beginUpdate();
        while (removeTheFirstSelectedItem())
            ++ret;
        endUpdate();
    }
    return ret;
}

void Manager::hideAllSelected(size_t layerID)
{
    Window::Inspector::Unselect();

    if (!pSelectedItems.empty())
    {
        beginUpdate();
        NodeMap::const_iterator j = pSelectedItems.begin();
        NodeMap::const_iterator end = pSelectedItems.end();
        for (; j != end; j++)
            if (dynamic_cast<Node*>(j->first))
                (dynamic_cast<Node*>(j->first))->removeLayerVisibility(layerID);
        endUpdate();
    }
}

void Manager::showAllSelected(size_t layerID)
{
    Window::Inspector::Unselect();

    if (!pSelectedItems.empty())
    {
        beginUpdate();
        NodeMap::const_iterator j = pSelectedItems.begin();
        NodeMap::const_iterator end = pSelectedItems.end();
        for (; j != end; j++)
            if (dynamic_cast<Node*>(j->first))
                (dynamic_cast<Node*>(j->first))->addLayerVisibility(layerID);
        endUpdate();
    }
}

Data::AreaName Manager::findNewCaption(Data::AreaName wantedName) const
{
    if (!(!pStudy))
    {
        // Adding an area
        Data::AreaName newName;
        newName = wantedName.empty() ? "Area" : wantedName << " ";
        Data::AreaName id;
        TransformNameIntoID(newName, id);
        if (pStudy->areas.find(id))
        {
            int i = 1;
            do
            {
                ++i;
                if (i > 1000000)
                {
                    logs.error() << "Impossible to find a name for a new area";
                    return Data::AreaName();
                }
                newName = wantedName.empty() ? "Area " : wantedName;
                newName << i;
                TransformNameIntoID(newName, id.clear());
            } while (pStudy->areas.find(id));
        }
        logs.debug() << "  found new area name: " << newName;
        return newName;
    }
    else
    {
        assert(false and "no study");
        return nullptr;
    }
}

uint Manager::areasCount(const size_t& layerID) const
{
    if (not pAllNodes.empty())
    {
        uint result(0);
        const NodeByZPosition::const_iterator end = pAllNodes.end();
        for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
        {
            const NodeMap::const_iterator jEnd = i->second.end();
            for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
            {
                if (dynamic_cast<Node*>(j->first))
                    if ((dynamic_cast<Node*>(j->first))->isVisibleOnLayer(layerID))
                        ++result;
            }
        }
        return result;
    }
    return 0;
}

void Manager::removeLayerVisibility(const size_t& layerID)
{
    const NodeByZPosition::const_iterator end = pAllNodes.end();
    for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
    {
        const NodeMap::const_iterator jEnd = i->second.end();
        for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
        {
            if (dynamic_cast<Node*>(j->first))
                (dynamic_cast<Node*>(j->first))->removeLayerVisibility(layerID);
        }
    }
}

size_t Manager::getActiveLayerId()
{
    return pComponent.getActiveLayerID();
}

Node* Manager::find(const Data::Area* area)
{
    const NodeByZPosition::const_iterator end = pAllNodes.end();
    for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
    {
        const NodeMap::const_iterator jEnd = i->second.end();
        for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
        {
            Node* node = (dynamic_cast<Node*>(j->first));
            if (node and node->attachedArea() == area)
                return node;
        }
    }
    return nullptr;
}

uint Manager::connectionsCount(const size_t& layerID) const
{
    if (!pAllNodes.empty())
    {
        uint result(0);
        const NodeByZPosition::const_iterator end = pAllNodes.end();
        for (NodeByZPosition::const_iterator i = pAllNodes.begin(); i != end; ++i)
        {
            const NodeMap::const_iterator jEnd = i->second.end();
            for (NodeMap::const_iterator j = i->second.begin(); j != jEnd; ++j)
            {
                if (dynamic_cast<Connection*>(j->first))
                    if ((dynamic_cast<Connection*>(j->first))->isVisibleOnLayer(layerID))
                        ++result;
            }
        }
        return result;
    }
    return 0;
}

void Manager::pendingDeleteArea(Data::Area* area)
{
    OnStudyAreaDelete(area);
    pAreasToDelete.push_back(area);
}

void Manager::pendingDeleteLink(Data::AreaLink* lnk)
{
    OnStudyLinkDelete(lnk);
    pLinksToDelete.push_back(lnk);
}

void Manager::deleteAllPendingData()
{
    bool reloadConstraints = false;

    // The links must be destroyed before the areas
    if (!pLinksToDelete.empty())
    {
        if (study())
        {
            logs.info() << "Destroying " << pLinksToDelete.size() << " links...";
            const Data::AreaLink::Vector::iterator end = pLinksToDelete.end();
            for (Data::AreaLink::Vector::iterator i = pLinksToDelete.begin(); i != end; ++i)
            {
                study()->linkDelete(*i);
                OnStudyLinkDelete(*i);
            }
        }
        pLinksToDelete.clear();
        MarkTheStudyAsModified();
        GUIFlagInvalidateAreas = true;
        reloadConstraints = true;
    }

    if (!pAreasToDelete.empty())
    {
        if (study())
        {
            study()->areaDelete(pAreasToDelete);
            OnStudyAreasChanged();
        }
        pAreasToDelete.clear();
        MarkTheStudyAsModified();
        GUIFlagInvalidateAreas = true;
        reloadConstraints = true;
    }

    if (reloadConstraints and study() and study()->uiinfo)
        study()->uiinfo->reloadAll();
}

void Manager::beginUpdate()
{
    ++pUpdaterLock;
}

void Manager::endUpdate()
{
    assert(pUpdaterLock > 0 and "internal error");
    if (!(--pUpdaterLock))
    {
        deleteAllPendingData();
        pComponent.refreshHeaderInformations();
    }
}

} // namespace Map
} // namespace Antares

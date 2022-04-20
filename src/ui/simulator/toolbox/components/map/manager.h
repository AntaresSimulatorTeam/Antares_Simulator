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
#ifndef __ANTARES_TOOLBOX_MAP_CONTAINER_H__
#define __ANTARES_TOOLBOX_MAP_CONTAINER_H__

#include <antares/wx-wrapper.h>
#include <antares/study.h>
#include <vector>
#include <queue>
#include "nodes/node.h"

namespace Antares
{
namespace Map
{
//! A bounding box
typedef std::pair<wxPoint, wxPoint> BoundingBox;

// Forward declarations
class Connection;
class Component;
class Node;
class BindingConstraint;

/*!
** \brief A convenient container for nodes on the graph
*/
class Manager final
{
public:
    /*!
    ** \brief Get the instance of the main window
    */
    static Manager* Instance();

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Manager(Component& component);
    //! Destructor
    ~Manager();
    //@}

    //! \name Study
    //@{
    /*!
    ** \brief Attach this map to an existing study
    */
    void attachStudy(Data::Study::Ptr study);

    /*!
    ** \brief Get the attached study to this map
    */
    Data::Study::Ptr study() const
    {
        return pStudy;
    }
    //@}

    //! \name Bounding box
    //@{
    /*!
    ** \brief Get the 2D-Bounding box wich contains all nodes
    */
    BoundingBox boundingBox(const size_t& layerID) const;

    /*!
    ** \brief Get the 2D-Bounding box wich contains all nodes
    */
    BoundingBox boundingBoxOfSelectedNodes(const size_t& layerID) const;

    /*!
    ** \brief Recentre all nodes to the origin point
    **
    ** \param offsetX An additional offset for the X-axis
    ** \param offsetY An additional offset for the Y-axis
    */
    // void recentreAllNodes(const int offsetX = 0, const int offsetY = 0);
    //@}

    //! \name Node manipulation
    //@{
    /*!
    ** \brief Load the layout from an existing study
    **
    ** All existing nodes/connections will be removed first
    ** \param study The real study
    */
    bool loadFromStudy(Data::Study& study);

    /*!
    ** \brief Load the layout from the attached study
    */
    bool loadFromAttachedStudy();

    /*!
    ** \brief Add a new node in the map
    */
    template<class T>
    T* add();

    /*!
    ** \brief Add a new node with a given id
    */
    Node* addNode(const wxString& id);

    /*!
    ** \brief Add a new node with a given id, caption and position
    */
    Node* addNode(const wxString& id, const wxString& caption, const int x, const int y);

    /*!
    ** \brief Add a connection between two items
    */
    Connection* addConnection(Item* a, Item* b);

    /*!
    ** \brief Add a connection for each selected item to a item
    */
    void addConnectionFromEachSelectedItem(Item* to, Item::Vector* results = NULL);

    /*!
    ** \brief Remove a node from its id or its caption
    */
    bool remove(const wxString& text);

    /*!
    ** \brief Remove all selected items
    ** \return The number of removed items
    */
    uint removeAllSelected();

    /*!
    ** \brief Hide all selected items from a layer
    */
    void hideAllSelected(size_t id);

    /*!
    ** \brief Show all selected items from a layer
    */
    void showAllSelected(size_t id);

    /*!
    ** \brief Clear (and destroy) all nodes
    */
    void clear();

    /*!
    ** \brief Find a node from its position
    */
    Item* find(const int x, const int y) const;

    /*!
    ** \brief Get the list of node which are contained inside a bounding box
    **
    ** The returned list must be deleted.
    **
    ** \param x1 The X-Coordinate of the top-left point of the bouding box
    ** \param y1 The Y-Coordinate of the top-left point of the bouding box
    ** \param x2 The X-Coordinate of the bottom-right point of the bouding box
    ** \param y2 The Y-Coordinate of the bottom-right point of the bouding box
    ** \return A valid pointer to a list of nodes. The list may be empty.
    */
    Item::Vector* find(const int x1, const int y1, const int x2, const int y2) const;

    /*!
    ** \brief Find a node from its id or its caption
    */
    Item* find(const wxString& text) const;

    Node* find(const Data::Area* area);

    /*!
    ** \brief Moved all selected nodes
    */
    void moveAllSelected(const int x, const int y);

    /*!
    ** \brief Invalidate all nodes
    */
    void invalidateAllNodes();

    /*!
    ** \brief Update the cache for all nodes
    */
    void refreshCacheForAllNodes(wxDC& dc);
    //@}

    //! \name Selection
    //@{
    /*!
    ** \brief Select a single item (a node or an interconnection)
    */
    void selectOnly(Item* item);

    void changeItemSelectionState(Item* item);

    void selectOnly(const Item::Vector& item);

    void selectOnly(const Data::Area::Vector& areas);

    void selectOnly(const Data::Area::Vector& areas, const Data::AreaLink::Vector& links);

    /*!
    ** \brief Select all nodes
    */
    void selectAllNodes(size_t layerID);

    /*!
    ** \brief Select all items, without any distinction
    */
    void selectAllItems(size_t layerID);

    /*!
    ** \brief Select all areas only
    */
    void selectAllAreas(size_t layerID);

    /*!
    ** \brief Select all links
    */
    void selectAllLinks(size_t layerID);

    /*!
    ** \brief Unselect all nodes
    */
    void unselectAll();

    /*!
    ** \brief Reverse the selection
    */
    void reverseSelection();

    /*!
    ** \brief Select all nodes which are contained in a bounding box
    */
    void selectFromBoundingBox(const wxPoint& a, const wxPoint& b, const size_t layerID = 0);

    /*!
    ** \brief Get the count of nodes that are selected
    */
    uint selectedItemsCount() const
    {
        return (uint)pSelectedItems.size();
    }

    uint selectedItemsAsConnectionCount() const
    {
        return (uint)pSelectedItemsAsConnection;
    };

    void getAllSelectedItems(std::vector<Item*>& list);

    Data::AreaName findNewCaption(Data::AreaName = "") const;
    //@}

    void beginUpdate();
    void endUpdate();

    /*!
    ** \brief Draw all items
    */
    void draw(DrawingContext& dc);

    void drawExternalDrawer(DrawingContext& dc);

    void drawerVisible(bool v);

    bool hasChanges() const
    {
        return pHasChanges;
    }
    void setChangesFlag(bool v)
    {
        pHasChanges = v;
    }

    /*!
    ** \brief Get how many area we have
    */
    uint areasCount(const size_t& layerID) const;

    /*!
    ** \brief remove layer visibility for all nodes
    */
    void removeLayerVisibility(const size_t& layerID);

    size_t getActiveLayerId();

    /*!
    ** \brief Get how many connections we have
    */
    uint connectionsCount(const size_t& layerID) const;

    void pendingDeleteArea(Data::Area* area);
    void pendingDeleteLink(Data::AreaLink* lnk);

public:
    bool mouseSelectionArea;
    bool mouseSelectionLinks;
    bool mouseSelectionPlants;
    bool mouseSelectionConstraints;

private:
    void internalAddItem(Item* item, bool takeOwnership = false);
    void internalRemoveItem(Item* item);
    bool removeTheFirstSelectedItem();

    void deallocationStackAdd(Item* i);
    void deallocationStackRemove(Item* it);
    bool deallocationStackExists(Item* it);

    void deleteAllPendingData();

private:
    //! All nodes ordered by their Z-Position
    typedef std::map<Item*, bool> NodeMap;
    typedef std::map<int, NodeMap> NodeByZPosition;

private:
    Component& pComponent;
    Data::Study::Ptr pStudy;

    NodeByZPosition pAllNodes;

    Item::Set pStackDeallocation;

    NodeMap pSelectedItems;
    int pSelectedItemsAsConnection;

    std::queue<Item*> pExternalQueue;

    bool pHasChanges;

    //!
    uint pUpdaterLock;

    Data::AreaLink::Vector pLinksToDelete;
    Data::Area::Vector pAreasToDelete;

    // Some friends
    friend class ::Antares::Map::Item;
    friend class ::Antares::Map::Node;
    friend class ::Antares::Map::Connection;
    friend class ::Antares::Map::BindingConstraint;

}; // class Manager

} // namespace Map
} // namespace Antares

#include "nodes/connection.h"
#include "manager.hxx"

#endif // __ANTARES_TOOLBOX_MAP_CONTAINER_H__

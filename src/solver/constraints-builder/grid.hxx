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
#include "grid.h"
#include "cbuilder.h"
#include <algorithm>
#include <utility>
#include <algorithm>
#include <numeric>
#include <stack>
#include <list>

using namespace Yuni;

namespace Antares
{
namespace Graph
{
template<class NodeT>
Grid<NodeT>::Grid() : inf(0)
{
}

template<class NodeT>
Grid<NodeT>::~Grid()
{
    for (auto i = pEdgesList.begin(); i != pEdgesList.end(); i++)
        delete (*i);
    for (auto i = pNodesList.begin(); i != pNodesList.end(); i++)
        delete (*i);
}

template<class NodeT>
typename Grid<NodeT>::NodeP Grid<NodeT>::addNode(NodeT& n, std::string id)
{
    auto nodeIT
      = std::find_if(pNodesList.begin(), pNodesList.end(), [&id](const NodeP& nodeP) -> bool {
            return nodeP->getName() == id;
        });

    if (nodeIT == pNodesList.end())
    {
        NodeP newNode = new Graph::Node<NodeT>(&n);
        newNode->setName(id);
        pNodesList.push_back(newNode);
        return newNode;
    }
    else
    {
        return *nodeIT;
    }
}

template<class NodeT>
typename Grid<NodeT>::EdgeP Grid<NodeT>::addEdge(NodeP n1, NodeP n2, long weight)
{
    std::string s1(n1->getName());
    std::string s2(n2->getName());
    EdgeP edgePtr = findEdgeFromNodeNames(s1, s2);
    if (edgePtr == nullptr)
    {
        EdgeP newEdge = new Edge<NodeT>(n1, n2);
        inf += weight;
        newEdge->setWeight(weight);

        // add edge to the list
        pEdgesList.push_back(newEdge);

        // redefine inf
        inf += weight;

        // update adjency
        adjency[newEdge->getOrigin()].insert(std::make_pair(newEdge->getDestination(), newEdge));
        adjency[newEdge->getDestination()].insert(std::make_pair(newEdge->getOrigin(), newEdge));

        return newEdge;
    }
    else
        return edgePtr;
}

template<class NodeT>
uint Grid<NodeT>::getNumberOfConnectedComponents()
{
    // create union-find set
    MapNodes uf;

    for (auto i = pNodesList.begin(); i != pNodesList.end(); i++)
    {
        uf.insert(std::make_pair(*i, *i));
    }

    // algorithm
    for (auto i = pEdgesList.begin(); i != pEdgesList.end(); i++)
    {
        // if the two areas are not yet connected
        if (uf[(*i)->getOrigin()] != uf[(*i)->getDestination()])
        {
            // update uf
            NodeP cset(uf[(*i)->getDestination()]);
            for (auto j = pNodesList.begin(); j != pNodesList.end(); j++)
            {
                if (uf[*j] == cset)
                {
                    uf[*j] = uf[(*i)->getOrigin()];
                }
            }
        }
    }

    std::unordered_map<NodeP, bool> cVec;
    for (auto cIT = uf.begin(); cIT != uf.end(); cIT++)
    {
        cVec[cIT->second] = true;
    }

    return (uint)(cVec.size());
}

template<class NodeT>
void Grid<NodeT>::kruskal()
{
    // clear spanning tree
    pMinSpanningTree.clear();

    // create union-find set
    MapNodes uf;

    for (auto i = pNodesList.begin(); i != pNodesList.end(); i++)
    {
        uf.insert(std::make_pair(*i, *i));
    }

    // create temporary sorted vector of Link
    VectorEdgeP tempEdgesList = pEdgesList;
    std::stable_sort(
      tempEdgesList.begin(), tempEdgesList.end(), typename Graph::Edge<NodeT>::compareWeight());

    // algorithm
    for (auto i = tempEdgesList.begin(); i != tempEdgesList.end(); i++)
    {
        // if the two areas are not yet connected
        if (uf[(*i)->getOrigin()] != uf[(*i)->getDestination()])
        {
            pMinSpanningTree.push_back(*i);

            // update uf
            NodeP cset(uf[(*i)->getDestination()]);
            for (auto j = pNodesList.begin(); j != pNodesList.end(); j++)
            {
                if (uf[*j] == cset)
                {
                    uf[*j] = uf[(*i)->getOrigin()];
                }
            }
        }
    }
}

template<class NodeT>
bool Grid<NodeT>::buildMesh()
{
    // build the minimum spanning tree
    kruskal();

    if (pMinSpanningTree.empty())
        return false;

    // clear the spanning cycle base
    pMesh.clear();
    meshIndexMatrix.clear();

    // initialization
    std::vector<EdgeP> linksToBeAdded;
    for (auto i = pEdgesList.begin(); i != pEdgesList.end(); i++)
    {
        // check if the link already belong to the skeleton
        if (std::find(pMinSpanningTree.begin(), pMinSpanningTree.end(), (*i))
            == pMinSpanningTree.end())
            linksToBeAdded.push_back(*i);
    }

    std::vector<EdgeIncidence> incidenceMatrix(linksToBeAdded.size());
    for (uint i = 0; i < linksToBeAdded.size(); i++)
    {
        incidenceMatrix[i] = getIncidenceVector(linksToBeAdded[i]);
    }

    for (uint i = 0; i < linksToBeAdded.size(); i++)
    {
        logs.info() << "Searching basis (loop " << i+1 << "/" << linksToBeAdded.size() << ")";
        // build the graph with two copies (+/-) for each node and edge
        Grid<NodeT> polarisedDuplicate;
        getDuplicatedGrid(polarisedDuplicate);
        VectorEdgeP v = getEdgeVectorFromIncidence(incidenceMatrix[i]);
        VectorEdgeP Ci;
        //! vecteur contenant les sommets adjacents aux arêtes Ei
        VectorNodeP adjacentNodes;
        // retrait des arêtes (u,v) qui sont dan Ei et remplacement dans le graphe duliqué
        // par(u+,v-) et (u-,v+)
        for (uint j = 0; j < v.size(); j++)
        {
            if (std::find(adjacentNodes.begin(), adjacentNodes.end(), v[j]->getOrigin())
                == adjacentNodes.end())
                adjacentNodes.push_back(v[j]->getOrigin());
            if (std::find(adjacentNodes.begin(), adjacentNodes.end(), v[j]->getDestination())
                == adjacentNodes.end())
                adjacentNodes.push_back(v[j]->getDestination());

            EdgeP ei = polarisedDuplicate.findEdgeFromNodeNames(
              v[j]->getOrigin()->getName() + "+", v[j]->getDestination()->getName() + "+");

            NodeP ni1 = polarisedDuplicate.findNodeFromName(v[j]->getOrigin()->getName() + "+");
            NodeP ni2
              = polarisedDuplicate.findNodeFromName(v[j]->getDestination()->getName() + "-");

            polarisedDuplicate.addEdge(ni1, ni2, ei->getWeight());
            polarisedDuplicate.removeEdge(ei);

            ei = polarisedDuplicate.findEdgeFromNodeNames(v[j]->getOrigin()->getName() + "-",
                                                          v[j]->getDestination()->getName() + "-");

            ni1 = polarisedDuplicate.findNodeFromName(v[j]->getOrigin()->getName() + "-");
            ni2 = polarisedDuplicate.findNodeFromName(v[j]->getDestination()->getName() + "+");

            polarisedDuplicate.addEdge(ni1, ni2, ei->getWeight());
            polarisedDuplicate.removeEdge(ei);
        }

        //! extraction du chemin sur deux niveaux
        v = polarisedDuplicate.twoLevelPath(adjacentNodes);

        //! définition du cycle correspondant dans le graph d'origine (non signé)
        Ci.clear();
        std::vector<int> edgeIndices;
        for (typename VectorEdgeP::iterator e = v.begin(); e != v.end(); e++)
        {
            auto name1 = (*e)->getOrigin()->getName();
            name1 = name1.substr(0, name1.length() - 1);
            auto name2 = (*e)->getDestination()->getName();
            name2 = name2.substr(0, name2.length() - 1);
            EdgeP ei = findEdgeFromNodeNames(name1, name2);
            Ci.push_back(ei);

            auto eIT = std::find(pEdgesList.begin(), pEdgesList.end(), ei);
            int pos = (int)std::distance(pEdgesList.begin(), eIT);
            edgeIndices.push_back(pos);
        }

        pMesh.push_back(Ci);
        EdgeIncidence I = getIncidenceVector(Ci);
        meshIndexMatrix.push_back(edgeIndices);
        // remplacement par la différence symétrique des des jeux d'arêtes
        for (uint j = i + 1; j < linksToBeAdded.size(); j++)
        {
            if (incidenceInnerProduct(I, incidenceMatrix[j]) == 1)
            {
                incidenceMatrix[j] = incidenceXOR(incidenceMatrix[i], incidenceMatrix[j]);
            }
        }
    }

    return true;
}

/*!
** \brief get a Grid where each edge and node is duplicated and signed
** \param a reference to a virgin grid object
*/
template<class NodeT>
bool Grid<NodeT>::getDuplicatedGrid(Grid<NodeT>& grid)
{
    // duplicate nodes
    for (typename VectorNodeP::iterator node = pNodesList.begin(); node != pNodesList.end(); node++)
    {
        { //+
            grid.addNode((*(*node)->nodeProperties), (*node)->getName() + "+");
        }

        { //-
            grid.addNode((*(*node)->nodeProperties), (*node)->getName() + "-");
        }
    }

    // Duplicate edges
    for (typename VectorEdgeP::iterator e = pEdgesList.begin(); e != pEdgesList.end(); e++)
    {
        { //+
            auto nodeOrigIT = std::find_if(
              grid.pNodesList.begin(), grid.pNodesList.end(), [&e](const NodeP& nodeP) -> bool {
                  return nodeP->getName() == (*e)->getOrigin()->getName() + "+";
              });

            auto nodeDestIT = std::find_if(
              grid.pNodesList.begin(), grid.pNodesList.end(), [&e](const NodeP& nodeP) -> bool {
                  return nodeP->getName() == (*e)->getDestination()->getName() + "+";
              });

            grid.addEdge(*nodeOrigIT, *nodeDestIT, (*e)->getWeight());
        }

        { //-
            auto nodeOrigIT = std::find_if(
              grid.pNodesList.begin(), grid.pNodesList.end(), [&e](const NodeP& nodeP) -> bool {
                  return nodeP->getName() == (*e)->getOrigin()->getName() + "-";
              });

            auto nodeDestIT = std::find_if(
              grid.pNodesList.begin(), grid.pNodesList.end(), [&e](const NodeP& nodeP) -> bool {
                  return nodeP->getName() == (*e)->getDestination()->getName() + "-";
              });

            grid.addEdge(*nodeOrigIT, *nodeDestIT, (*e)->getWeight());
        }
    }
    return true;
}

/*!
** \brief get a clone of the grid
** \param a reference to a virgin grid object
*/
template<class NodeT>
bool Grid<NodeT>::cloneGrid(Grid<NodeT>& grid)
{
    // duplicate nodes
    for (typename VectorNodeP::iterator node = pNodesList.begin(); node != pNodesList.end(); node++)
    {
        {
            grid.addNode((*(*node)->nodeProperties), (*node)->getName());
        }
    }

    // duplicate edges
    for (typename VectorEdgeP::iterator e = pEdgesList.begin(); e != pEdgesList.end(); e++)
    {
        { //+
            auto nodeOrigIT = std::find_if(
              grid.pNodesList.begin(), grid.pNodesList.end(), [&e](const NodeP& nodeP) -> bool {
                  return nodeP->getName() == (*e)->getOrigin()->getName();
              });

            auto nodeDestIT = std::find_if(
              grid.pNodesList.begin(), grid.pNodesList.end(), [&e](const NodeP& nodeP) -> bool {
                  return nodeP->getName() == (*e)->getDestination()->getName();
              });

            grid.addEdge(*nodeOrigIT, *nodeDestIT, (*e)->getWeight());
        }
    }
    return true;
}

template<class NodeT>
typename Grid<NodeT>::VectorEdgeP Grid<NodeT>::twoLevelPath(VectorNodeP vN)
{
    VectorEdgeP SP, minSP;
    double minLength = DBL_MAX, length;
    for (typename VectorNodeP::iterator n = vN.begin(); n != vN.end(); n++)
    {
        SP = findShortestPath(findNodeFromName((*n)->getName() + "+"),
                              findNodeFromName((*n)->getName() + "-"));
        length = std::accumulate(
          SP.begin(), SP.end(), (long)0, typename Graph::Edge<NodeT>::addpWeight());
        if (length < minLength)
        {
            minLength = length;
            minSP = SP;
        }
    }

    return minSP;
}

//! Find shortest path between the two nodes (Djikstra)
template<class NodeT>
typename Grid<NodeT>::VectorEdgeP Grid<NodeT>::findShortestPath(NodeP node1, NodeP node2) const
{
    assert(node1 != node2);

    // Dijkstra (lots of computation which could be optimised)
    // initialization
    std::map<NodeP, double> dist;
    std::map<NodeP, NodeP> prev;

    bool checkNode1(0), checkNode2(0);
    VectorNodeP nodes = pNodesList;

    for (auto i = nodes.begin(); i != nodes.end(); i++)
    {
        dist[(*i)] = inf;
        prev[(*i)] = 0;
        if ((*i) == node1)
        {
            checkNode1 = true;
        }
        else if ((*i) == node2)
        {
            checkNode2 = true;
        }
    }
    assert(checkNode1 && checkNode2);
    dist[node1] = 0;
    // search for shortest path
    bool targetFound(0);
    while (!targetFound)
    {
        // look for closest element u belonging in nodes
        std::pair<Grid::NodeP, double> u(nullptr, inf - 1);
        typename Grid::VectorNodeP::iterator ui;
        for (auto i = nodes.begin(); i != nodes.end(); i++)
        {
            if (dist[(*i)] < u.second)
            {
                u.first = (*i);
                u.second = dist[(*i)];
                ui = i;
            }
        }

        // see if destination node has been f
        if (u.first == node2)
        {
            targetFound = true;
            break;
        }

        // remove u from the list of nodes
        nodes.erase(ui);

        // update the neighoubours of u
        for (auto i = adjency.at(u.first).begin(); i != adjency.at(u.first).end(); i++)
        {
            if (i->second == nullptr)
                assert(0);

            if ((u.second + i->second->getWeight()) < dist[i->first])
            {
                dist[i->first] = u.second + i->second->getWeight();
                prev[i->first] = u.first;
            }
        }
    }

    // rebuild path
    Grid::VectorEdgeP path;
    Grid::NodeP currentNode = node2;

    while (!prev[currentNode] == 0)
    {
        path.push_back(adjency.at(currentNode).at(prev[currentNode]));
        currentNode = prev[currentNode];
    }

    return path;
}

} // namespace Graph
} // namespace Antares

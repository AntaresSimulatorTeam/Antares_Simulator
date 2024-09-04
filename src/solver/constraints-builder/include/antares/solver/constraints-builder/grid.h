/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef CONSTRAINTSBUILDER_BUILDER_GRID_H
#define CONSTRAINTSBUILDER_BUILDER_GRID_H

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/study/study.h>

namespace Antares
{
namespace Graph
{
template<class NodeT>
class Node
{
public:
    Node(NodeT* data)
    {
        nodeProperties = data;
    }

    virtual ~Node() = default;

    virtual std::string getName()
    {
        return name;
    }

    virtual void setName(std::string newName)
    {
        name = newName;
    }

public:
    std::string name;
    NodeT* nodeProperties;
};

template<class NodeT>
class Edge
{
public:
    Edge(Node<NodeT>* from, Node<NodeT>* to)
    {
        originNode = from;
        destinationNode = to;
    }

    long getWeight() const
    {
        return weight;
    }

    void setWeight(long w)
    {
        weight = w;
    }

    Node<NodeT>* getOrigin()
    {
        return originNode;
    }

    Node<NodeT>* getDestination()
    {
        return destinationNode;
    }

    void setOrigin(Node<NodeT>* from)
    {
        originNode = from;
    }

    void setDestination(Node<NodeT>* to)
    {
        destinationNode = to;
    }

private:
    long weight;
    Node<NodeT>* originNode;
    Node<NodeT>* destinationNode;

public:
    struct addpWeight
    {
        long operator()(long i, const Edge<NodeT>* o) const
        {
            return (o->getWeight() + i);
        }
    };

    struct compareWeight
    {
        bool operator()(const Edge<NodeT>* lhs, const Edge<NodeT>* rhs) const
        {
            return lhs->getWeight() < rhs->getWeight();
        }
    };
};

/*!
** \brief Antares Grid (graph)
*/
template<class NodeT>
class Grid
{
public:
    typedef Antares::Graph::Node<NodeT>* NodeP;
    typedef Antares::Graph::Edge<NodeT>* EdgeP;
    typedef std::vector<NodeP> VectorNodeP;
    typedef std::vector<EdgeP> VectorEdgeP;
    typedef std::map<NodeP, NodeP> MapNodes;
    typedef std::vector<bool> EdgeIncidence;

public:
    //! \name Constructor & Destructor
    //@{
    //! Constructor
    Grid();
    //! Destructor
    ~Grid();
    //@}

    //! Add one node to the graph
    NodeP addNode(NodeT&, std::string);

    //! Add one edge to the graph
    EdgeP addEdge(NodeP, NodeP, long weight = 0);

    uint getNumberOfConnectedComponents();

    VectorEdgeP findShortestPath(NodeP node1, NodeP node2) const;

    //! find an edge from node names
    EdgeP findEdgeFromNodeNames(std::string u, std::string v)
    {
        auto edgeIT = std::find_if(pEdgesList.begin(),
                                   pEdgesList.end(),
                                   [&u, &v](const EdgeP& edgeP) -> bool
                                   {
                                       if (edgeP->getOrigin()->getName() == u
                                           && edgeP->getDestination()->getName() == v)
                                       {
                                           return true;
                                       }
                                       if (edgeP->getDestination()->getName() == u
                                           && edgeP->getOrigin()->getName() == v)
                                       {
                                           return true;
                                       }
                                       else
                                       {
                                           return false;
                                       }
                                   });
        if (edgeIT != pEdgesList.end())
        {
            return *edgeIT;
        }

        return nullptr;
    }

    EdgeP findDrivingEdgeFromNodeNames(std::string u, std::string v)
    {
        auto edgeIT = std::find_if(pMinSpanningTree.begin(),
                                   pMinSpanningTree.end(),
                                   [&u, &v](const EdgeP& edgeP) -> bool
                                   {
                                       if (edgeP->getOrigin()->getName() == u
                                           && edgeP->getDestination()->getName() == v)
                                       {
                                           return true;
                                       }
                                       if (edgeP->getDestination()->getName() == u
                                           && edgeP->getOrigin()->getName() == v)
                                       {
                                           return true;
                                       }
                                       else
                                       {
                                           return false;
                                       }
                                   });
        if (edgeIT != pMinSpanningTree.end())
        {
            return *edgeIT;
        }

        return nullptr;
    }

    // remove an edge from the graph
    void removeEdge(EdgeP e)
    {
        auto edgeIT = std::find(pEdgesList.begin(), pEdgesList.end(), e);
        auto edge = *edgeIT;
        if (edgeIT != pEdgesList.end())
        {
            pEdgesList.erase(edgeIT);

            adjency[e->getOrigin()].erase(e->getDestination());
            adjency[e->getDestination()].erase(e->getOrigin());

            delete edge;
        }
    }

    //! find a node from it's name
    NodeP findNodeFromName(std::string name)
    {
        auto nodeIT = std::find_if(pNodesList.begin(),
                                   pNodesList.end(),
                                   [&name](NodeP& n) -> bool { return n->getName() == name; });
        if (nodeIT != pNodesList.end())
        {
            return *nodeIT;
        }
        else
        {
            return nullptr;
        }
    }

    /*!
    ** \brief Kruskal algorithm
    **
    ** Search for the minimum spanning tree of the graph
    ** Saved in pMinSpanningTree
    */
    void kruskal();

    /*!
    ** \brief Get minimum spanning tree
    **
    */
    /*VectorEdgeData getMinSpanningTree()
    {
            VectorEdgeData minSpanningTree(pMinSpanningTree.size());
            auto end = pMinSpanningTree.end();
            for (auto e = pMinSpanningTree.begin(); e != end; e++)
            {
                    minSpanningTree.push_back((*e)->edgeProperties);
            }
            return minSpanningTree;
    }*/

    /*!
    ** \brief Build pMesh
    **
    ** the smallest set of loops meshing the graph
    */
    bool buildMesh();

    /*!
    ** \brief Get the min cycle basis mesh
    **
    ** the smallest set of loops meshing the graph
    */
    const std::vector<std::vector<int>>& getMeshIndexMatrix()
    {
        return meshIndexMatrix;
    }

    VectorEdgeP twoLevelPath(VectorNodeP vN);

    /*!
    ** \brief get a Gid where edges and nodes are duplicated
    **
    */
    bool getDuplicatedGrid(Grid&);

    /*!
    ** \brief get a clone of the Grid
    **
    */
    bool cloneGrid(Grid&);

    const VectorEdgeP& getEdges()
    {
        return pEdgesList;
    }

public:
    EdgeIncidence getIncidenceVector(VectorEdgeP vE)
    {
        EdgeIncidence Ei(pEdgesList.size(), false);
        for (uint i = 0; i < pEdgesList.size(); i++)
        {
            if (std::find(vE.begin(), vE.end(), pEdgesList[i]) != vE.end())
            {
                Ei[i] = true;
            }
        }
        return Ei;
    }

    EdgeIncidence getIncidenceVector(EdgeP vE)
    {
        EdgeIncidence Ei(pEdgesList.size(), false);
        for (uint i = 0; i < pEdgesList.size(); i++)
        {
            if (vE == pEdgesList[i])
            {
                Ei[i] = true;
                break;
            }
        }
        return Ei;
    }

    VectorEdgeP getEdgeVectorFromIncidence(EdgeIncidence vI)
    {
        VectorEdgeP vE;
        for (uint i = 0; i < pEdgesList.size(); i++)
        {
            if (vI[i] == true)
            {
                vE.push_back(pEdgesList[i]);
            }
        }
        return vE;
    }

    EdgeIncidence incidenceXOR(EdgeIncidence& e1, EdgeIncidence& e2)
    {
        EdgeIncidence Ei(e1.size(), false);
        std::transform(e1.begin(), e1.end(), e2.begin(), Ei.begin(), std::bit_xor<bool>());

        return Ei;
    }

    int incidenceInnerProduct(EdgeIncidence& e1, EdgeIncidence& e2)
    {
        int r1 = std::inner_product(e1.begin(), e1.end(), e2.begin(), 0);
        return r1 % 2;
    }

    void clear()
    {
        for (auto it = pNodesList.begin(); it != pNodesList.end(); it++)
        {
            delete (*it);
        }
        pNodesList.clear();

        for (auto it = pEdgesList.begin(); it != pEdgesList.end(); it++)
        {
            delete (*it);
        }
        pEdgesList.clear();

        pMinSpanningTree.clear();
        pMesh.clear();
        meshIndexMatrix.clear();
        adjency.clear();
    }

private:
    //! \definition of the graph
    //@{
    //! list of nodes
    VectorNodeP pNodesList;
    //! list of edges
    VectorEdgeP pEdgesList;
    //@}

    //! Minimum Spanning Tree of the graph
    VectorEdgeP pMinSpanningTree;
    //! Smallest set of loops meshing all the graph
    std::vector<VectorEdgeP> pMesh;

    std::vector<std::vector<int>> meshIndexMatrix;

    double inf;

    //! adjency list
    std::map<NodeP, std::map<NodeP, EdgeP>> adjency;
};

} // namespace Graph
} // namespace Antares

#include "grid.hxx"
#endif // CONSTRAINTSBUILDER_BUILDER_GRID_H

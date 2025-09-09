/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#ifndef __YUNI_CORE_TREE_N_DEPTH_SUFFIX_ITERATOR_HXX__
#define __YUNI_CORE_TREE_N_DEPTH_SUFFIX_ITERATOR_HXX__

namespace Yuni::Private::Core::Tree
{
template<class NodeT>
inline DepthSuffixIterator<NodeT>::DepthSuffixIterator():
    pNode(nullptr)
{
}

template<class NodeT>
inline DepthSuffixIterator<NodeT>::DepthSuffixIterator(const Type& it):
    pNode(it.pNode)
{
}

template<class NodeT>
template<class N>
inline DepthSuffixIterator<NodeT>::DepthSuffixIterator(const DepthSuffixIterator<N>& p):
    pNode(p.pNode)
{
}

template<class NodeT>
inline DepthSuffixIterator<NodeT>::DepthSuffixIterator(const NodePtr& p):
    pNode(p)
{
}

template<class NodeT>
inline typename DepthSuffixIterator<NodeT>::reference DepthSuffixIterator<NodeT>::operator*()
{
    return *pNode;
}

template<class NodeT>
inline typename DepthSuffixIterator<NodeT>::const_reference DepthSuffixIterator<NodeT>::operator*()
  const
{
    return *pNode;
}

template<class NodeT>
inline typename DepthSuffixIterator<NodeT>::pointer DepthSuffixIterator<NodeT>::operator->()
{
    return pNode.pointer();
}

template<class NodeT>
inline typename DepthSuffixIterator<NodeT>::const_pointer DepthSuffixIterator<NodeT>::operator->()
  const
{
    return pNode.pointer();
}

template<class NodeT>
template<class N>
inline bool DepthSuffixIterator<NodeT>::equals(const DepthSuffixIterator<N>& rhs) const
{
    return pNode == rhs.pNode;
}

template<class NodeT>
template<class N>
inline void DepthSuffixIterator<NodeT>::reset(const DepthSuffixIterator<N>& rhs)
{
    pNode = rhs.pNode;
}

template<class NodeT>
void DepthSuffixIterator<NodeT>::forward()
{
    // If we are on the root, stop
    if (!pNode->parent())
    {
        pNode = nullptr;
        return;
    }
    // If this is the right-most child, go to the parent
    if (!pNode->nextSibling())
    {
        pNode = pNode->parent();
        return;
    }
    // Go to the next sibling
    pNode = pNode->nextSibling();
    // While we can walk down the tree, do it
    while (!pNode->empty())
    {
        pNode = pNode->firstChild();
    }
    // Once we reached a leaf, stop
}

template<class NodeT>
void DepthSuffixIterator<NodeT>::forward(difference_type n)
{
    // FIXME : There is possibility to optimize this a lot
    while (n--)
    {
        forward();
    }
}

template<class NodeT>
void DepthSuffixIterator<NodeT>::backward()
{
    // Return the right-most child when possible
    if (!pNode->empty())
    {
        pNode = pNode->lastChild();
        return;
    }
    // If we reached a leaf, do the siblings
    NodePtr previous = pNode->previousSibling();
    if (previous)
    {
        pNode = previous;
        return;
    }
    // Climb back the parents until we find siblings
    while (pNode->parent() && !pNode->parent()->previousSibling())
    {
        pNode = pNode->parent();
    }
    // If there is still no sibling, it means we reached
    // the left-most sibling of the root, which means we have finished.
    if (!pNode->previousSibling())
    {
        pNode = nullptr;
        return;
    }
    // Otherwise, the previous sibling we found is the next node to traverse
    pNode = pNode->parent()->previousSibling();
}

template<class NodeT>
void DepthSuffixIterator<NodeT>::backward(difference_type n)
{
    // FIXME : There is possibility to optimize this a lot
    while (n--)
    {
        backward();
    }
}

} // namespace Yuni::Private::Core::Tree

#endif // __YUNI_CORE_TREE_N_DEPTH_SUFFIX_ITERATOR_HXX__

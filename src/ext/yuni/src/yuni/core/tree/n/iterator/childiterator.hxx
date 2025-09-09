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
#ifndef __YUNI_CORE_TREE_N_CHILD_ITERATOR_HXX__
#define __YUNI_CORE_TREE_N_CHILD_ITERATOR_HXX__

namespace Yuni::Private::Core::Tree
{
template<class NodeT>
inline ChildIterator<NodeT>::ChildIterator():
    pNode(nullptr)
{
}

template<class NodeT>
inline ChildIterator<NodeT>::ChildIterator(const Type& it):
    pNode(it.pNode)
{
}

template<class NodeT>
template<class N>
inline ChildIterator<NodeT>::ChildIterator(const ChildIterator<N>& p):
    pNode(p.pNode)
{
}

template<class NodeT>
inline ChildIterator<NodeT>::ChildIterator(const NodePtr& p):
    pNode(p)
{
}

template<class NodeT>
inline typename ChildIterator<NodeT>::reference ChildIterator<NodeT>::operator*()
{
    return *pNode;
}

template<class NodeT>
inline typename ChildIterator<NodeT>::const_reference ChildIterator<NodeT>::operator*() const
{
    return *pNode;
}

template<class NodeT>
inline typename ChildIterator<NodeT>::pointer ChildIterator<NodeT>::operator->()
{
    return pNode.pointer();
}

template<class NodeT>
inline typename ChildIterator<NodeT>::const_pointer ChildIterator<NodeT>::operator->() const
{
    return pNode.pointer();
}

template<class NodeT>
template<class N>
inline bool ChildIterator<NodeT>::equals(const ChildIterator<N>& rhs) const
{
    return pNode == rhs.pNode;
}

template<class NodeT>
template<class N>
inline void ChildIterator<NodeT>::reset(const ChildIterator<N>& rhs)
{
    pNode = rhs.pNode;
}

template<class NodeT>
void ChildIterator<NodeT>::forward()
{
    pNode = pNode->nextSibling();
}

template<class NodeT>
void ChildIterator<NodeT>::forward(difference_type n)
{
    while (n--)
    {
        forward();
    }
}

template<class NodeT>
void ChildIterator<NodeT>::backward()
{
    pNode = pNode->previousSibling();
}

template<class NodeT>
void ChildIterator<NodeT>::backward(difference_type n)
{
    while (n--)
    {
        backward();
    }
}

} // namespace Yuni::Private::Core::Tree

#endif // __YUNI_CORE_TREE_N_CHILD_ITERATOR_HXX__

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
#ifndef __YUNI_CORE_TREE_N_DEPTH_SUFFIX_ITERATOR_H__
#define __YUNI_CORE_TREE_N_DEPTH_SUFFIX_ITERATOR_H__

namespace Yuni
{
namespace Private
{
namespace Core
{
namespace Tree
{
template<class NodeT>
class DepthSuffixIterator
{
public:
    typedef DepthSuffixIterator<NodeT> Type;

    typedef typename NodeT::Ptr NodePtr;

    typedef NodeT value_type;
    typedef int difference_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;

public:
    enum
    {
        canGoForward = true,
        canGoBackward = true,
    };

public:
    //! \name Constructors
    //@{
    DepthSuffixIterator();

    DepthSuffixIterator(const Type& it);

    template<class N>
    DepthSuffixIterator(const DepthSuffixIterator<N>& p);

    DepthSuffixIterator(const NodePtr& p);
    //@}

    //! \name Static overloads from IIterator
    //@{
    void forward();

    void forward(difference_type n);

    void backward();

    void backward(difference_type n);

    void advance(difference_type n);

    template<class N>
    bool equals(const DepthSuffixIterator<N>& rhs) const;

    template<class N>
    void reset(const DepthSuffixIterator<N>& rhs);

    template<class N>
    difference_type distance(const DepthSuffixIterator<N>& rhs) const;
    //@}

    //! \name Operator overloads
    //@{

    //! The operator `*`
    reference operator*();

    //! The operator `*`
    const_reference operator*() const;

    //! The operator `->`
    pointer operator->();

    //! The operator `->`
    const_pointer operator->() const;

    //@}

private:
    //! The actual data
    NodePtr pNode;
};

} // namespace Tree
} // namespace Core
} // namespace Private
} // namespace Yuni

#include "depthsuffixiterator.hxx"

#endif // __YUNI_CORE_TREE_N_DEPTH_SUFFIX_ITERATOR_H__

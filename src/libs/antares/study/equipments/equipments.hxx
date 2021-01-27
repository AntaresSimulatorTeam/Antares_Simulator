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
#ifndef __ANTARES_LIBS_STUDY_EQUIPMENTS_EQUIPMENTS_HXX__
#define __ANTARES_LIBS_STUDY_EQUIPMENTS_EQUIPMENTS_HXX__

namespace Antares
{
namespace Data
{
template<class ItemT>
Equipments<ItemT>::Equipments() : modified(true), pUpdateCounter(0)
{
}

template<class ItemT>
Equipments<ItemT>::Equipments(const Equipments<ItemT>& rhs) :
 modified(true), pItems(rhs.pItems), pUpdateCounter(0)
{
}

template<class ItemT>
Equipments<ItemT>::~Equipments()
{
    // we should delete all elements as soon as possible to avoid
    // using a corrupt vtable from the elements
    // however it should change nothing in most cases since not all
    // elements use reference to their container
    pItems.clear();
}

template<class ItemT>
inline Equipments<ItemT>& Equipments<ItemT>::operator=(const Equipments& rhs)
{
    modified = true;
    pItems = rhs.pItems;
    return *this;
}

template<class ItemT>
template<class T>
inline Equipments<ItemT>& Equipments<ItemT>::operator+=(const T& element)
{
    add(element);
    return *this;
}

template<class ItemT>
template<class T>
inline Equipments<ItemT>& Equipments<ItemT>::operator-=(const T& element)
{
    remove(element);
    return *this;
}

template<class ItemT>
inline typename Equipments<ItemT>::ItemType& Equipments<ItemT>::operator[](uint index)
{
    assert(index < pItems.size() && "equipments: operator[]: index out of bounds");
    return *(pItems[index]);
}

template<class ItemT>
inline const typename Equipments<ItemT>::ItemType& Equipments<ItemT>::operator[](uint index) const
{
    assert(index < pItems.size() && "equipments: operator[]: index out of bounds");
    return *(pItems[index]);
}

template<class ItemT>
inline void Equipments<ItemT>::reserve(uint count)
{
    pItems.reserve(count);
}

template<class ItemT>
inline typename Equipments<ItemT>::iterator Equipments<ItemT>::begin()
{
    return pItems.begin();
}

template<class ItemT>
inline typename Equipments<ItemT>::const_iterator Equipments<ItemT>::begin() const
{
    return pItems.begin();
}

template<class ItemT>
inline typename Equipments<ItemT>::const_iterator Equipments<ItemT>::cbegin() const
{
    return pItems.cbegin();
}

template<class ItemT>
inline typename Equipments<ItemT>::iterator Equipments<ItemT>::end()
{
    return pItems.end();
}

template<class ItemT>
inline typename Equipments<ItemT>::const_iterator Equipments<ItemT>::end() const
{
    return pItems.end();
}

template<class ItemT>
inline typename Equipments<ItemT>::const_iterator Equipments<ItemT>::cend() const
{
    return pItems.cend();
}

template<class ItemT>
inline uint Equipments::size() const
{
    return (uint)pItems.size();
}

template<class ItemT>
inline bool Equipments::empty() const
{
    return pItems.empty();
}

template<class ItemT>
inline void Equipments<ItemT>::clear()
{
    pItems.clear();
    modified = true;
}

template<class ItemT>
bool Equipments<ItemT>::add(const ItemType& element)
{
    if (not exists(element))
    {
        addWithoutCheck(&element);
        return true;
    }
    return false;
}

template<class ItemT>
template<class T>
bool Equipments<ItemT>::add(const ItemPtr& element)
{
    if (!(!element) && not exists(element))
    {
        addWithoutCheck(element);
        return true;
    }
    return false;
}

template<class ItemT>
void Equipments<ItemT>::addWithoutCheck(const ItemPtr& element)
{
    assert(!(!element) && "impossible to insert a null element");
    pItems.push_back(element);
    if (!pUpdateCounter)
        reindex();
}

template<class ItemT>
void Equipments<ItemT>::reindex()
{
    if (not pItems.empty())
    {
        // First, we have to reorder all items
        std::sort(pItems.begin(), pItems.end(), [](const ItemPtr& a, const ItemPtr& b) {
            return a->name() < b->name();
        });

        // then we have to re-update all indexes
        uint index = 0;
        auto end = pItems.end();
        for (auto i = pItems.begin(); i != end; ++i, ++index)
            (*i)->index = index;

        // Obvisouly, the container has been modified
        modified = true;
    }
}

template<class ItemT>
inline void Equipments<ItemT>::beginUpdate()
{
    ++pUpdateCounter;
}

template<class ItemT>
void Equipments<ItemT>::endUpdate()
{
    assert(pUpdateCounter > 0 && "beginUpdate / endUpdate mismatch");
    if (!--pUpdateCounter)
        reindex();
}

template<class ItemT>
typename Equipments<ItemT>::ItemPtr Equipments<ItemT>::find(const AnyString& text) const
{
    if (not text.empty())
    {
        auto end = pItems.end();
        for (auto i = pItems.begin(); i != end; ++i)
        {
            auto& element = *(*i);
            if (element.name().equalsInsensitive(text))
                return *i;
            if (element.id().equalsInsensitive(text))
                return *i;
        }
    }
    return nullptr;
}

template<class ItemT>
typename Equipments<ItemT>::ItemPtr Equipments<ItemT>::find(const AnyString& text)
{
    if (not text.empty())
    {
        auto end = pItems.end();
        for (auto i = pItems.begin(); i != end; ++i)
        {
            auto& element = *(*i);
            if (element.name().equalsInsensitive(text))
                return *i;
            if (element.id().equalsInsensitive(text))
                return *i;
        }
    }
    return nullptr;
}

template<class ItemT>
bool Equipments<ItemT>::exists(const AnyString& text) const
{
    if (not text.empty())
    {
        auto end = pItems.cend();
        for (auto i = pItems.cbegin(); i != end; ++i)
        {
            auto& element = *(*i);
            if (element.name().equalsInsensitive(text))
                return true;
            if (element.id().equalsInsensitive(text))
                return true;
        }
    }
    return false;
}

template<class ItemT>
bool Equipments<ItemT>::exists(const ItemPtr& test) const
{
    auto end = pItems.cend();
    for (auto i = pItems.cbegin(); i != end; ++i)
    {
        if (*i == test)
            return true;
    }
    return false;
}

template<class ItemT>
bool Equipments<ItemT>::exists(const ItemType* test) const
{
    auto end = pItems.cend();
    for (auto i = pItems.cbegin(); i != end; ++i)
    {
        if (*i == test)
            return true;
    }
    return false;
}

template<class ItemT>
inline bool Equipments<ItemT>::exists(const ItemType& test) const
{
    return exists(&test);
}

template<class ItemT>
inline uint Equipments<ItemT>::remove(const ItemType& element)
{
    return remove(&element);
}

template<class ItemT>
inline uint Equipments<ItemT>::remove(const ItemType* element)
{
    return removeIf([&](const ItemType& rhs) { return element == rhs; });
}

template<class ItemT>
uint Equipments<ItemT>::remove(const ItemPtr& element)
{
    return removeIf([&](const ItemType& rhs) { return element == rhs; });
}

template<class ItemT>
uint Equipments<ItemT>::remove(const AnyString& text)
{
    return removeIf([&](const ItemType& element) {
        return (element.name().equalsInsensitive(text) || element.id().equalsInsensitive(text))
    });
}

template<class ItemT>
template<class P>
uint Equipments<ItemT>::removeIf(const P& predicate)
{
    uint count = 0;
    pItems.erase(std::remove_if(pItems.begin(),
                                p.end(),
                                [&](const ItemType* rhs) -> bool {
                                    if (predicate(*rhs))
                                    {
                                        ++count;
                                        return true;
                                    }
                                    return false;
                                }),
                 pItems.end());

    if (0 != count)
    {
        modified = true;
        return count;
    }
    return 0;
}

template<class ItemT>
template<class T>
inline void Equipments<ItemT>::each(const T& functor)
{
    auto end = pItems.end();
    for (auto i = pItems.begin(); i != end; ++i)
        functor(*(*i));
}

template<class ItemT>
template<class StringT>
bool Equipments<ItemT>::findNonExistingName(StringT& out,
                                            const AnyString& prefix,
                                            bool emptybefore) const
{
    if (emptybefore)
        out.clear();

    // trivial solution
    if (not exists(prefix))
    {
        out += prefix;
        return;
    }

    YString text;
    uint index = 1; // "copy", "copy 2", "copy 3", ...
    do
    {
        text.clear() << prefix << ' ' << (++index);
        if (not exists(text))
        {
            out += text;
            return true;
        }
        if (index > 2147483648)
        {
            assert(false && "findNonExistingName: help me !");
            break;
        }
    } while (true); // infinite
    return false;
}

template<class ItemT>
Yuni::uint64 Equipments<ItemT>::memoryUsage() const
{
    Yuni::uint64 amount = sizeof(*this);
    auto end = pItems.end();
    for (auto i = pItems.begin(); i != end; ++i)
        amount += (*i)->memoryUsage();
    return amount;
}

} // namespace Data
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_EQUIPMENTS_EQUIPMENTS_H__

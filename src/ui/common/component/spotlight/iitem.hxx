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
#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_IITEM_HXX__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_IITEM_HXX__

namespace Antares
{
namespace Component
{
inline const Spotlight::IItem::GroupType& Spotlight::IItem::group() const
{
    return pGroup;
}

template<class StringT>
inline bool Spotlight::IItem::isSameGroup(const StringT& group) const
{
    return (pGroup == group);
}

inline const Yuni::String& Spotlight::IItem::caption() const
{
    return pCaption;
}

template<class StringT>
inline void Spotlight::IItem::caption(const StringT& caption)
{
    pCaption = caption;
    pCacheCaption = wxStringFromUTF8(pCaption);
}

inline const Yuni::String& Spotlight::IItem::subcaption() const
{
    return pSubCaption;
}

template<class StringT>
inline void Spotlight::IItem::subcaption(const StringT& caption)
{
    pSubCaption = caption;
}

template<class StringT>
inline void Spotlight::IItem::group(const StringT& g)
{
    pGroup = g;
}

inline bool Spotlight::IItem::canBeSelected() const
{
    return true;
}

inline uint Spotlight::IItem::IPart::width() const
{
    return 0u;
}

inline uint Spotlight::IItem::IPart::height() const
{
    return 0u;
}

inline const wxBitmap& Spotlight::IItem::image() const
{
    return pBitmap;
}

inline void Spotlight::IItem::image(const wxBitmap& bmp)
{
    pBitmap = bmp;
}

inline void Spotlight::IItem::image(const wxBitmap* bmp)
{
    if (bmp)
        pBitmap = *bmp;
}

inline bool Spotlight::IItem::selected() const
{
    return pSelected;
}

inline void Spotlight::IItem::select()
{
    pSelected = true;
}

inline void Spotlight::IItem::unselect()
{
    pSelected = false;
}

template<class StringT>
inline void Spotlight::IItem::addTag(const StringT& text)
{
    pLeftTags.push_back(new Tag(text));
}

template<class StringT>
inline void Spotlight::IItem::addTag(const StringT& text,
                                     Yuni::uint8 r,
                                     Yuni::uint8 g,
                                     Yuni::uint8 b)
{
    pLeftTags.push_back(new Tag(text, r, g, b));
}

template<class StringT>
inline void Spotlight::IItem::addRightTag(const StringT& text)
{
    pRightTags.push_back(new Tag(text));
}

template<class StringT>
inline void Spotlight::IItem::addRightTag(const StringT& text,
                                          Yuni::uint8 r,
                                          Yuni::uint8 g,
                                          Yuni::uint8 b)
{
    pRightTags.push_back(new Tag(text, r, g, b));
}

inline void Spotlight::IItem::clearTags()
{
    pLeftTags.clear();
    pRightTags.clear();
}

inline bool Spotlight::IItem::countedAsResult() const
{
    return pCountedAsResult;
}

inline void Spotlight::IItem::countedAsResult(bool v)
{
    pCountedAsResult = v;
}

} // namespace Component
} // namespace Antares

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_IITEM_HXX__

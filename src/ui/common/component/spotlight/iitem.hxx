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
    pLeftTags.push_back(std::make_shared<Tag>(text));
}

template<class StringT>
inline void Spotlight::IItem::addTag(const StringT& text,
                                     uint8_t r,
                                     uint8_t g,
                                     uint8_t b)
{
    pLeftTags.push_back(std::make_shared<Tag>(text, r, g, b));
}

template<class StringT>
inline void Spotlight::IItem::addRightTag(const StringT& text)
{
    pRightTags.push_back(std::make_shared<Tag>(text));
}

template<class StringT>
inline void Spotlight::IItem::addRightTag(const StringT& text,
                                          uint8_t r,
                                          uint8_t g,
                                          uint8_t b)
{
    pRightTags.push_back(std::make_shared<Tag>(text, r, g, b));
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

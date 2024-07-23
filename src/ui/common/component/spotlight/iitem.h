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
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_IITEM_H__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_IITEM_H__

#include <memory>

class IItem
{
public:
    //! Ptr
    using Ptr = std::shared_ptr<IItem>;
    //! Vector of items
    using Vector = std::vector<Ptr>;
    //! Vector Ptr
    using VectorPtr = std::shared_ptr<Vector>;
    //! Group
    using GroupType = Yuni::CString<64, false>;

    class IPart
    {
    public:
        IPart()
        {
        }
        virtual ~IPart()
        {
        }

        //! Width of the cell
        virtual uint width() const;
        //! Height of the cell
        virtual uint height() const;

    }; // background color

    class Tag
    {
    public:
        //! The most suitable smart pointer
        using Ptr = std::shared_ptr<Tag>;
        //! Vector
        using Vector = std::vector<Ptr>;

    public:
        template<class StringT>
        Tag(const StringT& t) : text(wxStringFromUTF8(t))
        {
        }
        Tag(const wxChar* t) : text(t)
        {
        }
        Tag(const wxString& t) : text(t)
        {
        }
        template<class StringT>
        Tag(const StringT& t, uint8_t r, uint8_t g, uint8_t b) :
         text(wxStringFromUTF8(t)), color(r, g, b)
        {
        }
        Tag(const wxChar* t, uint8_t r, uint8_t g, uint8_t b) : text(t), color(r, g, b)
        {
        }
        Tag(const wxString& t, uint8_t r, uint8_t g, uint8_t b) :
         text(t), color(r, g, b)
        {
        }

    public:
        //! Text
        wxString text;
        //! Tag color
        Yuni::Color::RGB<> color;
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    IItem();
    //! Destructor
    virtual ~IItem();
    //@}

    //! \name Caption
    //@{
    //! Get the caption
    const Yuni::String& caption() const;
    //! Set the caption
    template<class StringT>
    void caption(const StringT& caption);
    //! Set the caption
    void caption(const wxString& caption);

    //! Get the sub-caption
    const Yuni::String& subcaption() const;
    //! Set the caption
    template<class StringT>
    void subcaption(const StringT& caption);

    //@}

    //! \name Group
    //@{
    //! Get the group of the item
    const GroupType& group() const;
    //! Set the group
    template<class StringT>
    void group(const StringT& g);
    //! Compare with another group
    template<class StringT>
    bool isSameGroup(const StringT& group) const;
    //@}

    //! \name Selection
    //@{
    //! Get if the item can be selected
    virtual bool canBeSelected() const;
    //@}

    //! \name Image
    //@{
    //! Set the item's image (reference)
    void image(const wxBitmap& bmp);
    //! Set the item's image (pointer, can be null)
    void image(const wxBitmap* bmp);
    //! Get the item& image
    const wxBitmap& image() const;
    //@}

    //! \name Selection
    //@{
    //! Get if the item is selected
    bool selected() const;
    //! Mark the item as selected
    void select();
    //! Mark the item as not selected
    void unselect();
    //@}

    //! \name Tags
    //@{
    //! Add a new tag
    template<class StringT>
    void addTag(const StringT& text);
    //! Add a new tag
    template<class StringT>
    void addTag(const StringT& text, uint8_t r, uint8_t g, uint8_t b);
    //! Add a new tag on the right
    template<class StringT>
    void addRightTag(const StringT& text);
    //! Add a new tag on the right
    template<class StringT>
    void addRightTag(const StringT& text, uint8_t r, uint8_t g, uint8_t b);
    //! Clear all tags
    void clearTags();
    //@}

    //! \name Result
    //@{
    //! Get if this item should be counted as a result
    bool countedAsResult() const;
    //! Set if this item should be counted as a result
    void countedAsResult(bool v);
    //@}

    //! \name Drawing
    //@{
    /*!
    ** \brief Draw the item
    **
    ** \param dc The device context
    ** \param[in,out] bounds The bounds of the surface. The width and the height should be be
    *modified
    */
    virtual void draw(wxDC& dc,
                      uint itemHeight,
                      wxRect& bounds,
                      bool selected,
                      const SearchToken::VectorPtr& tokens) const;
    //@}

public:
    //! User-custom tag
    int tag;

protected:
    //! The group
    GroupType pGroup;
    //! Caption
    Yuni::String pCaption;
    //!
    Yuni::String pSubCaption;
    //! Image
    wxBitmap pBitmap;
    //! Selected
    bool pSelected;
    //! Flag to determine if this items should be counted as a result
    bool pCountedAsResult;
    //! Tags on the left
    Tag::Vector pLeftTags;
    //! Tags on the right
    Tag::Vector pRightTags;
    //! Cache wxString
    wxString pCacheCaption;

}; // class IITem

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_IITEM_H__

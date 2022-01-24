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
#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_IITEM_H__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_IITEM_H__

#include <memory>

class IItem
{
public:
    //! Ptr
    typedef std::shared_ptr<IItem> Ptr;
    //! Vector of items
    typedef std::vector<Ptr> Vector;
    //! Vector Ptr
    typedef std::shared_ptr<Vector> VectorPtr;
    //! Group
    typedef Yuni::CString<64, false> GroupType;

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
        typedef std::shared_ptr<Tag> Ptr;
        //! Vector
        typedef std::vector<Ptr> Vector;

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
        Tag(const StringT& t, Yuni::uint8 r, Yuni::uint8 g, Yuni::uint8 b) :
         text(wxStringFromUTF8(t)), color(r, g, b)
        {
        }
        Tag(const wxChar* t, Yuni::uint8 r, Yuni::uint8 g, Yuni::uint8 b) : text(t), color(r, g, b)
        {
        }
        Tag(const wxString& t, Yuni::uint8 r, Yuni::uint8 g, Yuni::uint8 b) :
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
    void addTag(const StringT& text, Yuni::uint8 r, Yuni::uint8 g, Yuni::uint8 b);
    //! Add a new tag on the right
    template<class StringT>
    void addRightTag(const StringT& text);
    //! Add a new tag on the right
    template<class StringT>
    void addRightTag(const StringT& text, Yuni::uint8 r, Yuni::uint8 g, Yuni::uint8 b);
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

// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_SEPARATOR_H__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_SEPARATOR_H__

#include <memory>

class Separator final : public IItem
{
public:
    //! Ptr
    using Ptr = std::shared_ptr<Separator>;
    //! Vector of items
    using Vector = std::vector<Ptr>;
    //! Vector Ptr
    using VectorPtr = std::shared_ptr<Vector>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Separator();
    //! Destructor
    virtual ~Separator();
    //@}

    virtual bool canBeSelected() const
    {
        return false;
    }

    virtual void draw(wxDC& dc,
                      uint itemHeight,
                      wxRect& bounds,
                      bool selected,
                      const SearchToken::VectorPtr& tokens) const;

}; // class Separator

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_SEPARATOR_H__

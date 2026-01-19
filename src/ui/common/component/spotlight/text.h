// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_TEXT_H__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_TEXT_H__

#include <memory>

class Text : public IItem
{
public:
    //! Ptr
    using Ptr = std::shared_ptr<Text>;
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
    Text();
    //! Constructor with text
    template<class StringT>
    Text(const StringT& text)
    {
        caption(text);
    }
    //! Destructor
    virtual ~Text();
    //@}

    virtual bool canBeSelected() const override
    {
        return false;
    }

}; // class Text

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_TEXT_H__

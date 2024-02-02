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

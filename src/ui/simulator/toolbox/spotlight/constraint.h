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
#ifndef __ANTARES_TOOLBOX_SPOTLIGHT_CONSTRAINT_H__
#define __ANTARES_TOOLBOX_SPOTLIGHT_CONSTRAINT_H__

#include <yuni/yuni.h>
#include <ui/common/component/spotlight.h>
#include <antares/study/study.h>

#include <memory>

namespace Antares
{
namespace Toolbox
{
namespace Spotlight
{
class ItemConstraint : public Component::Spotlight::IItem
{
public:
    //! Smart ptr
    using Ptr = std::shared_ptr<ItemConstraint>;

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit ItemConstraint(Data::BindingConstraint* a);
    //! Destructor
    virtual ~ItemConstraint();
    //@}

public:
    Data::BindingConstraint* constraint;
};

} // namespace Spotlight
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_SPOTLIGHT_CONSTRAINT_H__
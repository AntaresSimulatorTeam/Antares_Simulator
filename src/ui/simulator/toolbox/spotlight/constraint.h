// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_SPOTLIGHT_CONSTRAINT_H__
#define __ANTARES_TOOLBOX_SPOTLIGHT_CONSTRAINT_H__

#include <yuni/yuni.h>
#include <ui/common/component/spotlight.h>
#include <antares/study/study.h>

#include <memory>

namespace Antares::Toolbox::Spotlight
{
class ItemConstraint: public Component::Spotlight::IItem
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

} // namespace Antares::Toolbox::Spotlight

#endif // __ANTARES_TOOLBOX_SPOTLIGHT_CONSTRAINT_H__

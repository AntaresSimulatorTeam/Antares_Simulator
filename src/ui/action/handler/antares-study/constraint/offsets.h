/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_OFFSETS_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_OFFSETS_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Constraint
{
class Offsets : public IAction
{
public:
    //! The most suitable smart ptr for the class
    using Ptr = IAction::Ptr;
    //! The threading policy
    using ThreadingPolicy = IAction::ThreadingPolicy;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Offsets(const AnyString& name, Antares::Data::ConstraintName targetName = "");

    //! Destructor
    virtual ~Offsets();
    //@}

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);
    void translate(Antares::Data::AreaName& out, const Antares::Data::AreaName& original);
    void toLower(Antares::Data::AreaName& out, const Antares::Data::AreaName& original);

private:
    //! From
    Antares::Data::ConstraintName pOriginalConstraintName;
    Antares::Data::ConstraintName targetName;
    Context* pCurrentContext;

}; // class IAction

} // namespace Constraint
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_OFFSETS_H__

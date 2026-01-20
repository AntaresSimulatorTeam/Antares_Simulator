// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_COMMENTS_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_COMMENTS_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::Constraint
{
class Comments: public IAction
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
    template<class StringT>
    Comments(const StringT& name);

    //! Destructor
    virtual ~Comments();
    //@}

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

private:
    //! From
    Antares::Data::ConstraintName pOriginalConstraintName;

}; // class IAction

} // namespace Antares::Action::AntaresStudy::Constraint

#include "comments.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_COMMENTS_H__

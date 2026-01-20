// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_WEIGHTS_H__
#define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_WEIGHTS_H__

#include <yuni/yuni.h>
#include <action/action.h>

namespace Antares::Action::AntaresStudy::Constraint
{
class Weights: public IAction
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
    Weights(const AnyString& name, Antares::Data::ConstraintName targetName = "");

    //! Destructor
    virtual ~Weights();
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

} // namespace Antares::Action::AntaresStudy::Constraint

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_WEIGHTS_H__

// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_TEXT_H__
#define __ANTARES_LIBS_STUDY_ACTION_TEXT_H__

#include <yuni/yuni.h>
#include "action.h"

namespace Antares::Action
{
/*!
** \brief A dummy action, for displaying some text
*/
template<bool AutoExpandT = true>
class Text: public IAction
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
    explicit Text(const StringT& text);
    //! Destructor
    virtual ~Text();
    //@}

    virtual bool bold() const;

    virtual bool autoExpand() const;

    virtual bool canDoSomething() const;

protected:
    virtual bool prepareWL(Context& ctx);
    virtual bool performWL(Context& ctx);

}; // class IAction

} // namespace Antares::Action

#include "text.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_TEXT_H__

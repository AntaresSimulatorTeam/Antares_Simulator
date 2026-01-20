// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_ACTION_ACTION_H__
#define __ANTARES_LIBS_STUDY_ACTION_ACTION_H__

#include <yuni/yuni.h>
#include <yuni/core/tree/treeN.h>
#include <yuni/core/string.h>
#include "fwd.h"
#include <antares/study/study.h>
#include <vector>

namespace Antares::Action
{
class Context;

class IAction: public Yuni::Core::TreeN<IAction>
{
public:
    //! The ancestor
    using AncestorType = Yuni::Core::TreeN<IAction>;
    //! The most suitable smart ptr for the class
    using Ptr = AncestorType::Ptr;
    //! The threading policy
    using ThreadingPolicy = AncestorType::ThreadingPolicy;

    //! Vector
    using Vector = std::vector<Ptr>;

    //! iterator
    using iterator = AncestorType::iterator;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    IAction();
    //! Destructor
    virtual ~IAction();
    //@}

    /*!
    ** \brief Get the caption of the action
    */
    Yuni::String caption() const;

    /*!
    ** \brief Get the message of the action
    */
    Yuni::String message() const;

    /*!
    ** \brief Make preparations for the execution
    */
    bool prepare(Context& ctx);

    /*!
    ** \brief Register if possible the action into the appropriate view
    */
    void registerViews(Context& ctx);

    void datagridCaption(Yuni::String& title);

    /*!
    ** \brief Look for the root node and make preparations for the execution
    */
    bool prepareRootNode(Context& ctx);

    /*!
    ** \brief Perform the action
    */
    bool perform(Context& ctx, bool recursive = true);

    /*!
    ** \brief Dump the tree from this node to the stdcout
    */
    void dump() const;

    /*!
    ** \brief Get if the action should be represented with a bold font
    */
    virtual bool bold() const;

    /*!
    ** \brief Auto-expand
    */
    virtual bool autoExpand() const;

    /*!
    **
    */
    virtual bool canDoSomething() const;

    virtual bool allowUpdate() const;
    virtual bool allowSkip() const;
    virtual bool allowOverwrite() const;
    virtual bool shouldPrepareRootNode() const;

    //! Get if the action should be visible to the user
    virtual bool visible() const;

    //! \name Behavior
    //@{
    //! Get the behavior
    Behavior behavior() const;
    //! Set the behavior
    void behavior(Behavior newBehavior);
    //@}

    //! Prepare the whole list of actions to execute in the given order
    bool prepareStack(Vector& vector);

    //! \name State
    //@{
    State state() const;
    //@}

    //! \name UI
    //@{
    virtual void behaviorToText(Behavior behavior, Yuni::String& out);
    //@}

    /*!
    ** \brief Create post actions if required
    */
    void createPostActions(const IAction::Ptr& node);

protected:
    //! Prepare the execution
    virtual bool prepareWL(Context& ctx) = 0;

    //! Prepare the execution (when the behavior says that the action should be skipped)
    virtual void prepareSkipWL(Context&)
    {
    }

    //! Perform the action
    virtual bool performWL(Context& ctx) = 0;

    //! Register all views
    virtual void registerViewsWL(Context&)
    {
    }

    //! Create post actions (if any)
    virtual void createPostActionsWL(const IAction::Ptr& node);

protected:
    //! All data related to the action
    ActionInformations pInfos;

private:
    void internalDump(Yuni::String& tmp, uint level) const;

}; // class IAction

} // namespace Antares::Action

#include "action.hxx"
#include "context.h"

#endif // __ANTARES_LIBS_STUDY_ACTION_ACTION_H__

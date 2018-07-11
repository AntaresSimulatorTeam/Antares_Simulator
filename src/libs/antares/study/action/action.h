/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_STUDY_ACTION_ACTION_H__
# define __ANTARES_LIBS_STUDY_ACTION_ACTION_H__

# include <yuni/yuni.h>
# include <yuni/core/tree/treeN.h>
# include <yuni/core/string.h>
# include "fwd.h"
# include "../study.h"
# include <vector>


namespace Antares
{
namespace Action
{
	class Context;


	class IAction : public Yuni::Core::TreeN<IAction>
	{
	public:
		//! The ancestor
		typedef Yuni::Core::TreeN<IAction>  AncestorType;
		//! The most suitable smart ptr for the class
		typedef AncestorType::Ptr  Ptr;
		//! The threading policy
		typedef AncestorType::ThreadingPolicy  ThreadingPolicy;

		//! Vector
		typedef std::vector<Ptr> Vector;

		//! iterator
		typedef AncestorType::iterator  iterator;

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
		virtual void prepareSkipWL(Context&) {}
		//! Perform the action
		virtual bool performWL(Context& ctx) = 0;
		//! Register all views
		virtual void registerViewsWL(Context&) {}
		//! Create post actions (if any)
		virtual void createPostActionsWL(const IAction::Ptr& node);

	protected:
		//! All data related to the action
		ActionInformations pInfos;

	private:
		void internalDump(Yuni::String& tmp, uint level) const;

	}; // class IAction





} // namespace Action
} // namespace Antares

# include "context.h"
# include "action.hxx"

#endif // __ANTARES_LIBS_STUDY_ACTION_ACTION_H__

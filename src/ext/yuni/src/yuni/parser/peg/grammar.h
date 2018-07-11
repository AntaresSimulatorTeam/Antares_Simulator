/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "../../yuni.h"
#include "../../core/string.h"
#include "../../core/noncopyable.h"
#include "../../core/event/event.h"
#include "node.h"
#include <iosfwd>


namespace Yuni
{
namespace Parser
{
namespace PEG
{


	class Grammar final : private NonCopyable<Grammar>
	{
	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		Grammar();
		//! Destructor
		~Grammar();
		//@}


		//! clear
		void clear();

		//! Load a grammar file
		bool loadFromFile(const AnyString& filename);

		//! Load a grammar from a string content
		bool loadFromString(const AnyString& content);

		//! Export as DOT file
		void exportToDOT(Clob& out) const;
		//! Export to C++
		bool exportToCPP(const AnyString& rootfilename, const AnyString& name) const;

		//! print the whole grammar to cout
		void print(std::ostream& out) const;


	public:
		//! Event: warning
		Event<void (const AnyString&)> onWarning;
		//! Event: error
		Event<void (const AnyString&)> onError;

	private:
		bool loadFromData(const AnyString& content, const AnyString& source);

	private:
		//! Rules
		Node::Map pRules;

	}; // class Grammar





} // namespace PEG
} // namespace Parser
} // namespace Yuni

std::ostream& operator << (std::ostream& out, const Yuni::Parser::PEG::Grammar& grammar);

#include "grammar.hxx"


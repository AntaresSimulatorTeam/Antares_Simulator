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
#include <map>
#include <iosfwd>



namespace Yuni
{
namespace Parser
{
namespace PEG
{

	class Node final
	{
	public:
		enum Type
		{
			asRule,
			asString,
			asSet,
			asAND,
			asOR,
		};

	public:
		//! List of nodes
		typedef std::vector<Node> Vector;
		//! Map of nodes
		typedef std::map<String, Node> Map;

	public:
		Node();

		void clear();
		bool checkRules(AnyString& error, const Node::Map& rules, std::map<AnyString, bool>&) const;

		void exportStd(std::ostream& out, uint depth = 1) const;
		void exportDOTSubgraph(Clob& out, const Map& rules, const String& rulename) const;
		void exportCPP(Clob& out, const Map& rules, Clob::Vector& helpers, String::Vector& datatext, uint depth, bool canreturn, uint& sp) const;
		void resetIndex(uint& base);
		void resetEnumID(const AnyString& rulename);
		void resetRuleIndexesFromMap(const Node::Map& rules);
		uint treeCount() const;
		uint depth() const;
		bool isSimpleTextCapture() const;

	public:
		struct
		{
			//! negate the return value
			bool negate;
			//! Minimum number of occurences
			uint min;
			//! Maximum number of occurences
			uint max;

			void reset(uint a, uint b) {min = a; max = b;}
		}
		match;

		struct
		{
			//! Type of node
			Type type;
			//! Text or set of chars to match
			String text;
		}
		rule;

		struct
		{
			//! Flag to determine whether this rule should be inlined or not
			bool inlined;
			//! Flag to determine whether this rule contains whitespaces
			bool whitespaces;
			//! Flag to determine whether this node will capture the matching text
			bool capture;
			//! Flag to determine whether this node is important or not (affect printing only)
			bool important;
			//! Flag to determine whether this node can eat characters or not (true most of the time)
			bool canEat;
		}
		attributes;

		//! Sub nodes
		Vector children;

		//! Global index among all nodes and sub nodes
		String id;
		//! Enum ID for code generation
		String enumID;

	}; // class Node





} // namespace PEG
} // namespace Parser
} // namespace Yuni

#include "node.hxx"

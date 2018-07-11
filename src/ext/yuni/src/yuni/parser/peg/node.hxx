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
#include "node.h"


namespace Yuni
{
namespace Parser
{
namespace PEG
{

	inline Node::Node()
	{
		match.negate = false;
		match.min = 1;
		match.max = 1;
		rule.type = asRule;
		attributes.inlined = false;
		attributes.whitespaces = true;
		attributes.capture = true;
		attributes.important = false;
		attributes.canEat = true;
	}


	inline bool Node::isSimpleTextCapture() const
	{
		return (rule.type == asString) and (not rule.text.empty())
			and (not match.negate)
			and (not attributes.inlined)
			and (not attributes.important)
			and (match.min == 1 and match.min == 1);
	}


	inline uint Node::depth() const
	{
		uint depth = 0;
		for (uint i = 0; i != children.size(); ++i)
		{
			uint d = children[i].depth();
			if (d > depth)
				depth = d;
		}
		return depth + 1;
	}


	inline uint Node::treeCount() const
	{
		uint count = (uint) children.size();
		for (uint i = 0; i != children.size(); ++i)
			count += children[i].treeCount();
		return count;
	}




} // namespace PEG
} // namespace Parser
} // namespace Yuni

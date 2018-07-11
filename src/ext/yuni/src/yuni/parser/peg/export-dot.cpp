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
#include "grammar.h"


namespace Yuni
{
namespace Parser
{
namespace PEG
{


	void Grammar::exportToDOT(Clob& out) const
	{
		out << "\ndigraph {\n";

		Node::Map::const_iterator end = pRules.end();

		out << "\t// options\n";
		out << "\trankdir = LR;\n";
		out << "\tcompound = true;\n";
		out << '\n';
		out << "\tsubgraph \"cluster_:start\" {\n";
		out << "\t\tstyle = filled;\n";
		out << "\t\tlabel = \"start\";\n";
		out << "\t\tcolor = lightgrey;\n";
		out << "\t\tnode [style = filled, color = white];\n";
		out << "\t\t\":start\" [shape = diamond];\n";
		out << "\t}\n";
		out << "\tsubgraph \"cluster_:end\" {\n";
		out << "\t\tstyle = filled;\n";
		out << "\t\tlabel = \"final\";\n";
		out << "\t\tcolor = \"#bbbbff\";\n";
		out << "\t\tnode [style = filled, color = white];\n";
		out << "\t\t\"EOF\" [shape = diamond];\n";
		out << "\t}\n";
		out << "\n";

		// export rules
		out << "\t// rules\n";
		for (Node::Map::const_iterator i = pRules.begin(); i != end; ++i)
		{
			// the current rulename
			const String& rulename = i->first;
			// the node itself
			const Node& node = i->second;

			out << "\tsubgraph \"cluster-" << rulename << "\" { // " << node.enumID << '\n';
			out << "\t\tstyle = filled;\n";
			out << "\t\tlabel = \"rule:" << rulename << "\";\n";
			out << "\t\tcolor = \"#e2e2e2\";\n";
			out << "\t\tnode [style = filled, color = white];\n";
			out << '\n';
			node.exportDOTSubgraph(out, pRules, rulename);
			out << '\n';
			out << "\t} // cluster-" << rulename << '\n';
			out << "\n\n";
		}

		out << "}\n\n";
	}






} // namespace PEG
} // namespace Parser
} // namespace Yuni


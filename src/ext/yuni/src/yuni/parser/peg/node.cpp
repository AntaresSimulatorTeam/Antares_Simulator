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
#include "node.h"
#include <iostream>


namespace Yuni
{
namespace Parser
{
namespace PEG
{

	namespace // anonymous
	{

		template<class StreamT>
		static inline StreamT& PrintTabs(StreamT& out, uint depth)
		{
			assert(depth < 1000u); // arbitrary value
			assert(out.size() < 1024 * 1024 * 10);
			assert(out.capacity() < 1024 * 1024 * 10);
			for (uint i = 0; i != depth; ++i)
				out << '\t';

			assert(out.size() < 1024 * 1024 * 10);
			assert(out.capacity() < 1024 * 1024 * 10);
			return out;
		}

		static inline std::ostream& PrintTabs(std::ostream& out, uint depth)
		{
			assert(depth < 10000u); // arbitrary value
			for (uint i = 0; i != depth; ++i)
				out << '\t';
			return out;
		}


		template<class StreamT>
		static inline StreamT& PrintString(StreamT& out, const String& text)
		{
			assert(out.capacity() < 1024 * 1024 * 500); // arbitrary
			assert(out.size() < 1024 * 1024 * 500);
			String::const_utf8iterator end = text.utf8end();
			String::const_utf8iterator i   = text.utf8begin();

			for (; i != end; ++i)
			{
				if (i->size() == 1)
				{
					switch ((char) *i)
					{
						case '\n': out << "\\n"; break;
						case '\t': out << "\\t"; break;
						case '\r': out << "\\r"; break;
						case '\f': out << "\\f"; break;
						case '\v': out << "\\v"; break;
						case '"' : out << "\\\""; break;
						case '\\': out << "\\\\";break;
						default: out << *i;
					}
				}
				else
					out << *i;
			}
			return out;
		}


		template<class StreamT>
		static inline void PrintAsciiChar(StreamT& out, char c)
		{
			assert(out.capacity() < 1024 * 1024 * 500); // arbitrary
			assert(out.size() < 1024 * 1024 * 500);

			switch (c)
			{
				case '\\':  out << "\\\\";break;
				case '\'':  out << "\\\'";break;
				case '\n':  out << "\\n";break;
				case '\t':  out << "\\t";break;
				case '\r':  out << "\\r";break;
				case '\f':  out << "\\f";break;
				case '\v':  out << "\\v";break;
				default: out << c;
			}
		}



		struct AutoReset final
		{
			AutoReset(bool enabled, Clob& out, uint& depth)
				: enabled(enabled), out(out), depth(depth)
			{
				if (enabled)
				{
					out << "\n";
					PrintTabs(out, depth) << "{\n";
					++depth;
					PrintTabs(out, depth) << "// not allowed to eat :(\n";
					PrintTabs(out, depth) << "OffsetAutoReset autoreset(ctx);\n\n";
				}
			}

			~AutoReset()
			{
				if (enabled)
				{
					PrintTabs(out, depth) << "}\n";
					--depth;
				}
			}

			bool enabled;
			Clob& out;
			uint& depth;
		};



		template<class StreamT>
		static inline void PrintText(StreamT& out, const String& text)
		{
			String::const_utf8iterator end = text.utf8end();
			String::const_utf8iterator i   = text.utf8begin();

			for (; i != end; ++i)
			{
				if (i->size() == 1)
				{
					switch ((char) *i)
					{
						case '\n': out << "\\\\n"; break;
						case '\t': out << "\\\\t"; break;
						case '\r': out << "\\\\r"; break;
						case '\f': out << "\\\\f"; break;
						case '\v': out << "\\\\v"; break;
						case '"' : out << "\\\\\""; break;
						default: out << *i;
					}
				}
				else
					out << *i;
			}
		}


		static inline void PrintSubNodesIDs(Clob& out, const Node& node)
		{
			if (node.rule.type == Node::asRule and node.rule.text == "EOF")
				return;

			out << "\t\t\"" << node.id << "\" [label = \"";
			if (node.match.negate)
				out << "! ";

			switch (node.rule.type)
			{
				case Node::asAND:
				{
					out << "AND";
					break;
				}
				case Node::asOR:
				{
					out << "OR";
					break;
				}
				case Node::asString:
				{
					out << "\\\"";
					PrintText(out, node.rule.text);
					out << "\\\"";
					break;
				}
				case Node::asSet:
				{
					out << "one of \\\"";
					PrintText(out, node.rule.text);
					out << "\\\"";
					break;
				}
				case Node::asRule:
				{
					PrintText(out, node.rule.text);
					break;
				}
				default:
					PrintText(out, node.rule.text);
			}

			out << "\"";

			if (node.rule.type == Node::asAND or node.rule.type == Node::asOR)
				out << " shape = diamond";

			out << "];\n";
			for (uint i = 0; i != (uint) node.children.size(); ++i)
				PrintSubNodesIDs(out, node.children[i]);
		}


		static inline void PrintSubNodesLinks(Clob& out, const Node::Map& rules, const Node& node, const String& source, bool inverseColor)
		{
			if (node.rule.type == Node::asRule)
				return;

			if (node.rule.type == Node::asRule)
			{
				Node::Map::const_iterator i = rules.find(node.rule.text);
				if (i != rules.end())
					out << "\t\t\"" << source << "\" -> \"" << i->second.id << "\"";
				else
					out << "\t\t\"" << source << "\" -> \"" << node.id << "\"";
			}
			else
				out << "\t\t\"" << source << "\" -> \"" << node.id << "\"";

			out << ";\n";

			// relation
			if (not (node.match.min == 1 and node.match.max == 1))
			{
				out << "\t\t\"" << node.id << "\" -> \"" << node.id << "\" [";
				if (node.rule.type == Node::asRule)
					out << "lhead = \"cluster-" << node.rule.text << "\"; ";
				out << "label=<<font color=\"#FF5500\">";

				if (node.match.min == 0 and node.match.max == (uint) -1)
					out << '*' << node.id;
				else if (node.match.min == 0 and node.match.max == 1)
					out << '?';
				else if (node.match.min == 1 and node.match.max == (uint) -1)
					out << '+';
				else
				{
					out << '{' << node.match.min << ',';
					if (node.match.max == (uint) -1)
						out << 'n';
					else
						out << node.match.max;
					out << '}';
				}

				out << "</font>>];\n";
			}

			// display all subnodes
			if (not node.children.empty())
			{
				out << "\t\tsubgraph \"cluster-" << node.id << "\" {\n";
				if (inverseColor)
				{
					out << "\t\tstyle = filled;\n";
					out << "\t\tcolor = lightgrey;\n";
					out << "\t\tnode [style = filled, color = white];\n";
				}
				else
				{
					out << "\t\tstyle = filled;\n";
					out << "\t\tcolor = \"#dfdfdf\";\n";
					out << "\t\tnode [style = filled, color = lightgrey];\n";
				}

				switch (node.rule.type)
				{
					case Node::asOR:
					{
						PrintSubNodesLinks(out, rules, node.children[0], node.id, not inverseColor);
						for (uint i = 1; i < (uint) node.children.size(); ++i)
							PrintSubNodesLinks(out, rules, node.children[i], node.id, not inverseColor);
						break;
					}
					default:
					{
						PrintSubNodesLinks(out, rules, node.children[0], node.id, not inverseColor);
						for (uint i = 1; i < (uint) node.children.size(); ++i)
							PrintSubNodesLinks(out, rules, node.children[i], node.children[i - 1].id, not inverseColor);
					}
				}

				out << "\t\t}\n";
			}
		}


	} // anonymous namepace




	void Node::clear()
	{
		match.negate = false;
		match.min = 1;
		match.max = 1;
		rule.type = asRule;
		rule.text.clear();
		attributes.inlined = false;
		attributes.whitespaces = true;
		attributes.capture = true;
		attributes.important = false;
		attributes.canEat = true;
		children.clear();
	}


	void Node::resetIndex(uint& base)
	{
		id.clear() << "n" << base;
		++base;
		for (uint i = 0; i != (uint) children.size(); ++i)
			children[i].resetIndex(base);
	}


	void Node::resetRuleIndexesFromMap(const Node::Map& rules)
	{
		if (rule.type == Node::asRule)
		{
			if (rule.text == "EOF")
			{
				id = "EOF";
				enumID.clear();
			}
			else
			{
				const Node::Map::const_iterator i = rules.find(rule.text);
				if (i != rules.end())
				{
					id = (i->second).id;
					enumID = (i->second).enumID;
				}
			}
		}

		for (uint i = 0; i != (uint) children.size(); ++i)
			children[i].resetRuleIndexesFromMap(rules);
	}


	void Node::resetEnumID(const AnyString& rulename)
	{
		enumID.clear();
		enumID << "rg";

		bool maj = false;
		for (uint i = 0; i != rulename.size(); ++i)
		{
			const char c = rulename[i];
			if (c == '-' or c == '_' or c == ' ')
			{
				maj = true;
			}
			else
			{
				if (not maj)
				{
					enumID += c;
				}
				else
				{
					maj = false;
					enumID += (char) String::ToUpper(c);
				}
			}
		}

		if (enumID.size() > 2)
			enumID[2] = (char) String::ToUpper(enumID[2]);
	}


	bool Node::checkRules(AnyString& error, const Node::Map& rules, std::map<AnyString, bool>& unusedList) const
	{
		if (rule.type == asRule)
		{
			if (rule.text != '|' and rule.text != '.' and rule.text != "EOF")
			{
				if (rules.count(rule.text) == 0)
				{
					if (rule.text.empty() and children.empty()) // deals with empty rules
						return true;
					error = rule.text;
					return false;
				}
				else
					unusedList[rule.text] = true; // mark the rule as used
			}
		}

		for (uint i = 0; i != children.size(); ++i)
		{
			if (not children[i].checkRules(error, rules, unusedList))
				return false;
		}
		return true;
	}



	void Node::exportDOTSubgraph(Clob& out, const Node::Map& rules, const String& rulename) const
	{
		// display all subnodes
		out << "\t\t\"" << id << "\" [label = \"" << rulename << "\"];\n";
		for (uint i = 0; i != children.size(); ++i)
			PrintSubNodesIDs(out, children[i]);

		// display all subnodes
		if (not children.empty())
		{
			PrintSubNodesLinks(out, rules, children[0], id, false);
			for (uint i = 1; i != children.size(); ++i)
				PrintSubNodesLinks(out, rules, children[i], children[i - 1].id, false);
		}
	}



	void Node::exportCPP(Clob& out, const Map& rules, Clob::Vector& helpers, String::Vector& datatext, uint depth, bool canreturn, uint& sp) const
	{
		assert(out.capacity() < 1024 * 1024 * 500); // arbitrary
		assert(out.size() < 1024 * 1024 * 500);

		// the current node depth
		uint d = depth;
		// negate expression
		AnyString neg;
		if (match.negate) // reverse check
			neg = "not ";
		// return or break ?
		AnyString returnfalse = (canreturn) ? "return false;" : "break;";
		// temporary string for statement building
		String stmt;
		// determine whether a stack barrier is required or not
		bool safeFromComplexity = false;


		switch (rule.type)
		{
			case Node::asRule:
			{
				Node::Map::const_iterator r = rules.find(rule.text);
				if (r != rules.end())
				{
					stmt << neg << "yy" << r->second.enumID << "(ctx)";
				}
				else
				{
					assert(rule.text.empty() and "missing rule !");
					return;
				}
				break;
			}

			case Node::asString:
			{
				// matchSingleAscii is guaranted to not modify the stack if it does not match
				if (rule.text.size() > 1)
				{
					datatext.push_back(nullptr);
					uint sIX = ++sp;
					datatext.back() << "static const AnyString datatext" << sIX << "(\"";
					PrintString(datatext.back(), rule.text) << "\", " << rule.text.size() << ");";

					safeFromComplexity = true;
					stmt << neg << "ctx.matchString(datatext" << sIX << " /* ";
					String s;
					PrintString(s, rule.text);
					s.replace("/", " / ");
					stmt << s << " */)";
					break;
				}
				// if equals to 1, do not break but treat it as a character set
				// break;
			}

			case Node::asSet:
			{
				safeFromComplexity = true;

				switch (rule.text.size())
				{
					case 0:
					{
						if (match.negate)
							stmt << "ctx.notMatchOneOf(\"\")";
						else
							stmt << "ctx.matchOneOf(\"\")";
						break;
					}
					case 1:
					{
						if (not match.negate)
							stmt << "ctx.matchSingleAsciiChar('";
						else
							stmt << "ctx.notMatchSingleAsciiChar('";
						PrintAsciiChar(stmt, rule.text[0]);
						stmt << "')";

						break;
					}
					default:
					{
						// not utf8 chars
						if (rule.text.utf8size() == rule.text.size())
						{
							datatext.push_back(nullptr);
							uint sIX = ++sp;
							datatext.back() << "static const AnyString datatext" << sIX << "(\"";
							PrintString(datatext.back(), rule.text) << "\", " << rule.text.size() << ");";

							if (match.negate)
								stmt << "ctx.notMatchOneOf(datatext" << sIX << " /* ";
							else
								stmt << "ctx.matchOneOf(datatext" << sIX << " /* ";

							String s;
							PrintString(s, rule.text);
							s.replace("/", " / ");
							stmt << s << " */)";
						}
						else
						{
							// Not handled yet
							PrintTabs(out, d) << "# error Sequence of chars with UTF8 chars not handled yet\n";
						}
					}
				}
				break;
			}

			case Node::asAND:
			{
				if (not match.negate and match.min == 1 and match.max == 1)
				{
					for (uint i = 0; i != (uint) children.size(); ++i)
					{
						children[i].exportCPP(out, rules, helpers, datatext, d, canreturn, sp);
						assert(out.size() < 1024 * 1024 * 10);
						assert(out.capacity() < 1024 * 1024 * 10);
					}
					return;
				}
				else
				{
					uint from = ++sp;
					sp += (uint) children.size();
					if (match.negate)
						stmt << "not (";

					uint helpersFrom = (uint) helpers.size();
					// the vector `helpers` must already have the good size
					// it is mandatory to avoid bad reference due to std::vector::resize()
					assert(helpersFrom + children.size() < helpers.capacity());
					helpers.resize(helpersFrom + (uint) children.size());

					for (uint i = 0; i != (uint) children.size(); ++i)
					{
						// The reference to helpers[X] may change (because of std::vector::resize)
						// using index instead of reference
						uint hi = i + helpersFrom;
						assert(hi < (uint) helpers.size());

						helpers[hi] << "\tstatic inline bool __helper" << (from + i) << "(Datasource& ctx)\n";
						helpers[hi] << "\t{\n";
						helpers[hi] << "\t\tTRACE(\"    :: entering helper " << (from + i) << "\");\n";
						children[i].exportCPP(helpers[hi], rules, helpers, datatext, 2, true, sp);

						helpers[hi] << "\t\treturn true;\n";
						helpers[hi] << "\t}\n";

						if (i != 0)
							stmt << " and ";
						stmt << "__helper" << (from + i) << "(ctx)";
					}
					if (match.negate)
						stmt << ')';
				}
				break;
			}

			case Node::asOR:
			{
				assert(children.size() == 2 and "invalid OR node");
				if (children.size() != 2)
					return;

				uint lsp = ++sp;
				uint osp = ++sp;
				PrintTabs(out, d) << "uint sp" << osp << " = ctx.push();\n";
				PrintTabs(out, d) << "bool rt" << lsp << " = false;\n";
				PrintTabs(out, d) << "// condition: first part\n";
				PrintTabs(out, d) << "do\n";
				PrintTabs(out, d) << "{\n";
				children[0].exportCPP(out, rules, helpers, datatext, d + 1, false, sp);
				PrintTabs(out, d) << "\trt" << lsp << " = true;\n";
				PrintTabs(out, d) << "}\n";
				PrintTabs(out, d) << "while (false);\n";

				PrintTabs(out, d) << "// condition: second part\n";
				PrintTabs(out, d) << "while (not rt" << lsp << ")\n";
				PrintTabs(out, d) << "{\n";
				PrintTabs(out, d) << "\tctx.restart(sp" << osp << ");\n";
				children[1].exportCPP(out, rules, helpers, datatext, d + 1, false, sp);
				PrintTabs(out, d) << "\trt" << lsp << " = true;\n";
				PrintTabs(out, d) << "\tbreak;\n";
				PrintTabs(out, d) << "}\n";
				out << '\n';

				PrintTabs(out, d) << "if (not rt" << lsp << ")\n";
				if (not canreturn)
				{
					PrintTabs(out, d) << "{\n";
					PrintTabs(out, d) << "\tassert(sp" << osp << " < ctx.offset and \"invalid offset if\");\n";
					PrintTabs(out, d) << "\tctx.offset = sp" << osp << "; // restore context before if\n";
				}
				PrintTabs(out, d) << '\t' << returnfalse << '\n';
				if (not canreturn)
					PrintTabs(out, d) << "}\n";
				return; // nothing to do here
				break;
			}
		}

		assert(rule.type != Node::asOR and "case already handle above");
		assert(out.size() < 1024 * 1024 * 100); // arbitrary - consistency check - 100MiB should be enough
		assert(out.capacity() < 1024 * 1024 * 100);

		AutoReset autoreset(not attributes.canEat, out, d);

		if (match.min == 0 and match.max == 1)
		{
			if (not safeFromComplexity and attributes.canEat)
			{
				uint osp = ++sp;
				PrintTabs(out, d) << "uint sp" << osp << " = ctx.push(); // 0 or 1\n";
				PrintTabs(out, d) << "if (not (" << stmt << ")) // ?\n";
				PrintTabs(out, d) << "{\n";
				PrintTabs(out, d) << "\tassert(sp" << osp << " < ctx.offset and \"invalid offset match ?\");\n";
				PrintTabs(out, d) << "\tctx.offset = sp" << osp << "; // ignore the results if it does not match\n";
				PrintTabs(out, d) << "}\n";
			}
			else
			{
				// others are guaranted to not modify the stack
				PrintTabs(out, d) << "(" << stmt << "); // ? - ignore the result if it does not match\n";
			}
			return;
		}

		if (match.min == 1)
		{
			if (not safeFromComplexity)
			{
				if (canreturn)
				{
					if (attributes.canEat)
						PrintTabs(out, d) << "ctx.push(); // 0 or 1\n";
					PrintTabs(out, d) << "if (not (" << stmt << ")) // ?\n";
					PrintTabs(out, d) << "\treturn false;\n";
				}
				else
				{
					uint osp = ++sp;
					if (attributes.canEat)
						PrintTabs(out, d) << "uint sp" << osp << " = ctx.push(); // 0 or 1\n";
					PrintTabs(out, d) << "if (not (" << stmt << ")) // ?\n";
					if (attributes.canEat)
					{
						PrintTabs(out, d) << "{\n";
						PrintTabs(out, d) << "\tassert(sp" << osp << " < ctx.offset and \"invalid offset match min == 1\");\n";
						PrintTabs(out, d) << "\tctx.offset = sp" << osp << ";\n";
						PrintTabs(out, d) << "\t" << returnfalse << '\n';
						PrintTabs(out, d) << "}\n";
					}
					else
						PrintTabs(out, d) << "\t" << returnfalse << '\n';
				}
			}
			else
			{
				// others are guaranted to not modify the stack
				PrintTabs(out, d) << "if (not (" << stmt << ")) // ?\n";
				PrintTabs(out, d) << '\t' << returnfalse << '\n';
			}
		}
		if (match.max == 1)
			return;

		if (match.min <= 1 and match.max == (uint) -1)
		{
			if (not safeFromComplexity)
			{
				uint osp = ++sp;
				if (match.min == 1)
					PrintTabs(out, d) << "do // 1 or more\n";
				else
					PrintTabs(out, d) << "do // 0 or more\n";

				PrintTabs(out, d) << "{\n";
				if (attributes.canEat)
					PrintTabs(out, d) << "\tuint sp" << osp << " = ctx.push(); // 0 or 1\n";
				PrintTabs(out, d) << "\tif (not (" << stmt << "))\n";
				if (attributes.canEat)
				{
					PrintTabs(out, d) << "\t{\n";
					PrintTabs(out, d) << "\t\tassert(sp" << osp << " < ctx.offset and \"invalid offset match *\");\n";
					PrintTabs(out, d) << "\t\tctx.offset = sp" << osp << ";\n";
					PrintTabs(out, d) << "\t\tbreak;\n";
					PrintTabs(out, d) << "\t}\n";
				}
				else
					PrintTabs(out, d) << "\t\tbreak;\n";
				PrintTabs(out, d) << "}\n";
				PrintTabs(out, d) << "while (true);\n";
			}
			else
			{
				PrintTabs(out, d) << "while (" << stmt << ") // 0-1 or more\n";
				PrintTabs(out, d) << "{}\n";
			}
			return;
		}
		assert(false and "a rule is missing !");
	}




	void Node::exportStd(std::ostream& out, uint d) const
	{
		PrintTabs(out, d);

		switch (rule.type)
		{
			case Node::asAND:
			{
				out << "AND";
				break;
			}
			case Node::asOR:
			{
				out << "OR";
				break;
			}
			case Node::asString:
			{
				out << "\\\"";
				PrintText(out, rule.text);
				out << "\\\"";
				break;
			}
			case Node::asSet:
			{
				out << "one of \\\"";
				PrintText(out, rule.text);
				out << "\\\"";
				break;
			}
			case Node::asRule:
			{
				PrintText(out, rule.text);
				break;
			}
		}

		out << '\n';

		for (uint i = 0; i != children.size(); ++i)
			children[i].exportStd(out, d + 1);
	}





} // namespace PEG
} // namespace Parser
} // namespace Yuni


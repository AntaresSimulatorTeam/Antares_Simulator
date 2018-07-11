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
#include "../../io/file.h"
#include <stack>
#include <iostream>

namespace Yuni
{
namespace Parser
{
namespace PEG
{
namespace // anonymous
{

	enum RuleType
	{
		rtRule,
		rtGroup,
		rtModifier,
		rtString,
		rtListOfChars,
	};




	typedef std::vector<std::pair<uint, String> > VectorPairYAndLine;

	class SubRulePart final
	{
	public:
		typedef std::vector<SubRulePart> Vector;
	public:
		SubRulePart() :
			type(rtRule),
			line((uint) -1)
		{}

		SubRulePart(const NullPtr&) :
			type(rtRule),
			line((uint) -1)
		{}

		bool empty() const
		{
			return (type == rtRule && text.empty());
		}

	public:
		String text;
		RuleType type;
		uint line;
		String source;

	}; // class SubRulePart


	class TempRuleInfo final
	{
	public:
		void reset()
		{
			pragmas.clear();
			subrules.clear();
		}

	public:
		VectorPairYAndLine pragmas;
		VectorPairYAndLine subrules;

	}; // class TempRuleInfo


	class RuleParser final
	{
	public:
		RuleParser(Grammar& grammar, Node::Map& rules);

		bool analyze();

		void resetIndexes();


	public:
		//! Original content
		AnyString content;
		//! Source
		AnyString source;

	private:
		bool analyzeEachRule();
		bool prepareRuleIdentifierName(AnyString& line, uint lineIndex, VectorPairYAndLine& inlinePragmas);
		bool prepareNodeFromPragmas(const AnyString& source, const AnyString& rulename, Node& node, const VectorPairYAndLine& pragmas);
		bool prepareNodeFromSubrules(const String& rulename, Node& node, SubRulePart::Vector& rules);
		bool checkForRulesExistence() const;

		bool error(String& message) const;
		void warns(String& message) const;

	private:
		//! The original grammar
		Grammar& grammar;
		//! All rules of the grammar
		Node::Map& grammarRules;
		//! Error message
		mutable String errmsg;
		//! The current line
		AnyString line;
		//! The current rule name
		String currentRuleName;

		// rules (pragma list, subrule)
		typedef std::map<String, TempRuleInfo> TempRuleList;
		TempRuleList rules;

	}; // class RuleParser




	template<class StreamT> static inline void RuleTypeToString(StreamT& out, RuleType type)
	{
		switch (type)
		{
			case rtRule:         out << "rule"; break;
			case rtGroup:        out << "group"; break;
			case rtModifier:     out << "modifier"; break;
			case rtString:       out << "string"; break;
			case rtListOfChars:  out << "set"; break;
		}
	}




	static inline bool CheckIdentifier(const AnyString& text)
	{
		if (text.empty())
			return false;

		if (String::IsDigit(text[0]))
			return false;

		for (uint i = 0; i != text.size(); ++i)
		{
			if (not String::IsAlpha(text[i]) and text[i] != '-')
				return false;
		}
		return true;
	}



	inline RuleParser::RuleParser(Grammar& grammar, Node::Map& rules) :
		grammar(grammar),
		grammarRules(rules)
	{}


	inline bool RuleParser::error(String& message) const
	{
		grammar.onError(message);
		return false;
	}


	inline void RuleParser::warns(String& message) const
	{
		grammar.onWarning(message);
	}


	inline bool RuleParser::prepareRuleIdentifierName(AnyString& line, uint lineIndex, VectorPairYAndLine& inlinePragmas)
	{
		uint offset = line.find(':');
		if (not (offset < line.size()))
		{
			errmsg.clear() << source << ':' << lineIndex << ": invalid rule name, missing ':'";
			return error(errmsg);
		}

		// automatically add inline pragmas as it were standard pragmas
		AnyString options(line, offset + 1);
		options.trim();
		if (not options.empty())
		{
			AnyString::Vector list;
			options.split(list, ", ");
			for (uint i = 0; i != (uint) list.size(); ++i)
			{
				if (list[i].startsWith("//"))
					break;
				inlinePragmas.push_back(std::make_pair(lineIndex, list[i]));
			}
		}

		line.resize(offset);
		line.trimRight();
		if (not CheckIdentifier(line))
		{
			errmsg.clear() << source << ':' << lineIndex << ": invalid rule identifier '" << line << "'";
			return error(errmsg);
		}

		return true;
	}



	static inline Node& PushNewNode(std::stack<Node*> stack)
	{
		Node& current = *stack.top();
		current.children.push_back(Node());
		return current.children.back();
	}


	static inline void PostProcessConditionalNode(Node& node)
	{
		do
		{
			bool loopAgain = false;

			for (uint i = 0; i != (uint) node.children.size(); ++i)
			{
				Node& subnode = node.children[i];
				if (subnode.rule.type == Node::asRule and subnode.rule.text == '|')
				{
					loopAgain = true;

					if (i == 0 or i == (uint) node.children.size() - 1)
					{
						// empty but not really invalid
						node.children.erase(node.children.begin() + i);
					}
					else
					{
						assert(i - 1 < (uint) node.children.size());
						assert(i + 1 < (uint) node.children.size());

						subnode.children.push_back(node.children[i - 1]);
						subnode.children.push_back(node.children[i + 1]);
						subnode.rule.type = Node::asOR;
						subnode.rule.text.clear();
						subnode.rule.text.shrink();
						node.children.erase(node.children.begin() + i + 1);
						node.children.erase(node.children.begin() + i - 1);
					}

					break;
				}
			}

			if (not loopAgain)
				break;
		}
		while (true);

		// recursive update
		for (uint i = 0; i != (uint) node.children.size(); ++i)
		{
			if (not node.children[i].children.empty())
				PostProcessConditionalNode(node.children[i]);
		}
	}


	inline void RuleParser::resetIndexes()
	{
		Node::Map::iterator end = grammarRules.end();
		uint index = 0;

		for (Node::Map::iterator i = grammarRules.begin(); i != end; ++i)
		{
			(i->second).resetIndex(index);
			(i->second).resetEnumID(i->first);
		}

		// post-fix ids for rules
		for (Node::Map::iterator i = grammarRules.begin(); i != end; ++i)
			(i->second).resetRuleIndexesFromMap(grammarRules);
	}


	inline bool RuleParser::checkForRulesExistence() const
	{
		Node::Map::const_iterator end = grammarRules.end();

		std::map<AnyString, bool> unusedList;
		for (Node::Map::const_iterator i = grammarRules.begin(); i != end; ++i)
			unusedList[i->first] = false;
		unusedList["start"] = true;
		unusedList["error"] = true;
		unusedList["unknown"] = true;
		unusedList["eol"] = true;


		bool ok = true;
		bool hasStart = false;
		AnyString errRulename;
		ShortString128 rulename;

		for (Node::Map::const_iterator i = grammarRules.begin(); i != end; ++i)
		{
			rulename = i->first;
			rulename.toLower();

			if (rulename == "start")
				hasStart = true;

			if (rulename == "eof" or rulename == "unknown" or rulename == "eol")
			{
				errmsg.clear() << "rule '" << i->first<< "': this name is reserved";
				error(errmsg);
				ok = false;
				continue;
			}

			const Node& node = i->second;
			if (not node.checkRules(errRulename, grammarRules, unusedList))
			{
				errmsg.clear() << "rule '" << i->first<< "': unknown rule reference '" << errRulename << "'";
				error(errmsg);
				ok = false;
				continue;
			}
		}

		if (not hasStart)
		{
			errmsg.clear() << "A rule named 'start' is required to be the first called rule";
			error(errmsg);
			ok = false;
		}

		for (std::map<AnyString, bool>::const_iterator i = unusedList.begin(); i != unusedList.end(); ++i)
		{
			if (not i->second)
				warns(errmsg.clear() << "rule '" << i->first << ": declared but not used");
		}

		return ok;
	}


	inline bool RuleParser::prepareNodeFromPragmas(const AnyString& source, const AnyString& rulename, Node& node, const VectorPairYAndLine& pragmas)
	{
		AnyString::Vector list;
		list.reserve(2);
		bool ok = true;

		for (uint i = 0; i != pragmas.size(); ++i)
		{
			uint line = pragmas[i].first;

			pragmas[i].second.split(list, ":");
			if (list.size() > 1)
				list[1].trim();

			if (list[0] == "whitespaces")
			{
				bool value = (list.size() == 1) or list[1].to<bool>();
				node.attributes.whitespaces = value;
				continue;
			}
			if (list[0] == "block")
			{
				if (list.size() > 1)
					warns(errmsg.clear() << source << ':' << line << ": the value has been ignored");
				node.attributes.whitespaces = false;
				continue;
			}
			if (list[0] == "hidden")
			{
				bool value = (list.size() == 1) or list[1].to<bool>();
				node.attributes.inlined = value;
				if (rulename == "start")
					error(errmsg.clear() << source << ':' << line << ": the rule 'start' can not be hidden");
				continue;
			}
			if (list[0] == "capture")
			{
				bool value = (list.size() == 1) or list[1].to<bool>();
				node.attributes.capture = value;
				continue;
			}
			if (list[0] == "notext")
			{
				if (list.size() > 1)
					warns(errmsg.clear() << source << ':' << line << ": the value has been ignored");
				node.attributes.capture = false;
				continue;
			}
			if (list[0] == "important")
			{
				bool value = (list.size() == 1) or list[1].to<bool>();
				node.attributes.important = value;
				continue;
			}

			ok = false;
			error(errmsg.clear() << source << ':' << line << ": unknown pragma '" << list[0] << "'");
		}

		return ok;
	}


	inline bool RuleParser::prepareNodeFromSubrules(const String& rulename, Node& node, SubRulePart::Vector& rules)
	{
		if (rules.empty())
			return true;

		std::stack<Node*> stack;
		stack.push(&node);
		uint firstLine = rules[0].line;
		// flag to determine whether some conditional expression has been encountered for post-process
		bool hasConditional = false;

		node.rule.type = Node::asAND;

		for (uint i = 0; i != rules.size(); ++i)
		{
			// alias
			RuleType type = rules[i].type;
			const String& text = rules[i].text;
			const String& source = rules[i].source;
			uint line = rules[i].line;

			switch (type)
			{
				case rtString:
				{
					if (not text.empty())
					{
						Node& subnode = PushNewNode(stack);
						subnode.rule.type = Node::asString;
						subnode.rule.text = text;
						break;
					}
					errmsg.clear() << source << ':' << line << ": empty string, nothing to match";
					return error(errmsg);
				}

				case rtListOfChars:
				{
					if (not text.empty())
					{
						Node& subnode = PushNewNode(stack);
						subnode.rule.type  = Node::asSet;
						subnode.match.negate = (text.first() == '^');
						if (subnode.match.negate)
						{
							uint offset = (subnode.match.negate) ? 1 : 0;
							subnode.rule.text.assign(text, text.size() - offset, offset);
						}
						else
							subnode.rule.text = text;
						break;
					}
					errmsg.clear() << source << ':' << line << ": empty set of chars";
					return error(errmsg);
				}

				case rtRule:
				{
					if (not text.empty())
					{
						if (text == '|') // internal virtual rule
						{
							errmsg.clear() << source << ':' << line << ": invalid rulename";
							return error(errmsg);
						}

						Node& subnode = PushNewNode(stack);
						subnode.rule.type = Node::asRule;
						subnode.rule.text = text;
						break;
					}
					errmsg.clear() << source << ':' << line << ": " << rulename << ": empty rule name";
					return error(errmsg);
				}

				case rtGroup:
				{
					if (text.first() == '(')
					{
						Node& subnode = PushNewNode(stack);
						subnode.rule.type = Node::asAND;
						stack.push(&subnode);
					}
					else if (text.first() == ')')
					{
						if (stack.size() > 1)
						{
							stack.pop();
						}
						else
						{
							errmsg.clear() << source << ':' << firstLine << ": " << rulename
								<< ": parenthesis mismatch, too many ')'";
							return error(errmsg);
						}
					}
					else if (text.first() == '|')
					{
						Node& current = *stack.top();
						if (current.children.empty())
						{
							// all is empty - useless declaration but not really an error
							break;
						}

						Node& subnode = PushNewNode(stack);
						subnode.rule.type = Node::asRule;
						subnode.rule.text = '|'; // virtual rule, which must be replaced later

						// require post-process
						hasConditional = true;
					}
					else
					{
						errmsg.clear() << source << ':' << firstLine << ": invalid group " << text;
						return error(errmsg);
					}
					break;
				}

				case rtModifier:
				{
					Node& subnode = *stack.top();
					if (subnode.children.empty())
					{
						errmsg.clear() << source << ':' << firstLine << ": repeat pattern without content: " << text;
						return error(errmsg);
					}
					Node& last = subnode.children.back();
					if (text == '*')
						last.match.reset(0, (uint) -1);
					else if (text == '?')
						last.match.reset(0, 1);
					else if (text == '+')
						last.match.reset(1, (uint) -1);
					else if (text == '^')
						last.attributes.canEat = false;
					else
					{
						errmsg.clear() << source << ':' << firstLine << ": invalid repeat pattern " << text;
						return error(errmsg);
					}
					break;
				}

			} // switch rule type
		} // each rule part

		if (stack.size() != 1) // '(' ')' mismatch
		{
			errmsg.clear() << source << ':' << firstLine << ": " << rulename
				<< ": parenthesis mismatch, missing " << (stack.size() - 1) << " ')'";
			return error(errmsg);
		}

		if (hasConditional)
			PostProcessConditionalNode(node);

		return true;
	}



	bool RuleParser::analyze()
	{
		// current offset
		AnyString::Size offset = 0;
		// current line
		uint lineIndex = 0;

		do
		{
			++lineIndex;
			AnyString::Size next = content.find('\n', offset);

			// re-adapt the current line
			if (next < content.size())
				line.adapt(content, next - offset, offset);
			else
				line.adapt(content, content.size() - offset, offset);

			offset = next + 1;

			if (line.empty())
				continue;

			if (not AnyString::IsSpace(line[0]))
			{
				if (not line.startsWith("//"))
				{
					// starting a new rule
					line.trimRight();

					VectorPairYAndLine inlinePragmas;
					if (not prepareRuleIdentifierName(line, lineIndex, inlinePragmas))
						return false;

					currentRuleName = line;
					if (rules.count(currentRuleName) != 0)
					{
						errmsg.clear() << source << ':' << lineIndex << ": rule '" << currentRuleName << "' already exists";
						return error(errmsg);
					}

					rules[currentRuleName].reset();
					for (uint i = 0; i != (uint) inlinePragmas.size(); ++i)
						rules[currentRuleName].pragmas.push_back(inlinePragmas[i]);
				}
				else
				{
					// one-line comments
				}
			}
			else
			{
				// continue the previous rule
				line.trim();

				if (not line.startsWith("//"))
				{
					if (not line.startsWith("@pragma "))
					{
						rules[currentRuleName].subrules.push_back(std::make_pair(lineIndex, line));
						// adding an extra space to make sure to not concatenate lines together
						rules[currentRuleName].subrules.back().second += ' ';
					}
					else
					{
						AnyString pragma(line, 8);
						rules[currentRuleName].pragmas.push_back(std::make_pair(lineIndex, pragma));
					}
				}
				else
				{
					// one-line comments
				}
			}
		}
		while (offset < content.size());

		if (not analyzeEachRule())
			return false;

		if (not checkForRulesExistence())
			return false;

		return true;
	}


	bool RuleParser::analyzeEachRule()
	{
		enum State
		{
			stDefault,
			stInString,
			stInCharList
		};
		// each rule
		TempRuleList::iterator end = rules.end();
		TempRuleList::iterator i = rules.begin();

		SubRulePart::Vector newsubrules;
		newsubrules.push_back(nullptr);
		// flag to determine whether an error has been encountered
		bool result = true;

		for (; i != end; ++i)
		{
			currentRuleName = i->first;
			newsubrules.clear();
			newsubrules.push_back(nullptr);
			State state = stDefault;
			uint instringStartUtf8 = 0;
			bool hasBackslash = false;
			uint bracketDepth = 0;
			char stringQuote = '\0';
			bool localerror = false;
			const VectorPairYAndLine& pragmas = i->second.pragmas;

			// for each line of the rule
			VectorPairYAndLine::const_iterator send = i->second.subrules.end();
			VectorPairYAndLine::const_iterator si   = i->second.subrules.begin();
			for (; si != send; ++si)
			{
				uint lineIndex = si->first;
				String::const_utf8iterator cend = si->second.utf8end();
				String::const_utf8iterator ci   = si->second.utf8begin();
				uint offset = 0;
				uint offsetutf8 = 0;

				for (; ci != cend; ++ci)
				{
					++offsetutf8;
					offset += ci->size();
					char c = *ci;

					switch (state)
					{
						case stDefault:
						{
							switch (c)
							{
								case '\'':
								// case '"':
								{
									state = stInString;
									hasBackslash = false;
									instringStartUtf8 = offsetutf8;
									if (not newsubrules.back().empty())
										newsubrules.push_back(nullptr);
									stringQuote = c;
									newsubrules.back().type = rtString;
									newsubrules.back().line = lineIndex;
									newsubrules.back().source = source;
									break;
								}
								case '[':
								{
									state = stInCharList;
									hasBackslash = false;
									instringStartUtf8 = offsetutf8;
									bracketDepth = 1;
									if (not newsubrules.back().empty())
										newsubrules.push_back(nullptr);
									newsubrules.back().type = rtListOfChars;
									newsubrules.back().line = lineIndex;
									newsubrules.back().source = source;
									break;
								}
								case '^':
								case '*':
								case '+':
								case '?':
								{
									if (not newsubrules.back().empty())
										newsubrules.push_back(nullptr);
									newsubrules.back().text = c;
									newsubrules.back().type = rtModifier;
									newsubrules.back().line = lineIndex;
									newsubrules.back().source = source;
									newsubrules.push_back(nullptr);
									break;
								}
								case '|':
								case '(':
								case ')':
								{
									if (not newsubrules.back().empty())
										newsubrules.push_back(nullptr);
									newsubrules.back().text = c;
									newsubrules.back().type = rtGroup;
									newsubrules.back().line = lineIndex;
									newsubrules.back().source = source;
									newsubrules.push_back(nullptr);
									break;
								}
								case '\t':
								case ' ':
								case '\r':
								case '\n':
								{
									if (not newsubrules.back().empty())
										newsubrules.push_back(nullptr);
									break;
								}
								case '/': // comments ?
								{
									String::const_utf8iterator slash = ci;
									++slash;
									if (slash != cend)
									{
										char nextValue = *slash;
										if (nextValue == '/')
										{
											ci = cend; // stop parsing
											break;
										}
									}
								}
								default:
								{
									newsubrules.back().text += *ci;
									newsubrules.back().line = lineIndex;
									break;
								}
							}
							break;
						}
						case stInString:
						{
							// in string
							switch (c)
							{
								//case '"':
								case '\'':
								{
									if (c == stringQuote)
									{
										if (hasBackslash)
										{
											newsubrules.back().text += stringQuote;
											hasBackslash = false;
										}
										else
										{
											if (not newsubrules.back().empty())
											{
												newsubrules.push_back(nullptr);
											}
											else
												warns(errmsg.clear() << source << ':' << lineIndex << ": empty string at " << instringStartUtf8);

											state = stDefault;
										}
									}
									break;
								}
								case '\\':
								{
									if (hasBackslash)
									{
										newsubrules.back().text += '\\';
										hasBackslash = false;
									}
									else
										hasBackslash = true;
									break;
								}
								default:
								{
									if (hasBackslash)
									{
										switch (c)
										{
											case '\\': newsubrules.back().text += '\\';break;
											case 't': newsubrules.back().text += '\t';break;
											case 'n': newsubrules.back().text += '\n';break;
											case 'r': newsubrules.back().text += '\r';break;
											case 'v': newsubrules.back().text += '\v';break;
											case 'f': newsubrules.back().text += '\f';break;
											default:
											{
												error(errmsg.clear() << source << ':' << lineIndex << ": invalid escape sequence at " << offsetutf8);
												return false;
											}
										}
										hasBackslash = false;
									}
									else
									{
										newsubrules.back().text.append(*ci);
									}
								}
							}
							break;
						}

						case stInCharList:
						{
							// in a list of chars
							switch (c)
							{
								case '[':
								{
									if (hasBackslash)
										hasBackslash = false;
									//else
									//	++bracketDepth;

									newsubrules.back().text += '[';
									break;
								}
								case ']':
								{
									if (hasBackslash)
									{
										newsubrules.back().text += ']';
										hasBackslash = false;
									}
									else
									{
										if (--bracketDepth == 0)
										{
											if (not newsubrules.back().empty())
											{
												newsubrules.push_back(nullptr);
											}
											else
												warns(errmsg.clear() << source << ':' << lineIndex << ": empty list of chars at " << instringStartUtf8);

											state = stDefault;
										}
										else
										newsubrules.back().text += ']';
									}
									break;
								}
								case '\\':
								{
									if (hasBackslash)
									{
										newsubrules.back().text += '\\';
										hasBackslash = false;
									}
									else
										hasBackslash = true;
									break;
								}
								default:
								{
									if (hasBackslash)
									{
										switch (c)
										{
											case '\\': newsubrules.back().text += '\\';break;
											case 't': newsubrules.back().text += '\t';break;
											case 'n': newsubrules.back().text += '\n';break;
											case 'r': newsubrules.back().text += '\r';break;
											case 'v': newsubrules.back().text += '\v';break;
											case 'f': newsubrules.back().text += '\f';break;
											default:
											{
												// continue on error
												error(errmsg.clear() << source << ':' << lineIndex << ": invalid escape sequence at " << offsetutf8);
												result = false;
											}
										}
										hasBackslash = false;
									}
									else
										newsubrules.back().text.append(*ci);
								}
							}

							break;
						}

					} // switch state

				} // each utf8 char

				if (state == stInString)
				{
					error(errmsg.clear() << source << ':' << lineIndex << ": unfinished string at offset " << instringStartUtf8);
					localerror = true;
					break;
				}
				if (state == stInCharList)
				{
					error(errmsg.clear() << source << ':' << lineIndex << ": unfinished list of char at offset "
						  << instringStartUtf8 << ", got " << newsubrules.back().text);
					localerror = true;
					break;
				}

				if (localerror)
					break; // abort

			} // each subrule

			if (not localerror)
			{
				// remove the empty last item
				if (newsubrules.back().empty())
					newsubrules.pop_back();

				// preparing rules and pragmas
				{
					Node& node = grammarRules[currentRuleName];
					node.clear();

					if (not prepareNodeFromPragmas(source, currentRuleName, node, pragmas))
						result = false; // continue on errors
					if (not prepareNodeFromSubrules(currentRuleName, node, newsubrules))
						result = false; // continue on errors
				}
			}
			else
				result = false;
		}

		return result;
	}




} // anonymous namespace






	Grammar::Grammar()
	{
	}


	Grammar::~Grammar()
	{
	}


	bool Grammar::loadFromFile(const AnyString& filename)
	{
		Clob content;
		if (IO::errNone != IO::File::LoadFromFile(content, filename))
			return false;

		// try to load the grammar file
		return loadFromData(content, filename);
	}


	void Grammar::clear()
	{
		pRules.clear();
	}


	bool Grammar::loadFromData(const AnyString& content, const AnyString& source)
	{
		clear();

		RuleParser parser(*this, pRules);
		parser.source  = source;
		parser.content = content;
		bool ok = parser.analyze();
		parser.resetIndexes();
		return ok;
	}


	void Grammar::print(std::ostream& out) const
	{
		Node::Map::const_iterator end = pRules.end();
		for (Node::Map::const_iterator i = pRules.begin(); i != end; ++i)
		{
			out << i->first << ":\n";
			i->second.exportStd(out);
			out << "\n\n";
		}
	}





} // namespace PEG
} // namespace Parser
} // namespace Yuni



std::ostream& operator << (std::ostream& out, const Yuni::Parser::PEG::Grammar& grammar)
{
	grammar.print(out);
	return out;
}



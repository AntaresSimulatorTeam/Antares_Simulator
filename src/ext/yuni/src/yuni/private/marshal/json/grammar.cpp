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
#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/system/console.h>
#include "grammar.h"
#include "internal-charsets.hxx"
#include "internal-productions.hxx"
#include "internal-symbols.hxx"
#include "internal-lalr.hxx"
#include "internal-dfa.hxx"
#include "internal-table.hxx"
#include <cassert>
#include <iostream>
#include <stack>

using namespace Yuni;



namespace Grammar
{
namespace JSON
{


	static inline void EmptyNodeCallbackEnter(ASTNode& node, const ASTNode::List&, void*)
	{
		# ifndef NDEBUG
		std::cout << "   [!!] node enter: uncaught symbol " << node.symbolName << "  (enum: "
			<< (uint) node.symbol << ")\n";
		# endif
	}

	static inline void EmptyNodeCallbackLeave(ASTNode& node, const ASTNode::List&, void*)
	{
		# ifndef NDEBUG
		std::cout << "   [!!] node leave: uncaught symbol " << node.symbolName << "  (enum: "
			<< (uint) node.symbol << ")\n";
		# endif
	}




	namespace // anonymous
	{


	class Internal final
	{
	public:
		class Stack
		{
		public:
			Stack() :
				lalr((uint) -1),
				next(nullptr)
			{}

			inline void reset()
			{
				token.reset();
				lalr = (uint) -1;
			}

			//! Token
			Token token;
			//! LALR index
			uint lalr;
			//! Next item
			Stack* next;
		};

	public:
		Internal(EventError& error, const AnyString& content);
		~Internal();

		void refurb(Stack* item);

		Stack* allocateStackItem();

		void prepareDiagnostic(String& diagnostic, const Cursor& cursor, uint tokenlength) const;

		bool error(Error error, const Stack* current) const;

		void retrieveUntilEndOfLine(Token& token);

		void retrieveNextToken(Token& token);

		Private::JSON::LALR::ActionType  parseToken(Stack& current);

		bool operator () ();


	private:
		bool detectBOM();


	public:
		//! Position within the stream
		Cursor cursor;

		struct
		{
			//! Real offset within the buffer/file/other
			uint offset;
			//! Decal offset - introduced by BOM
			uint decalOffset;
		} data;

		struct
		{
			//! Current lalr state
			uint lalr;
			//! Stack queue
			Stack* queue;
			//! Cuurent queue item
			Stack* current;
			//! Reserve
			Stack* refurb;
			//! Comments
			Stack* comments;
		} state;
		struct
		{
			//! Comment level
			int level;
		} comments;

		struct
		{
			bool reductions;
			bool comments;
		} trim;

		//! Grammar
		const Private::JSON::GrammarInfos& grammar;

		bool colors;

		//! The text to parse
		AnyString content;

		EventError& notifyError;

	}; // class Internal




	inline bool Internal::detectBOM()
	{
		if (content.size() > 1)
		{
			// UTF-16 Big endian
			if ((unsigned char) content[0] == 0xFE and ((unsigned char) content[1]) == 0xFF)
			{
				if (content.size() > 3 and !content[2] and !content[3])
					notifyError(errBOM, nullptr, "UTF-32 Little Endian encoding detected. ASCII/UTF-8 required", cursor.line, cursor.column);
				else
					notifyError(errBOM, nullptr, "UTF-16 Big Endian encoding detected. ASCII/UTF-8 required", cursor.line, cursor.column);
				return false;
			}
			// UTF-16 Little endian
			if ((unsigned char) content[0] == 0xFF and ((unsigned char) content[1]) == 0xFE)
			{
				notifyError(errBOM, nullptr, "UTF-16 Little Endian encoding detected. ASCII/UTF-8 required", cursor.line, cursor.column);
				return false;
			}
			// UTF-8 Little endian
			if ((unsigned char) content[0] == 0xEF and ((unsigned char) content[1]) == 0xBB
				and content.size() > 2 and ((unsigned char) content[2]) == 0xBF)
			{
				// Slipping the byte-order mark
				content.consume(3); // eat 3 bytes
				data.decalOffset = 3;
				return true;
			}
			if (content.size() > 3)
			{
				if (!content[0] and !content[1] and ((unsigned char) content[2]) == 0xFE
					and ((unsigned char) content[3]) == 0xFF)
				{
					notifyError(errBOM, nullptr, "UTF-32 Big Endian encoding detected. ASCII/UTF-8 required", cursor.line, cursor.column);
					return false;
				}
			}
		}
		return true;
	}



	Internal::Internal(EventError& error, const AnyString& content) :
		grammar(Private::JSON::grammar),
		content(content),
		notifyError(error)
	{
		cursor.line       = 1;
		cursor.column     = 1;
		cursor.bytes      = 0; // if an error occurs on the first line
		data.offset       = 0;
		data.decalOffset  = 0;
		state.lalr        = grammar.initialLALRState;
		state.queue       = nullptr;
		state.comments    = nullptr;
		state.refurb      = nullptr;
		comments.level    = 0;
		trim.reductions   = true;
		trim.comments     = false;
	}


	inline Internal::~Internal()
	{
		while (state.queue)
		{
			Stack* next = state.queue->next;
			delete state.queue;
			state.queue = next;
		}
		while (state.refurb)
		{
			Stack* next = state.refurb->next;
			delete state.refurb;
			state.refurb = next;
		}
	}


	inline void Internal::refurb(Stack* item)
	{
		item->reset();
		item->next = state.refurb;
		state.refurb = item;
	}


	inline Internal::Stack* Internal::allocateStackItem()
	{
		// Trying to reuse a "deleted" stack item
		if (state.refurb)
		{
			Stack* reuse = state.refurb;
			state.refurb = reuse->next;
			reuse->next = nullptr;
			// reuse->reset(); - already prepared by refurb()
			return reuse;
		}
		return new Stack();
	}


	void Internal::prepareDiagnostic(String& diagnostic, const Cursor& cursor, uint tokenlength) const
	{
		CString<32,false> linestr;
		linestr =  "l.";
		linestr += cursor.line;
		diagnostic.clear();

		if (cursor.bytes >= content.size())
			return;

		assert(not content.empty() and "should never reach this code if content is empty");
		enum
		{
			marginLeft = 30,
			marginRight = 60,
		};

		if (colors)
			System::Console::TextColor<System::Console::black>::Set(diagnostic);

		diagnostic += "         ";
		diagnostic.overwriteRight(linestr);
		diagnostic += " | ";
		// looking for the previous linefeed
		uint previousLinefeed = content.rfind('\n', cursor.bytes);
		if (previousLinefeed < content.size())
			++previousLinefeed;
		else
			previousLinefeed = 0;

		// removing unwanted tabs at the begining
		while (previousLinefeed < content.size() and (content[previousLinefeed] == '\t' or content[previousLinefeed] == ' '))
			++previousLinefeed;

		if (previousLinefeed >= content.size()) // double check
			previousLinefeed = 0;

		if (previousLinefeed <= cursor.bytes) // is it possible otherwise ?
		{
			if (cursor.bytes - previousLinefeed > marginLeft)
				previousLinefeed = cursor.bytes - marginLeft;

			String::Size nextLinefeed = content.find('\n', cursor.bytes);
			if (!(nextLinefeed < content.size()))
				nextLinefeed = content.size();
			if (nextLinefeed - cursor.bytes > marginRight)
				nextLinefeed = cursor.bytes + marginRight;

			if (previousLinefeed - nextLinefeed > 0)
			{
				if (colors)
					System::Console::ResetTextColor(diagnostic);

				diagnostic.append(content, nextLinefeed - previousLinefeed, previousLinefeed);

				if (colors)
					System::Console::TextColor<System::Console::black>::Set(diagnostic);

				diagnostic += "\n          | ";

				for (uint i = previousLinefeed; i < cursor.bytes; ++i)
				{
					if (content[i] == '\t')
						diagnostic += '\t';
					else
						diagnostic += ' ';
				}

				if (colors)
					System::Console::TextColor<System::Console::purple>::Set(diagnostic);

				diagnostic += '^';
				for (uint i = 1; i < tokenlength; ++i)
					diagnostic += '~';

				// remove too much tabs at the begining (twice)
				diagnostic.replace(" | \t\t", " | \t");
				diagnostic.replace(" | \t\t", " | \t");

				{
					CString<32,false> replacement;
					if (colors)
						System::Console::TextColor<System::Console::black>::Set(replacement);
					replacement += ".    ";
					if (colors)
						System::Console::ResetTextColor(replacement);
					diagnostic.replace("\t", replacement);
				}

				if (colors)
					System::Console::ResetTextColor(diagnostic);
				return; // OK
			}
		}
		diagnostic.clear();
	}


	bool Internal::error(Error error, const Stack* current) const
	{
		String diagnostic;
		if (current)
		{
			prepareDiagnostic(diagnostic, current->token.cursor, current->token.text.size());
			return notifyError(error, current->token.text, diagnostic, current->token.cursor.line, current->token.cursor.column);
		}
		else
		{
			prepareDiagnostic(diagnostic, cursor, 0);
			return notifyError(error, nullptr, diagnostic, cursor.line, cursor.column);
		}
	}


	inline void Internal::retrieveUntilEndOfLine(Token& token)
	{
		if (not trim.comments)
		{
			// if we want to keep the comments, we should keep the first chars
			if (data.offset > 0)
			{
				--data.offset;
				if (content[data.offset] == '/' and data.offset > 0)
					--data.offset;
			}
		}

		uint linefeed = content.find('\n', data.offset);
		if (linefeed < content.size())
		{
			if (not trim.comments)
				token.text.adapt(content, linefeed - data.offset, data.offset);

			data.offset = (uint) linefeed + 1;
			++cursor.line;
			cursor.column = 1;
			cursor.bytes = data.offset;
		}
		else
			data.offset = content.size();
	}


	inline void Internal::retrieveNextToken(Token& token)
	{
		if (not (data.offset < content.size()))
		{
			token.symbol = symNTEOF; // end
			return;
		}

		uint dfa = grammar.initialDFAState;
		assert(dfa < grammar.dfa.count);
		uint length = 0;  // Number of processed characters from content
		uint acceptLength = (uint) -1; // Length of longest found symbol
		uint acceptIndex  = (uint) -1; // Longest found symbol so far

		while (data.offset + length < content.size())
		{
			// If this is a valid symbol-terminal then save it. We know the
			// input matches the symbol, but there may be a longer symbol that
			// matches so we have to keep scanning
			assert(dfa < grammar.dfa.count);
			if (grammar.dfa.table[dfa].acceptSymbol != (uint) -1)
			{
				acceptIndex  = dfa;
				acceptLength = length;
			}
			// Walk through the edges and scan the characterset of each edge for
			// the current character.
			bool foundSomething = false;

			assert(dfa < grammar.dfa.count);
			// current dfa entry
			const Private::JSON::DFA::Entry& dfaEntry = grammar.dfa.table[dfa];
			uint stateCount = dfaEntry.stateCount;

			// The current utf8 character
			AnyString::const_utf8iterator c = content.utf8begin(data.offset + length);
			//char c = content[data.offset + length];

			uint symbolLength = (*c).size();
			if (symbolLength == 1)
			{
				for (uint i = 0; i != stateCount; ++i)
				{
					uint charsetIndex = dfaEntry.states[i].charset;
					AnyString adapter(grammar.charsets.table[charsetIndex].ascii, grammar.charsets.table[charsetIndex].asciiSize);

					if (adapter.contains((char)(*c)))
					{
						foundSomething = true;

						// Jump to the TargetState, which points to another set of DFA edges
						// describing the next character.
						assert(i < dfaEntry.stateCount);
						dfa = dfaEntry.states[i].target;
						++length;
						break;
					}
				}
			}
			else
			{
				for (uint i = 0; i != stateCount; ++i)
				{
					uint charsetIndex = dfaEntry.states[i].charset;
					if (grammar.charsets.table[charsetIndex].utf8chars)
					{
						foundSomething = true;

						// Jump to the TargetState, which points to another set of DFA edges
						// describing the next character.
						assert(i < dfaEntry.stateCount);
						dfa = dfaEntry.states[i].target;
						length += symbolLength;
						break;
					}
				}
			}

			if (not foundSomething)
				break;
		}

		// If the DFA is a terminal then return the Symbol, and Length characters
		// from the input
		if (grammar.dfa.table[dfa].acceptSymbol != (uint) -1)
		{
			token.symbol = grammar.dfa.table[dfa].acceptSymbol;
			token.text.adapt(content, length, data.offset);
			data.offset += length;
		}
		else
		{
			// If we found a shorter terminal before, then return that Symbol, and
			// it's characters
			if (acceptIndex != (uint) -1)
			{
				assert(acceptIndex < grammar.dfa.count);
				token.symbol = grammar.dfa.table[acceptIndex].acceptSymbol;
				token.text.adapt(content, acceptLength, data.offset);
				data.offset += acceptLength;
			}
			else
			{
				// Return SYMBOLERROR and a string with 1 character from the input
				token.symbol = symNTError; // entry 1 in the symbol table - Error
				token.text = " "; // dummy char
				data.offset += 1;
				return; // useless to move the cursor
			}
		}

		// moving the cursor
		uint decal = 0;
		do
		{
			uint linefeed = token.text.find('\n', decal);
			if (linefeed < token.text.size())
			{
				++cursor.line;
				cursor.column = 1;
				decal = linefeed + 1;
				continue;
			}
			else
			{
				cursor.column += token.text.size() - decal;
				break;
			}
		}
		while (true);
		// absolute cursor
		cursor.bytes = data.offset;
	}


	Private::JSON::LALR::ActionType  Internal::parseToken(Stack& current)
	{
		// find the token symbol in the LALR table
		uint action = 0;
		{
			bool found = false;
			for (; action < grammar.lalr.table[state.lalr].actionCount; ++action)
			{
				if (grammar.lalr.table[state.lalr].actions[action].symbol == current.token.symbol)
				{
					found = true;
					break;
				}
			}
			if (!found)
				return Private::JSON::LALR::syntaxError;
		}

		// current entry within the LALR table
		const Private::JSON::LALR::Action& entry = grammar.lalr.table[state.lalr].actions[action];

		// ACCEPT
		switch (entry.action)
		{
			// SHIFT: switch the LALR state and return. We're ready to accept
			// the next token
			case Private::JSON::LALR::shift:
				{
					state.lalr = entry.nextState;
					return Private::JSON::LALR::shift;
				}

				// GOTO: switch the LALR state and return. We're ready to accept
				// the next token.
				// Note: In my implementation SHIFT and GOTO do the exact same thing. As far
				// as I can tell GOTO only happens just after a reduction. Perhaps GOLD makes
				// the difference to allow the program to perform special actions, which my
				// implementation does not need
			case Private::JSON::LALR::goTo:
				{
					state.lalr = entry.nextState;
					return Private::JSON::LALR::goTo;
				}

				// ACCEPT: exit. We're finished parsing the input
			case Private::JSON::LALR::accept:
				{
					return Private::JSON::LALR::accept;
				}

			default:
				break;
		}

		// REDUCE
		// Create a new Reduction according to the Rule that is specified by the action.
		// - Create a new Reduction in the ReductionArray.
		// - Pop tokens from the TokenStack and add them to the Reduction.
		// - Push a new token on the TokenStack for the Reduction.
		// - Iterate.
		uint rule = entry.nextState;

		if (trim.reductions
			and grammar.productions.table[rule].symbolCount == 1
			and grammar.symbols.table[grammar.productions.table[rule].symbols[0]].kind == Private::JSON::Symbols::kdNonTerminal)
		{
			// pop the rule from the queue
			Stack* pop = state.queue;
			state.queue = pop->next;

			// rewind the lalr state
			state.lalr = pop->lalr;
			// change the token into the rule
			pop->token.symbol = (Grammar::JSON::Symbol) grammar.productions.table[rule].head;

			// feed the token to the lalr state machine
			parseToken(*pop);

			// repush the modified token
			pop->next = state.queue;
			state.queue = pop;
			// save the new lalr state in the input token
			current.lalr = state.lalr;

			// feed the input token to the LALR state machine and exit
			return parseToken(current);
		}

		// reduction...
		assert(rule < grammar.productions.count);
		Stack* reduction = allocateStackItem();
		reduction->token.production = rule;
		reduction->token.symbol = (Grammar::JSON::Symbol) grammar.productions.table[rule].head;
		reduction->token.cursor = current.token.cursor;
		reduction->lalr = state.lalr;

		// Reduce tokens from the TokenStack by moving them to the Reduction.
		// The Lalr state will be rewound to the state it was for the first
		// symbol of the rule
		uint symbolCount = grammar.productions.table[rule].symbolCount;
		if (symbolCount)
		{
			// note: "tokens" must not be allocated if symbolCount == 0
			// (because of delete[]
			reduction->token.tokens.resize(symbolCount);

			for (uint i = symbolCount; i != 0; --i)
			{
				Stack* pop = state.queue;
				assert(pop and "invalid stack");
				state.queue = pop->next;
				pop->next = nullptr;

				// inserting the new token
				assert(i - 1 < reduction->token.tokens.size());

				reduction->token.tokens[i - 1] = pop->token;

				if (pop->token.special)
				{
					// this token is 'special', meaning that it is not really part of the grammar
					// (comment, error...)
					// this token was not expected, inserting a new one before
					// the current token
					Grammar::JSON::Token::Vector::iterator it = reduction->token.tokens.begin();
					reduction->token.tokens.insert(it + (i - 1), Token());
					// ....x.
					// ....ix.
					// the current token does not count, we must eat an additional token
					++i;
				}

				// updating state
				pop->token.tokens.clear(); // for not being deleted
				state.lalr = pop->lalr;
				reduction->lalr = state.lalr;
				reduction->token.cursor = pop->token.cursor;
				// the current item retrieved from the queue might be reused later
				refurb(pop);
			}
		}

		// call the lalr state machine with the symbol of the rule
		parseToken(*reduction);

		// push new token on the queue for the reduction
		reduction->next = state.queue;
		state.queue = reduction;

		// Save the current LALR state in the InputToken. We need this to be
		// able to rewind the state when reducing
		current.lalr = state.lalr;

		// Call the LALR state machine with the InputToken. The state has
		// changed because of the reduction, so we must accept the token
		// again
		return parseToken(current);
	}


	bool Internal::operator () ()
	{
		// Try to detect the necoding using the BOM (Byte-Order-Mark)
		if (not detectBOM())
			return false;

		do
		{
			Stack* current = allocateStackItem();
			current->lalr = state.lalr;
			current->token.cursor = cursor;

			retrieveNextToken(current->token);

			// If we are inside a comment then ignore everything except the end
			// of the comment, or the start of a nested comment
			if (comments.level > 0)
			{
				// Nested comment
				switch (grammar.symbols.table[current->token.symbol].kind)
				{
					case Private::JSON::Symbols::kdCommentStart:
						{
							current->next = state.queue;
							state.queue = current;
							++comments.level;
							break;
						}

					case Private::JSON::Symbols::kdCommentEnd:
						{
							if (trim.comments)
							{
								// deleting the current token which is a comment end
								refurb(current);
								// deleting the previous one, which is a kdCommentStart or equivalent
								current = state.queue;
								if (current)
								{
									state.queue = current->next;
									refurb(current);
								}
							}
							else
							{
								// comment text
								Cursor end = current->token.cursor;
								Cursor start = state.queue->token.cursor;
								// We have only two cases : #* or /*
								// In any cases, 2 chars are missing
								end.bytes += 2;

								if (end.bytes > start.bytes)
								{
									AnyString& text = state.queue->token.text;
									text.adapt(content, end.bytes - start.bytes, start.bytes);
								}

								// deleting the current token which is a comment end and
								// is completely useless
								refurb(current);

								// keeping the last one, but marking it as special
								(state.queue)->token.special = true;
							}

							--comments.level;
							break;
						}

					case Private::JSON::Symbols::kdEnd:
						//case Private::JSON::Symbols::kdError:
						{
							error(errLexical, current);
							delete current;
							return false;
						}

					default:
						break;
				} // switch

				continue;
			} // nested comments


			// symbol current token
			typedef Grammar::Private::JSON::Symbols::Kind  SymbolKind;
			SymbolKind kind = grammar.symbols.table[(uint) current->token.symbol].kind;

			if (kind == Private::JSON::Symbols::kdWhitespace)
			{
				// ignore whitespaces
				refurb(current);
				continue;
			}

			if (kind == Private::JSON::Symbols::kdTerminal or kind == Private::JSON::Symbols::kdEnd)
			{
				// Parse the current token
				Private::JSON::LALR::ActionType action = parseToken(*current);

				if ((uint) action >= Private::JSON::LALR::accept)
				{
					if (action == Private::JSON::LALR::accept)
					{
						if (grammar.symbols.table[(uint)current->token.symbol].kind == Private::JSON::Symbols::kdEnd)
						{
							// destroy the pseudo token, won't be reused
							delete current;
							return true; // OK, the parse is terminated !
						}
					}
					if (action == Private::JSON::LALR::syntaxError)
						error(errLexical, current);

					delete current;
					return false;
				}
			}
			else
			{
				// new comments
				if (kind == Private::JSON::Symbols::kdCommentStart)
				{
					current->next = state.queue;
					state.queue = current;
					++comments.level;
					continue;
				}
				if (kind == Private::JSON::Symbols::kdCommentLine)
				{
					retrieveUntilEndOfLine(current->token);
					if (not trim.comments)
					{
						current->token.special = true;
						current->next = state.queue;
						state.queue = current;
					}
					else
						refurb(current);
					continue;
				}

				if (kind == Private::JSON::Symbols::kdError)
				{
					error(errLexical, current);
					delete current;
					return false;
				}

				if (kind == Private::JSON::Symbols::kdNonTerminal)
				{
					error(errLexical, current);
					delete current;
					return false;
				}

				if (kind == Private::JSON::Symbols::kdCommentEnd)
				{
					assert(false and "what are we doing here ?");
					error(errLexical, current);
					delete current;
					return false;
				}
			}

			// queuing...
			current->next = state.queue;
			state.queue = current;
		}
		while (true);

		assert(false and "what are we doing here ?");
		return false;
	}




	} // anonymous namespace










	Parser::Parser() :
		colors(true)
	{
	}


	Parser::~Parser()
	{
	}



	bool Parser::invoke()
	{
		if (content.empty())
			return true;

		Internal internal(onError, content);
		internal.colors = colors;
		if (internal())
		{
			Token& original = internal.state.queue->token;
			root = original;
			original.tokens.clear(); // for not being deleted
			return true;
		}
		return false;
	}


	ASTWalker::ASTWalker()
	{
		for (uint i = 0; i != symMax; ++i)
			onNodeEnter[i] = & EmptyNodeCallbackEnter;
		for (uint i = 0; i != symMax; ++i)
			onNodeLeave[i] = & EmptyNodeCallbackLeave;
	}


	ASTWalker::~ASTWalker()
	{
	}


	void ASTNode::copyFromToken(const Token& token)
	{
		text   = token.text;
		cursor = token.cursor;
		unused = false;
		depth  = 0;
		symbol = token.symbol;
		symbolName = Private::JSON::grammar.symbols.table[symbol].name;

		terminal =
			Private::JSON::grammar.symbols.table[symbol].kind == Private::JSON::Symbols::kdTerminal;
		nonTerminal =
			Private::JSON::grammar.symbols.table[symbol].kind == Private::JSON::Symbols::kdNonTerminal;

		childIndex = 0;
		childrenCount = (uint) token.tokens.size();
	}



	void ASTWalker::walk(const Parser& parser, void* user)
	{
		typedef std::stack<const Token*>  ToVisit;
		ASTNode::List stack;
		ToVisit tokensToVisit;

		# ifndef NDEBUG
		uint statsEnter = 0; // used to make internal checks
		uint statsLeave = 0;
		# endif

		// root node
		{
			stack.push_back(ASTNode());
			ASTNode& rootnode = stack.back();
			rootnode.copyFromToken(parser.root);
			rootnode.depth = 0;
			tokensToVisit.push(& parser.root);

			// Entering into the new node
			# ifndef NDEBUG
			++statsEnter;
			# endif
			(*onNodeEnter[rootnode.symbol])(stack.back(), stack, user);
		}

		do
		{
			ASTNode& node  = stack.back();

			// checking if there is still some child to visit
			if (node.childIndex < node.childrenCount)
			{
				// This node has children, we have to walk through them
				const Token* currentToken = tokensToVisit.top();
				const Token& nextToken    = currentToken->tokens[node.childIndex];


				stack.push_back(ASTNode());
				ASTNode& astnode = stack.back();
				astnode.copyFromToken(nextToken);
				astnode.unused = (astnode.nonTerminal and 0 == astnode.childrenCount);

				astnode.depth = node.depth + 1;
				++node.childIndex;

				tokensToVisit.push(& nextToken);

				// Entering into the new node
				# ifndef NDEBUG
				++statsEnter;
				# endif
				(*onNodeEnter[astnode.symbol])(astnode, stack, user);
				continue;
			}

			// This node is a leaf, or we do not longer have business with it
			// Leaving the node
			# ifndef NDEBUG
			++statsLeave;
			# endif
			(*onNodeLeave[node.symbol])(node, stack, user);

			do
			{
				stack.pop_back();
				tokensToVisit.pop();

				if (stack.empty())
					break;
				ASTNode& astnode = stack.back();
				// dequeue if the next child for the future ast node exists, to not
				// call onNodeEnter twice
				if (astnode.childIndex < astnode.childrenCount)
					break;

				# ifndef NDEBUG
				++statsLeave;
				# endif
				(*onNodeLeave[astnode.symbol])(astnode, stack, user);
			}
			while (true);
		}
		while (not stack.empty());

		# ifndef NDEBUG
		assert(statsLeave == statsEnter);
		# endif
	}





} // namespace JSON
} // namespace Grammar


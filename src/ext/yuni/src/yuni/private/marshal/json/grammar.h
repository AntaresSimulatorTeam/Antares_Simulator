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
#ifndef __CPP_PARSER_JSON_GRAMMAR_H__
# define __CPP_PARSER_JSON_GRAMMAR_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <yuni/core/bind.h>
# include "symbols.h"


namespace Grammar
{
namespace JSON
{


	enum Error
	{
		//! No error
		errNone = 0,
		//! Input could not be tokenized
		errLexical,
		//! Input is an invalid token
		errToken,
		//! Input does not match any rule
		errSyntax,
		//! A  comment was started but not finished
		errComment,
		//! Invalid BOM
		errBOM,
		//! Not enough memory
		errMemory
	};

	class Parser;

	struct Cursor final
	{
		Cursor();
		//! Current line
		uint line;
		//! Current column within the current line
		uint column;
		//! Raw offset within the content
		uint bytes;
	};

	struct Token final
	{
	public:
		//! Vector
		typedef std::vector<Token> Vector;

	public:
		Token();
		~Token();

		//! reset internal state
		void reset();


	public:
		//! Production index (reduction rule)
		uint production;
		//! reduction tokens
		Vector tokens;
		//! Symbol index
		Symbol symbol;
		//! data from the input
		AnyString text;
		//! Position within the stream
		Cursor cursor;
		//! Flag to determine whether this token is a special token
		// Special means not that this token is not expected by the grammar
		// (comment, error...)
		bool special;

	}; // class Token


	//! Error callback
	typedef Yuni::Bind<bool (Error error, const AnyString& text, const AnyString& diagnostic, uint line, uint column)>  EventError;


	class IASTNodeData
	{
	public:
		IASTNodeData() {}
		virtual ~IASTNodeData() {}
	};

	class ASTNode final
	{
	public:
		//! List of nodes
		typedef std::list<ASTNode> List;

	public:
		ASTNode() : data(nullptr) {}
		void copyFromToken(const Token& token);


	public:
		AnyString text;
		//! Position within the stream
		Cursor cursor;
		//! Depth
		uint depth;
		//! Symbol
		Symbol symbol;
		//! Symbol name
		AnyString symbolName;

		//! Extras - User data
		IASTNodeData* data;

		//! Current index within the children - for the next ast node
		uint childIndex;
		//! Children count
		uint childrenCount;

		//! Flag to determine if the node is relevant for code analysis
		bool unused;
		//! Flag to determine whether the symbol is terminal or not
		bool terminal;
		//! Flag to determine whether the symbol is non-terminal or not
		bool nonTerminal;

	}; // class ASTNode



	class ASTWalker final
	{
	public:
		//! Node callback
		typedef void (* NodeCallback) (ASTNode& node, const ASTNode::List&, void* user);
		enum
		{
			symbolCount = symMax,
		};

	public:
		ASTWalker();
		~ASTWalker();

		void walk(const Parser& parser, void* user = nullptr);

	public:
		//! Events: entering into a new node
		NodeCallback  onNodeEnter[symMax];
		//! Events: leaving node
		NodeCallback  onNodeLeave[symMax];

	}; // class ASTWalker




	class Parser final
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Parser();
		//! Destructor
		~Parser();
		//@}


		/*!
		** \brief Start a new parsing
		**
		** Callbacks will be used to retrieve input data
		** \return A valid root node if the parsing was complete, nullptr if an error has occured
		*/
		bool invoke();


	public:
		//! The content to parse
		AnyString content;
		//! Event: an error has occured
		EventError onError;
		//! Root node
		Token root;
		//! use colors
		bool colors;

	}; // class Parser





} // namespace JSON
} // namespace Grammar

# include "grammar.hxx"

#endif // __CPP_PARSER_JSON_GRAMMAR_H__

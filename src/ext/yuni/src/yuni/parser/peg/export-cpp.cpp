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



namespace Yuni
{
namespace Parser
{
namespace PEG
{

	namespace // anonymous
	{

		#include "yuni/private/parser/peg/__parser.include.cpp.hxx"



		class CPPConverter final
		{
		public:
			CPPConverter(const AnyString& root, const Node::Map& rules);
			bool initialize(const AnyString& name);
			void startHeaderheaderGuardID();
			void endHeaderheaderGuardID();
			void generateH();
			void generateHXX();
			void generateCPP();
			bool writeFiles() const;

		public:
			//! Original rules
			const Node::Map& rules;

			//! Code in the .h file
			Clob h;
			//! Code in the .hxx file
			Clob hxx;
			//! Code in the .cpp file
			Clob cpp;

			//! Header guard ID
			YString headerGuardID;
			//! Class name
			YString classname;
			//! Namespace
			YString::Vector namespaces;

			//! Root filename
			YString rootfilename;
			//! Local include
			YString localInclude;

		}; // class CPPConverter



		template<class YStringT>
		static inline void EnumIDToClassname(YStringT& out, const YString& enumID)
		{
			out = "Node";
			out << enumID;
			if (not enumID.empty())
				out[4] = (char) YString::ToUpper(out[4]);
		}


		inline CPPConverter::CPPConverter(const AnyString& root, const Node::Map& rules) :
			rules(rules),
			rootfilename(root)
		{
			IO::ExtractFileName(localInclude, root);
		}


		inline bool CPPConverter::initialize(const AnyString& name)
		{
			name.split(namespaces, "/\\:");
			return true;
		}


		inline void CPPConverter::generateH()
		{
			const Node::Map::const_iterator end = rules.end();

			h << "#pragma once\n";
			h << "#include <yuni/yuni.h>\n";
			h << "#include <yuni/core/string.h>\n";
			h << "#include <yuni/core/bind.h>\n";
			h << "#include <yuni/core/dictionary.h>\n";
			h << "#include <yuni/core/smartptr/intrusive.h>\n";
			h << "#if (__cplusplus > 199711L || (defined(_MSC_VER) && _MSC_VER >= 1800))\n";
			h << "#	define " << headerGuardID << "_HAS_CXX_INITIALIZER_LIST  1\n";
			h << "#include <initializer_list>\n";
			h << "#endif";
			h << "\n\n";
			h << "//! Metadata support\n";
			h << "#define " << headerGuardID << "_HAS_METADATA  1\n";
			h << '\n';
			h << '\n';
			h << '\n';
			h << '\n';
			h << '\n';

			for (uint i = 0; i != namespaces.size(); ++i)
				h << "namespace " << namespaces[i] << "\n{\n";
			h << '\n';
			h << "	enum ASTRule\n";
			h << "	{\n";
			h << "		//! Unknown rule\n";
			h << "		rgUnknown = 0,\n";
			uint enumIndex = 1;
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i, ++enumIndex)
			{
				h << "		//! enum for the rule `" << i->first << '`';
				if (i->second.attributes.inlined)
					h << " [inline]";
				h << '\n';
				h << "		" << i->second.enumID << " = " << enumIndex << ",\n";
			}

			h << "		//! enum for the final rule\n";
			h << "		rgEOF = " << (enumIndex++) << '\n';

			h << "	};\n";

			// rule count
			h << "	enum\n";
			h << "	{\n";
			h << "		//! The total number of rules\n";
			h << "		ruleCount = " << enumIndex << '\n';
			h << "	};\n";
			h << "\n\n\n";

			h << "	//! Convert a rule id into its text representation\n";
			h << "	YUNI_DECL AnyString ASTRuleToString(enum ASTRule);\n";
			h << '\n';
			h << '\n';
			h << "	//! Get if a rule is an error\n";
			h << "	YUNI_DECL bool  ASTRuleIsError(enum ASTRule ruleid);\n";
			h << '\n';
			h << '\n';
			h << "	//! Get if the rule should be ignored when duplucating an AST (starting from 'tk-' and some special rules)\n";
			h << "	YUNI_DECL bool ShouldIgnoreASTRuleForDuplication(enum ASTRule);\n";
			h << '\n';
			h << '\n';
			h << '\n';
			h << "	enum Error\n";
			h << "	{\n";
			h << "		//! No error\n";
			h << "		errNone,\n";
			h << "		//! Parse error\n";
			h << "		errParse,\n";
			h << "	};\n";
			h << "\n\n\n\n\n";

			h << "	class YUNI_DECL Notification final\n";
			h << "	{\n";
			h << "	public:\n";
			h << "		//! Most suitable martptr\n";
			h << "		typedef Yuni::SmartPtr<Notification> Ptr;\n";
			h << "		//! Vector of nodes\n";
			h << "		typedef std::vector<Ptr> Vector;\n";
			h << '\n';
			h << "	public:\n";
			h << "		Notification()\n";
			h << "			: offset()\n";
			h << "			, line()\n";
			h << "		{}\n";
			h << '\n';
			h << "	public:\n";
			h << "		//! Start offset\n";
			h << "		uint offset;\n";
			h << "		//! Line Index\n";
			h << "		uint line;\n";
			h << "		//! Filename\n";
			h << "		YString message;\n";
			h << "		//! Filename\n";
			h << "		YString filename;\n";
			h << '\n';
			h << "	};\n";
			h << '\n';
			h << '\n';
			h << '\n';
			h << '\n';
			h << "	class YUNI_DECL Node final : public Yuni::IIntrusiveSmartPtr<Node, false, Yuni::Policy::SingleThreaded>\n";
			h << "	{\n";
			h << "	public:\n";
			h << "		//! Ancestor\n";
			h << "		typedef Yuni::IIntrusiveSmartPtr<Node, false, Yuni::Policy::SingleThreaded> inherited;\n";
			h << "		//! Threading policy\n";
			h << "		typedef inherited::ThreadingPolicy ThreadingPolicy;\n";
			h << "		//! The most suitable smart ptr for the class\n";
			h << "		typedef inherited::SmartPtrType<Node>::Ptr  Ptr;\n";
			h << "		//! Vector of nodes\n";
			h << "		typedef std::vector<Node::Ptr> Vector;\n";
			h << "		//! Set of symbols\n";
			h << "		typedef Yuni::Set<Ptr>::Unordered Set;\n";
			h << '\n';
			h << "		//! Callback definition for releasing metadata\n";
			h << "		typedef void (* MetadataReleaseCallback)(void*);\n";
			h << "		//! Callback definition for cloning metadata\n";
			h << "		typedef void* (* MetadataCloneCallback)(void*);\n";
			h << '\n';
			h << "		//! Callback definition for export a node\n";
			h << "		typedef void (* ExportCallback)(const Node& node, YString& tmp);\n";
			h << '\n';
			h << '\n';
			h << "	public:\n";
			h << "		//! Export the tree node\n";
			h << "		static void Export(Yuni::Clob& out, const Node& node);\n";
			h << "		//! Export the tree node (with color output)\n";
			h << "		static void Export(Yuni::Clob& out, const Node& node, bool color, ExportCallback callback = nullptr);\n";
			h << "		//! Export the tree node to HTML\n";
			h << "		static void ExportToHTML(Yuni::Clob& out, const Node& node);\n";
			h << '\n';
			h << "		//! Export the tree node into a JSON object\n";
			h << "		static void ExportToJSON(Yuni::Clob& out, const Node& node);\n";
			h << "		//! Export the tree node into a JSON object (with callback for additional data)\n";
			h << "		static void ExportToJSON(Yuni::Clob& out, const Node& node, void (*callback)(Yuni::Dictionary<AnyString, YString>::Unordered&, const Nany::Node&));\n";
			h << '\n';
			h << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			h << "		//! Callback for releasing metadata\n";
			h << "		static MetadataReleaseCallback  metadataRelease;\n";
			h << "		//! Callback for cloning metadata\n";
			h << "		static MetadataCloneCallback  metadataClone;\n";
			h << "		#endif\n";
			h << '\n';
			h << '\n';
			h << "	public:\n";
			h << "		//! Default constructor\n";
			h << "		Node();\n";
			h << "		//! Default constructor with a pre-defined rule\n";
			h << "		explicit Node(enum ASTRule);\n";
			h << "		//! Default constructor with a pre-defined rule and a given text\n";
			h << "		Node(enum ASTRule, const AnyString& text);\n";
			h << "		//! Copy constructor\n";
			h << "		Node(const Node& rhs);\n";
			h << "		//! Destructor\n";
			h << "		~Node();\n";
			h << '\n';
			h << "		void clear();\n";
			h << '\n';
			h << "		void swap(Node&);\n";
			h << '\n';
			h << "		//! Iterate through all child nodes\n";
			h << "		template<class F> bool each(const F& callback);\n";
			h << '\n';
			h << "		//! Iterate through all child nodes (const)\n";
			h << "		template<class F> bool each(const F& callback) const;\n";
			h << '\n';
			h << "		template<class F> bool each(enum ASTRule rule, const F& callback);\n";
			h << '\n';
			h << "		template<class F> bool each(enum ASTRule rule, const F& callback) const;\n";
			h << '\n';
			h << "		template<class StringT> bool extractFirstChildText(StringT& out, enum ASTRule rule) const;\n";
			h << '\n';
			h << "		template<class StringT> bool extractChildText(StringT& out, enum ASTRule rule, const AnyString& separator = nullptr) const;\n";
			h << '\n';
			h << "		uint findFirst(enum ASTRule rule) const;\n";
			h << '\n';
			h << "		bool exists(enum ASTRule rule) const;\n";
			h << '\n';
			h << "		#ifdef " << headerGuardID << "_HAS_CXX_INITIALIZER_LIST\n";
			h << "		Node::Ptr  xpath(std::initializer_list<enum ASTRule> path) const;\n";
			h << "		#endif\n";
			// h << "		Node::Ptr  xpath(enum ASTRule path) const;\n";
			h << '\n';
			h << "		Node& operator = (const Node& rhs);\n";
			h << '\n';
			h << "		void toText(YString& out) const;\n";
			h << '\n';
			h << '\n';
			h << "		const Node& firstChild() const;\n";
			h << "		Node& firstChild();\n";
			h << '\n';
			h << "		const Node& lastChild() const;\n";
			h << "		Node& lastChild();\n";
			h << '\n';
			h << '\n';
			h << '\n';
			h << "	public:\n";
			h << "		//! The rule ID\n";
			h << "		enum ASTRule rule;\n";
			h << "		//! Start offset\n";
			h << "		uint offset;\n";
			h << "		//! End offset\n";
			h << "		uint offsetEnd;\n";
			h << "		//! Text associated to the node (if any)\n";
			h << "		AnyString text;\n";
			h << '\n';
			h << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			h << "		//! Metadata\n";
			h << "		void* metadata;\n";
			h << "		#endif\n";
			h << '\n';
			h << "		//! Children\n";
			h << "		Node::Vector children;\n";
			h << "	};\n";
			h << '\n';
			h << '\n';
			h << '\n';
			h << '\n';
			h << '\n';
			h << "	class YUNI_DECL Parser final\n";
			h << "	{\n";
			h << "	public:\n";
			h << "		typedef Yuni::Bind<bool (Yuni::Clob& out, const AnyString& uri)>   OnURILoading;\n";
			h << "		typedef Yuni::Bind<bool (const AnyString& filename, uint line, uint offset, Error, const YString::Vector&)>  OnError;\n";
			h << '\n';
			h << "	public:\n";
			h << "		Parser();\n";
			h << "		~Parser();\n";
			h << '\n';
			h << "		void clear();\n";
			h << "		bool loadFromFile(const AnyString& filename);\n";
			h << "		bool load(const AnyString& content);\n";
			h << "		void translateOffset(uint& column, uint& line, const Node&) const;\n";
			h << "		void translateOffset(uint& column, uint& line, uint offset) const;\n";
			h << "		uint translateOffsetToLine(const Node& node) const;\n";
			h << '\n';
			h << '\n';
			h << "	public:\n";
			h << "		//! Event: load another include file\n";
			h << "		OnURILoading onURILoading;\n";
			h << "		//! Event: load another include file\n";
			h << "		OnError  onError;\n";
			h << '\n';
			h << "		//! The root node, if any\n";
			h << "		Node::Ptr root;\n";
			h << '\n';
			h << "		//! Notifications\n";
			h << "		Notification::Vector notifications;\n";
			h << '\n';
			h << '\n';
			h << "	private:\n";
			h << "		void* pData;\n";
			h << '\n';
			h << "	}; // class Parser\n";
		}


		inline void CPPConverter::generateHXX()
		{
			hxx << "#pragma once\n";
			hxx << '\n';
			hxx << "\n\n\n";

			hxx << "inline std::ostream& operator << (std::ostream& out, const ";
			for (uint i = 0; i != namespaces.size(); ++i)
				hxx << "::" << namespaces[i];
			hxx << "::Node& node)\n";
			hxx << "{\n";
			hxx << "	::Yuni::Clob content;\n";
			hxx << "	";
			for (uint i = 0; i != namespaces.size(); ++i)
				hxx << "::" << namespaces[i];
			hxx << "::Node::Export(content, node);";
			hxx << "out << content;\n";
			hxx << "	return out;\n";
			hxx << "}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << '\n';


			hxx << "inline std::ostream& operator << (std::ostream& out, const ";
			for (uint i = 0; i != namespaces.size(); ++i)
				hxx << "::" << namespaces[i];
			hxx << "::Node::Ptr node)\n";
			hxx << "{\n";
			hxx << "	if (!(!node))\n";
			hxx << "		out << *node;\n";
			hxx << "	else\n";
			hxx << "		out << \"<invalid ast node>\";\n";
			hxx << "	return out;\n";
			hxx << "}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << '\n';

			for (uint i = 0; i != namespaces.size(); ++i)
				hxx << "namespace " << namespaces[i] << "\n{\n";
			hxx << "\n\n";

			hxx << "	inline Node::Node()\n";
			hxx << "		: rule(rgUnknown)\n";
			hxx << "		, offset()\n";
			hxx << "		, offsetEnd()\n";
			hxx << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			hxx << "		, metadata()\n";
			hxx << "		#endif\n";
			hxx << "	{}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline Node::Node(enum ASTRule rule)\n";
			hxx << "		: rule(rule)\n";
			hxx << "		, offset()\n";
			hxx << "		, offsetEnd()\n";
			hxx << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			hxx << "		, metadata()\n";
			hxx << "		#endif\n";
			hxx << "	{}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline Node::Node(enum ASTRule rule, const AnyString& text)\n";
			hxx << "		: rule(rule)\n";
			hxx << "		, offset()\n";
			hxx << "		, offsetEnd()\n";
			hxx << "		, text(text)\n";
			hxx << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			hxx << "		, metadata()\n";
			hxx << "		#endif\n";
			hxx << "	{}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline Node::~Node()\n";
			hxx << "	{\n";
			hxx << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			hxx << "		if (metadata)\n";
			hxx << "		{\n";
			hxx << "			assert(metadataRelease != nullptr and \"invalid callback for releasing metadata\");\n";
			hxx << "			metadataRelease(metadata);\n";
			hxx << "		}\n";
			hxx << "		#endif\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	template<class F> inline bool Node::each(const F& callback)\n";
			hxx << "	{\n";
			hxx << "		for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "		{\n";
			hxx << "			if (not callback(*children[i]))\n";
			hxx << "				return false;\n";
			hxx << "		}\n";
			hxx << "		return true;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	template<class F> inline bool Node::each(const F& callback) const\n";
			hxx << "	{\n";
			hxx << "		for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "		{\n";
			hxx << "			if (not callback(*children[i]))\n";
			hxx << "				return false;\n";
			hxx << "		}\n";
			hxx << "		return true;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	template<class F> inline bool Node::each(enum ASTRule rule, const F& callback)\n";
			hxx << "	{\n";
			hxx << "		for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "		{\n";
			hxx << "			Node& subnode = *children[i];\n";
			hxx << "			if (subnode.rule == rule and not callback(subnode))\n";
			hxx << "				return false;\n";
			hxx << "		}\n";
			hxx << "		return true;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	template<class F> inline bool Node::each(enum ASTRule rule, const F& callback) const\n";
			hxx << "	{\n";
			hxx << "		for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "		{\n";
			hxx << "			const Node& subnode = *children[i];\n";
			hxx << "			if (subnode.rule == rule and not callback(subnode))\n";
			hxx << "				return false;\n";
			hxx << "		}\n";
			hxx << "		return true;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	template<class StringT> inline bool Node::extractFirstChildText(StringT& out, enum ASTRule rule) const\n";
			hxx << "	{\n";
			hxx << "		for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "		{\n";
			hxx << "			const Node& subnode = *children[i];\n";
			hxx << "			if (subnode.rule == rule)\n";
			hxx << "			{\n";
			hxx << "				out += subnode.text;\n";
			hxx << "				return true;\n";
			hxx << "			}\n";
			hxx << "		}\n";
			hxx << "		return false;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	template<class StringT> inline bool Node::extractChildText(StringT& out, enum ASTRule rule, const AnyString& separator) const\n";
			hxx << "	{\n";
			hxx << "		bool somethingFound = false;\n";
			hxx << '\n';
			hxx << "		if (separator.empty())\n";
			hxx << "		{\n";
			hxx << "			for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "			{\n";
			hxx << "				const Node& subnode = *children[i];\n";
			hxx << "				if (subnode.rule == rule)\n";
			hxx << "				{\n";
			hxx << "					out += subnode.text;\n";
			hxx << "					somethingFound = true;\n";
			hxx << "				}\n";
			hxx << "			}\n";
			hxx << "		}\n";
			hxx << "		else\n";
			hxx << "		{\n";
			hxx << "			for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "			{\n";
			hxx << "				const Node& subnode = *children[i];\n";
			hxx << "				if (subnode.rule == rule)\n";
			hxx << "				{\n";
			hxx << "					if (not out.empty())\n";
			hxx << "						out += separator;\n";
			hxx << "					out += subnode.text;\n";
			hxx << "					somethingFound = true;\n";
			hxx << "				}\n";
			hxx << "			}\n";
			hxx << "		}\n";
			hxx << "		return somethingFound;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline uint Node::findFirst(enum ASTRule rule) const\n";
			hxx << "	{\n";
			hxx << "		for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "		{\n";
			hxx << "			if (children[i]->rule == rule)\n";
			hxx << "				return i;\n";
			hxx << "		}\n";
			hxx << "		return (uint)-1;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline bool Node::exists(enum ASTRule rule) const\n";
			hxx << "	{\n";
			hxx << "		for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "		{\n";
			hxx << "			if (children[i]->rule == rule)\n";
			hxx << "				return true;\n";
			hxx << "		}\n";
			hxx << "		return false;\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline const Node& Node::firstChild() const\n";
			hxx << "	{\n";
			hxx << "		assert(not children.empty());\n";
			hxx << "		return *(children[0]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline Node& Node::firstChild()\n";
			hxx << "	{\n";
			hxx << "		assert(not children.empty());\n";
			hxx << "		return *(children[0]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline const Node& Node::lastChild() const\n";
			hxx << "	{\n";
			hxx << "		assert(not children.empty());\n";
			hxx << "		return *(children[children.size() - 1]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	inline Node& Node::lastChild()\n";
			hxx << "	{\n";
			hxx << "		assert(not children.empty());\n";
			hxx << "		return *(children[children.size() - 1]);\n";
			hxx << "	}\n";
			hxx << '\n';
			hxx << '\n';
			hxx << "	#ifdef " << headerGuardID << "_HAS_CXX_INITIALIZER_LIST\n";
			hxx << "	inline Node::Ptr  Node::xpath(std::initializer_list<enum ASTRule> path) const\n";
			hxx << "	{\n";
			hxx << "		if (path.size() > 0)\n";
			hxx << "		{\n";
			hxx << "			auto it = path.begin();\n";
			hxx << "			for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			hxx << "			{\n";
			hxx << "				if (children[i]->rule == *it)\n";
			hxx << "				{\n";
			hxx << "					Node::Ptr result = children[i];\n";
			hxx << "					++it;\n";
			hxx << "					for (; it != path.end(); ++it)\n";
			hxx << "					{\n";
			hxx << "						bool found = false;\n";
			hxx << "						auto& subnode = *result;\n";
			hxx << "						for (unsigned int j = 0; j != (unsigned) subnode.children.size(); ++j)\n";
			hxx << "						{\n";
			hxx << "							if (subnode.children[j]->rule == *it)\n";
			hxx << "							{\n";
			hxx << "								result = subnode.children[j];\n";
			hxx << "								found = true;\n";
			hxx << "								break;\n";
			hxx << "							}\n";
			hxx << "						}\n";
			hxx << "						if (not found)\n";
			hxx << "							return nullptr;\n";
			hxx << "					}\n";
			hxx << "					return result;\n";
			hxx << "				}\n";
			hxx << "			}\n";
			hxx << "		}\n";
			hxx << "		return nullptr;\n";
			hxx << "	}\n";
			hxx << "	#endif\n";
			hxx << '\n';
			hxx << '\n';
			// hxx << "	inline Node::Ptr  Node::xpath(enum ASTRule path) const\n";
			// hxx << "	{\n";
			// hxx << "		for (unsigned int i = 0; i != (unsigned) children.size(); ++i)\n";
			// hxx << "		{\n";
			// hxx << "			if (children[i]->rule == path)\n";
			// hxx << "				return children[i];\n";
			// hxx << "		}\n";
			// hxx << "		return nullptr;\n";
			// hxx << "	}\n";
			// hxx << '\n';
			// hxx << '\n';
			hxx << '\n';
			hxx << '\n';
		}


		static inline void GenerateFunctionForEachRule(Clob& cpp, uint& sp, const Node::Map& rules, const AnyString& name, const Node& node)
		{
			Clob body;

			// subnodes
			{
				// We have to reserve from the begining this vector, to avoir future calls to
				// std::vector::resize, and to invalidate references to internal elements
				Clob::Vector helpers;
				String::Vector datatext;
				helpers.reserve(node.treeCount());

				// skip the first AND block, which is useless actually
				if (node.rule.type == Node::asAND)
				{
					for (uint i = 0; i != (uint) node.children.size(); ++i)
						node.children[i].exportCPP(body, rules, helpers, datatext, 2, true, sp);
				}
				else
					node.exportCPP(body, rules, helpers, datatext, 2, true, sp);

				// helpers
				bool foundHelper = false;

				if (not datatext.empty())
				{
					for (uint i = 0; i != (uint) datatext.size(); ++i)
						cpp << '	' << datatext[i] << '\n';
					cpp << "\n\n\n";
				}

				for (uint i = 0; i != (uint) helpers.size(); ++i)
				{
					if (not helpers[i].empty())
					{
						if (not foundHelper)
						{
							foundHelper = true;
							cpp << "\n\n";
						}
						// extract forward declaration
						uint linefeed = helpers[i].find('\n');
						if (linefeed < helpers[i].size())
							cpp << AnyString(helpers[i], 0, linefeed) << ";\n";
					}
				}

				foundHelper = false;
				for (uint i = 0; i != (uint) helpers.size(); ++i)
				{
					if (not helpers[i].empty())
					{
						if (not foundHelper)
						{
							foundHelper = true;
							cpp << "\n\n";
						}
						cpp << helpers[i] << '\n';
					}
				}
			}

			cpp << "	//! ASTRule " << name << '\n';
			cpp << "	";
			cpp << ((node.enumID != "rgStart") ? "static inline " : "static ");

			cpp << "bool yy" << node.enumID << "(Datasource& ctx)\n";
			cpp << "	{\n";
			cpp << "		(void) ctx;\n";
			cpp << "		TRACE(\"entering " << node.enumID;
			if (node.attributes.inlined)
				cpp << " [inline]";
			cpp << "\");\n";

			if (not node.attributes.inlined)
				cpp << "		uint _ruleOffset = ctx.enterRule(" << node.enumID << ");\n";
			cpp << '\n';
			cpp << body;
			cpp << '\n';
			if (not node.attributes.inlined)
				cpp << "		ctx.commit(_ruleOffset, " << node.enumID << ");\n";
			cpp << "		return true;\n";
			cpp << "	}\n";
			cpp << "\n\n";
		}


		inline void CPPConverter::generateCPP()
		{
			const Node::Map::const_iterator end = rules.end();
			cpp << '\n';
			cpp << "#include \"" << localInclude << "h\"\n";
			cpp << "#include <cassert>\n";
			cpp << "#include <yuni/core/string.h>\n";
			cpp << "#include <yuni/io/file.h>\n";
			cpp << "#include <yuni/io/directory.h>\n";
			cpp << "#include <yuni/core/dictionary.h>\n";
			cpp << "#include <yuni/core/noncopyable.h>\n";
			cpp << "#include <yuni/datetime/timestamp.h>\n";
			cpp << "#include <yuni/core/system/console/console.h>\n";
			cpp << "#include <iostream>\n";
			cpp << '\n';
			cpp << "using namespace Yuni;\n";
			cpp << "\n\n";

			for (uint i = 0; i != namespaces.size(); ++i)
				cpp << "namespace " << namespaces[i] << "\n{\n";

			cpp << "namespace // anonymous\n{\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	static const bool _attrAttributeCapture[] =\n";
			cpp << "	{\n";
			cpp << "		false, // rgUnknown\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
				cpp << "		" << (i->second.attributes.capture ? "true" : "false") << ", // " << i->second.enumID << '\n';
			cpp << "		false, // rgEOF\n";
			cpp << "	};\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	static bool  RuleAttributeCapture(enum ASTRule ruleid)\n";
			cpp << "	{\n";
			cpp << "		assert((uint) ruleid < (uint) ruleCount);\n";
			cpp << "		return _attrAttributeCapture[(uint) ruleid];\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	static const bool _attrAttributeError[] =\n";
			cpp << "	{\n";
			cpp << "		false, // rgUnknown\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
			{
				bool error = i->first == "error" or i->first.startsWith("error-");
				cpp << "		" << (error ? "true" : "false") << ", // " << i->second.enumID << '\n';
			}
			cpp << "		false, // rgEOF\n";
			cpp << "	};\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	static inline bool  RuleAttributeError(enum ASTRule ruleid)\n";
			cpp << "	{\n";
			cpp << "		assert((uint) ruleid < (uint) ruleCount);\n";
			cpp << "		return _attrAttributeError[(uint) ruleid];\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	static const bool _attrAttributeImportant[] = {\n";
			cpp << "		false, // rgUnknown\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
				cpp << "		" << (i->second.attributes.important ? "true" : "false") << ", // " << i->second.enumID << '\n';
			cpp << "		false, // rgEOF\n";
			cpp << "	};\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	static inline bool  RuleAttributeImportant(enum ASTRule ruleid)\n";
			cpp << "	{\n";
			cpp << "		assert((uint) ruleid < (uint) ruleCount);\n";
			cpp << "		return _attrAttributeImportant[(uint) ruleid];\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	static const char* const _attrAttributeSimpleTextCapture[] =\n";
			cpp << "	{\n";
			cpp << "		nullptr, // rgUnknown\n";
			String textCapture;
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
			{
				cpp << "		";
				if (i->second.children.size() == 1 and i->second.children[0].isSimpleTextCapture())
				{
					textCapture = i->second.children[0].rule.text;
					textCapture.replace("\\", "\\\\");
					textCapture.replace("\"", "\\\"");
					cpp << '"' << textCapture << '"';
				}
				else
					cpp << "nullptr";

				cpp << ", // " << i->second.enumID << '\n';
			}
			cpp << "		nullptr, // rgEOF\n";
			cpp << "	};\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	static AnyString  RuleAttributeSimpleTextCapture(enum ASTRule ruleid)\n";
			cpp << "	{\n";
			cpp << "		assert((uint) ruleid < (uint) ruleCount);\n";
			cpp << "		return _attrAttributeSimpleTextCapture[(uint) ruleid];\n";
			cpp << "	}\n\n\n\n";

			PrepareCPPInclude(cpp);

			cpp << "	// Forward declarations\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
				cpp << "	static inline bool yy" << i->second.enumID << "(Datasource& ctx);\n";

			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';

			// generate all rules
			{
				uint sp = 0;
				for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
					GenerateFunctionForEachRule(cpp, sp, rules, i->first, i->second);
			}

			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "} // anonymous namespace\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	bool ShouldIgnoreASTRuleForDuplication(enum ASTRule rule)\n";
			cpp << "	{\n";
			cpp << "		static const bool hints[] = {\n";
			cpp << "			false, // rgUnknown\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
			{
				if (i->first.startsWith("tk-"))
					cpp << "			true, // " << i->first << '\n';
				else
					cpp << "			false, // " << i->first << '\n';
			}
			cpp << "		};";
			cpp << "		return hints[(uint) rule];\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	bool  ASTRuleIsError(enum ASTRule ruleid)\n";
			cpp << "	{\n";
			cpp << "		return RuleAttributeError(ruleid);\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	AnyString ASTRuleToString(enum ASTRule ruleid)\n";
			cpp << "	{\n";
			cpp << "		switch (ruleid)\n";
			cpp << "		{\n";
			cpp << "			case rgUnknown:\n";
			cpp << "				return \"<unknown>\";\n";
			for (Node::Map::const_iterator i = rules.begin(); i != end; ++i)
			{
				cpp << "			case " << i->second.enumID << ":\n";
				cpp << "				return \"" << i->first << "\";\n";
			}
			cpp << "			case rgEOF:\n";
			cpp << "				return \"EOF\";\n";
			cpp << "		}\n";
			cpp << "		return \"<error>\";\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	Parser::Parser()\n";
			cpp << "		: pData()\n";
			cpp << "	{\n";
			cpp << "		onURILoading.bind(& StandardURILoaderHandler);\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	Parser::~Parser()\n";
			cpp << "	{\n";
			cpp << "		delete (Datasource*) pData;\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Parser::clear()\n";
			cpp << "	{\n";
			cpp << "		root = nullptr;\n";
			cpp << "		delete (Datasource*) pData;\n";
			cpp << "		pData = nullptr;\n";
			cpp << "		if (not notifications.empty())\n";
			cpp << "			Notification::Vector().swap(notifications);\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	bool Parser::loadFromFile(const AnyString& filename)\n";
			cpp << "	{\n";
			cpp << "		if (!pData)\n";
			cpp << "			pData = new Datasource(notifications);\n";
			cpp << '\n';
			cpp << "		Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "		ctx.open(filename);\n";
			cpp << "		DATASOURCE_PARSE(ctx);\n";
			cpp << "		return ctx.success;\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	bool Parser::load(const AnyString& content)\n";
			cpp << "	{\n";
			cpp << "		if (!pData)\n";
			cpp << "			pData = new Datasource(notifications);\n";
			cpp << '\n';
			cpp << "		Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "		ctx.openContent(content);\n";
			cpp << "		DATASOURCE_PARSE(ctx);\n";
			cpp << "		return ctx.success;\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Parser::translateOffset(uint& column, uint& line, const Node& node) const\n";
			cpp << "	{\n";
			cpp << "		assert(&node and \"invalid pointer to node\");\n";
			cpp << "		column = 0;\n";
			cpp << "		line = 0;\n";
			cpp << "		if (YUNI_LIKELY(pData))\n";
			cpp << "		{\n";
			cpp << "			Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "			ctx.translateOffset(column, line, node.offset);\n";
			cpp << "		}\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Parser::translateOffset(uint& column, uint& line, uint offset) const\n";
			cpp << "	{\n";
			cpp << "		column = 0;\n";
			cpp << "		line = 0;\n";
			cpp << "		if (YUNI_LIKELY(pData))\n";
			cpp << "		{\n";
			cpp << "			Datasource& ctx = *((Datasource*) pData);\n";
			cpp << "			ctx.translateOffset(column, line, offset);\n";
			cpp << "		}\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	uint Parser::translateOffsetToLine(const Node& node) const\n";
			cpp << "	{\n";
			cpp << "		assert(&node and \"invalid pointer to node\");\n";
			cpp << "		uint column;\n";
			cpp << "		uint line;\n";
			cpp << "		translateOffset(column, line, node);\n";
			cpp << "		return line;\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	#if " << headerGuardID << "_HAS_METADATA != 0\n";
			cpp << "	/* static */ Node::MetadataReleaseCallback  Node::metadataRelease = nullptr;\n";
			cpp << "	/* static */ Node::MetadataCloneCallback    Node::metadataClone   = nullptr;\n";
			cpp << "	#endif\n";
			cpp << '\n';
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Node::ExportToHTML(Clob& out, const Node& node)\n";
			cpp << "	{\n";
			cpp << "		assert(&node and \"invalid reference to node\");\n";
			cpp << '\n';
			cpp << "		String tmp;\n";
			cpp << "		String indent;\n";
			cpp << "		InternalNodeExportHTML(out, node, indent, tmp);\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Node::Export(Clob& out, const Node& node, bool color, ExportCallback callback)\n";
			cpp << "	{\n";
			cpp << "		assert(&node and \"invalid reference to node\");\n";
			cpp << '\n';
			cpp << "		String tmp;\n";
			cpp << "		String indent;\n";
			cpp << "		if (not color)\n";
			cpp << "			InternalNodeExportConsole<false>(out, node, false, indent, tmp, callback);\n";
			cpp << "		else\n";
			cpp << "			InternalNodeExportConsole<true>(out, node, false, indent, tmp, callback);\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Node::ExportToJSON(Yuni::Clob& out, const Node& node, void (*callback)(Yuni::Dictionary<AnyString, YString>::Unordered&, const Nany::Node&))\n";
			cpp << "	{\n";
			cpp << "		String tmp;\n";
			cpp << "		String indent;\n";
			cpp << "		out << \"{ \\\"data\\\": [\\n\";\n";
			cpp << "		InternalNodeExportJSON(out, node, false, indent, tmp, callback);\n";
			cpp << "		out << \"\t{}\\n\";\n";
			cpp << "		out << \"] }\\n\";\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Node::ExportToJSON(Yuni::Clob& out, const Node& node)\n";
			cpp << "	{\n";
			cpp << "		ExportToJSON(out, node, nullptr);\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Node::Export(Clob& out, const Node& node)\n";
			cpp << "	{\n";
			cpp << "		Export(out, node, ::Yuni::System::Console::IsStdoutTTY());\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	Node::Node(const Node& rhs)\n";
			cpp << "		: inherited()\n";
			cpp << "		, rule(rhs.rule)\n";
			cpp << "		, offset(rhs.offset)\n";
			cpp << "		, offsetEnd(rhs.offsetEnd)\n";
			cpp << "		, text(rhs.text)\n";
			cpp << "	{\n";
			cpp << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			cpp << "		// cloning metadata\n";
			cpp << "		if (rhs.metadata)\n";
			cpp << "		{\n";
			cpp << "			assert(metadataClone != nullptr and \"invalid callback for cloning metadata\");\n";
			cpp << "			metadata = metadataClone(rhs.metadata);\n";
			cpp << "		}\n";
			cpp << "		else\n";
			cpp << "			metadata = nullptr;\n";
			cpp << "		#endif\n";
			cpp << '\n';
			cpp << "		// cloning children\n";
			cpp << "		if (not rhs.children.empty())\n";
			cpp << "		{\n";
			cpp << "			children.resize(rhs.children.size());\n";
			cpp << "			for (unsigned int i = 0; i != (unsigned) rhs.children.size(); ++i)\n";
			cpp << "				children[i] = new Node(*rhs.children[i]);\n";
			cpp << "		}\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	Node& Node::operator = (const Node& rhs)\n";
			cpp << "	{\n";
			cpp << "		rule = rhs.rule;\n";
			cpp << "		offset = rhs.offset;\n";
			cpp << "		offsetEnd = rhs.offsetEnd;\n";
			cpp << "		text = rhs.text;\n";
			cpp << "		children = rhs.children;\n";
			cpp << '\n';
			cpp << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			cpp << "		// copying metadata\n";
			cpp << "		if (metadata)\n";
			cpp << "		{\n";
			cpp << "			assert(metadataRelease != nullptr and \"invalid callback for releasing metadata\");\n";
			cpp << "			metadataRelease(metadata);\n";
			cpp << "		}\n";
			cpp << "		// cloning metadata\n";
			cpp << "		if (rhs.metadata)\n";
			cpp << "		{\n";
			cpp << "			assert(metadataClone != nullptr and \"invalid callback for cloning metadata\");\n";
			cpp << "			metadata = metadataClone(rhs.metadata);\n";
			cpp << "		}\n";
			cpp << "		else\n";
			cpp << "			metadata = nullptr;\n";
			cpp << "		#endif\n";
			cpp << '\n';
			cpp << "		return *this;\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Node::toText(YString& out) const\n";
			cpp << "	{\n";
			cpp << "		if (not text.empty())\n";
			cpp << "		{\n";
			cpp << "			if (not out.empty())\n";
			cpp << "				out += ' ';\n";
			cpp << "			out += text;\n";
			cpp << "			out.trimRight();\n";
			cpp << "		}\n";
			cpp << "		for (uint i = 0; i != (uint) children.size(); ++i)\n";
			cpp << "			children[i]->toText(out);\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Node::clear()\n";
			cpp << "	{\n";
			cpp << "		children.clear();\n";
			cpp << "		text.clear();\n";
			cpp << "		offset = 0;\n";
			cpp << "		offsetEnd = 0;\n";
			cpp << "		rule = rgUnknown;\n";
			cpp << '\n';
			cpp << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			cpp << "		if (metadata)\n";
			cpp << "		{\n";
			cpp << "			assert(metadataRelease != nullptr and \"invalid callback for releasing metadata\");\n";
			cpp << "			metadataRelease(metadata);\n";
			cpp << "			metadata = nullptr;\n";
			cpp << "		}\n";
			cpp << "		#endif\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
			cpp << "	void Node::swap(Node& other)\n";
			cpp << "	{\n";
			cpp << "		std::swap(rule, other.rule);\n";
			cpp << "		std::swap(offset, other.offset);\n";
			cpp << "		std::swap(offsetEnd, other.offsetEnd);\n";
			cpp << "		std::swap(text, other.text);\n";
			cpp << "		std::swap(children, other.children);\n";
			cpp << "		#if " << headerGuardID << "_HAS_METADATA != 0\n";
			cpp << "		std::swap(metadata, other.metadata);\n";
			cpp << "		#endif\n";
			cpp << "	}\n";
			cpp << '\n';
			cpp << '\n';
		}


		inline void CPPConverter::startHeaderheaderGuardID()
		{
			headerGuardID = "__HEADER";

			for (uint i = 0; i != namespaces.size(); ++i)
				headerGuardID << '_' << namespaces[i];
			headerGuardID << "_GRAMMAR";
			headerGuardID.toUpper();
		}


		inline void CPPConverter::endHeaderheaderGuardID()
		{
			h << "\n\n\n\n\n";
			hxx << "\n\n\n\n\n";
			cpp << "\n\n\n\n\n";

			if (not namespaces.empty())
			{
				uint i = (uint) namespaces.size();
				do
				{
					--i;
					h << "} // namespace " << namespaces[i] << '\n';
					hxx << "} // namespace " << namespaces[i] << '\n';
					cpp << "} // namespace " << namespaces[i] << '\n';
				}
				while (i > 0);
			}

			h << '\n' << "#include \"" << localInclude << "hxx\"\n";
		}


		inline bool CPPConverter::writeFiles() const
		{
			AnyString root = rootfilename;
			YString file;
			file.clear() << root << "h";
			if (not IO::File::SetContent(file, h))
				return false;

			file.clear() << root << "hxx";
			if (not IO::File::SetContent(file, hxx))
				return false;

			file.clear() << root << "cpp";
			if (not IO::File::SetContent(file, cpp))
				return false;

			return true;
		}


	} // anonymous namespace






	bool Grammar::exportToCPP(const AnyString& rootfilename, const AnyString& name) const
	{
		CPPConverter data(rootfilename, pRules);
		if (YUNI_UNLIKELY(not data.initialize(name)))
			return false;

		data.startHeaderheaderGuardID();
		data.generateH();
		data.generateHXX();
		data.generateCPP();
		data.endHeaderheaderGuardID();
		return data.writeFiles();
	}





} // namespace PEG
} // namespace Parser
} // namespace Yuni


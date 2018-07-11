#ifndef __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_COMPOUND_H__
# define __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_COMPOUND_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <map>
# include <deque>
# include <vector>


namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{

	//! Any kind of compound
	enum CompoundType
	{
		kdUnknown = 0,
		kdNamespace,
		kdClass,
		kdTypedef,
		kdEnum,
		kdFunction,
		kdVariable,
		kdFile,
		kdFolder,
		kdGroup,
		kdFriend,
		kdMax
	};


	class Parameter
	{
	public:
		typedef SmartPtr<Parameter>  Ptr;
		typedef std::vector<Ptr>  Vector;

	public:
		String name;
		String type;
	};


	class Member
	{
	public:
		typedef SmartPtr<Member>  Ptr;
		typedef std::vector<Ptr>  Vector;

	public:
		CompoundType  kind;
		String id;
		String htmlID;
		String name;
		String brief;
		String detailedDescription;
		CString<16,false>  visibility;
		String type;
		bool isStatic;
		bool isConst;
		bool isExplicit;
		bool isInline;
		Parameter::Vector parameters;
		Parameter::Vector templates;

	}; // class Member



	class Section
	{
	public:
		typedef SmartPtr<Section>  Ptr;
		typedef std::deque<Ptr>  Deque;
		typedef std::vector<Ptr> Vector;

	public:
		String caption;
		CString<32,false>  kind;
		CString<16,false>  visibility;
		bool isStatic;
		Member::Vector members;

	}; // class Section



	class Compound
	{
	public:
		//! Reference ID
		typedef CString<256,false> RefID;
		//! The most suitable smart ptr
		typedef SmartPtr<Compound>  Ptr;
		//! Map
		typedef std::map<RefID, Ptr>  Map;
		//! Deque
		typedef std::deque<Ptr>  Deque;

		//! All symbols, ordered by their type
		typedef Map*  MapPerKind;

	public:
		template<class StringT> static CompoundType  StringToKind(const StringT& str);
		template<class StreamT> static void AppendKindToString(StreamT& out, CompoundType kind);

	public:
		Compound();
		~Compound();

		void prepare();


	public:
		//! Kind of compound
		CompoundType kind;
		//! Reference ID
		RefID refid;
		//!
		String name;

		String htdocs;

		//! Brief
		String brief;
		//! Detailed description
		String description;

		Map members;

		//! The parent compound
		Compound* parent;
		//! All sections
		Section::Vector sections;

	}; // class Compound










	//! All symbols ordered by their reference id
	extern Compound::Map  allSymbolsByRefID;

	//! All symbols, ordered by their type and their reference id
	extern Compound::MapPerKind  allSymbols;

	//! All symbols, ordered by their type and their name
	extern Compound::MapPerKind  allSymbolsByName;




} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

# include "compound.hxx"

#endif // __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_COMPOUND_H__

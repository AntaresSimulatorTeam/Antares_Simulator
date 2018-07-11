#ifndef __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_COMPOUND_HXX__
# define __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_COMPOUND_HXX__


namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{


	template<class StringT>
	CompoundType Compound::StringToKind(const StringT& str)
	{
		if (str == "function")
			return kdFunction;
		else if (str == "class")
			return kdClass;
		else if (str == "struct")
			return kdClass;
		else if (str == "variable")
			return kdVariable;
		else if (str == "typedef")
			return kdTypedef;
		else if (str == "enum")
			return kdEnum;
		else if (str == "file")
			return kdFile;
		else if (str == "namespace")
			return kdNamespace;
		else if (str == "dir")
			return kdFolder;
		else if (str == "group")
			return kdGroup;
		else if (str == "friend")
			return kdFriend;
		return kdUnknown;
	}



	template<class StreamT>
	void Compound::AppendKindToString(StreamT& out, CompoundType kind)
	{
		switch (kind)
		{
			case kdClass:      out << "class";break;
			case kdNamespace:  out << "namespace";break;
			case kdFunction:   out << "function";break;
			case kdTypedef:    out << "typedef";break;
			case kdEnum:       out << "enum";break;
			case kdVariable:   out << "variable";break;
			case kdFile:       out << "file";break;
			case kdFolder:     out << "folder";break;
			case kdGroup:      out << "group";break;
			case kdFriend:     out << "friend";break;
			case kdUnknown:break;
			case kdMax:break;
		}
	}





} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

#endif // __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_COMPOUND_HXX__

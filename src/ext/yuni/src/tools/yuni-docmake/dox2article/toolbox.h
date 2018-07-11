#ifndef __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_TOOLBOX_H__
# define __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_TOOLBOX_H__

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

	template<class StringT>
	static void HtmlEntities(StringT& out, const AnyString& string)
	{
		out = string;
		out.replace("&", "&amp;");
		out.replace("<", "&lt;");
		out.replace(">", "&gt;");
	}




	template<class StringT>
	static void ArrangeTypename(StringT& string)
	{
		string.replace(" *", "*");
		string.replace(" &amp;", "&amp;");
		string.replace(" &lt; ", "&lt;");
		string.replace(" &gt; ", "&gt;");
		string.replace(" &gt;","&gt;");
		string.replace(" , ", ", ");
	}


	template<class StringT1, class StringT2>
	static void PrepareTitle(StringT1& out, const StringT2& string)
	{
		out = string;
		out.replace("<br />", "");
		out.replace("<br>", ""); // should never happen
		out.replace("<u>", "");
		out.replace("</u>", "");
		out.replace("<b>", "");
		out.replace("</b>", "");
		out.replace("<div>", "");
		out.replace("</div>", "");
		out.replace("<p>", "");
		out.replace("</p>", "");
		out.trim(" \t\r\n:.;");
	}





} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

#endif // __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_TOOLBOX_H__

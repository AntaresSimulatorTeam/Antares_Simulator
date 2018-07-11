#ifndef __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_READ_INDEX_H__
# define __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_READ_INDEX_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>


namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{


	/*!
	** \brief Read the entire catalog index of a doxygen folder (index.xml)
	*/
	bool ReadXMLCatalogIndex(const String& folder);



} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

#endif // __YUNI_TOOL_DOCMAKE_DOXYGEN_2_ARTICLE_READ_INDEX_H__

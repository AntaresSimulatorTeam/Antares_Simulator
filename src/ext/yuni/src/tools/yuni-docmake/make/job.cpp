
#include "job.h"
#include "../logs.h"
#include <yuni/io/io.h>
#include "../tinyxml/tinyxml.h"
#include <iostream>
#include <yuni/core/slist.h>
#include "indexes.h"
#include "job-writer.h"
#include "program.h"


#define SEP  IO::Separator

using namespace Yuni;
using namespace Yuni::Tool::DocMake;


Yuni::Job::QueueService<>  queueService;



namespace // anonymous
{

	class XMLVisitor : public TiXmlVisitor
	{
	public:
		//! \name Constructor
		//@{
		/*!
		** \brief Constructor
		*/
		XMLVisitor(ArticleData& article, TiXmlDocument& document);
		//! Destructor
		virtual ~XMLVisitor();
		//@}

		virtual bool VisitEnter(const TiXmlDocument& /*doc*/ );

		virtual bool VisitExit(const TiXmlDocument& /*doc*/);

		virtual bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute* attr);

		virtual bool VisitExit(const TiXmlElement& element);

		virtual bool Visit(const TiXmlDeclaration& /*declaration*/);
		virtual bool Visit(const TiXmlText& /*text*/);
		virtual bool Visit(const TiXmlComment& /*comment*/);
		virtual bool Visit(const TiXmlUnknown& /*unknown*/);

		bool error() const {return pArticle.error;}

	private:
		void pushCoeff(float coeff);

		void popCoeff();

		void pushCoeffFromString(const TIXML_STRING& name);

		void seo(const AnyString& string);

	private:
		//!
		float pCoeff;

		//! Within a paragraph
		bool pWithinParagraph;
		//! XML document
		TiXmlDocument& pDocument;
		//! XML Filename
		const String& pFilename;
		//! Current state
		ArticleData::State pState;
		ArticleData& pArticle;

		//! Last TOC level (1: h2, 2: h3...)
		unsigned int pLastTOCLevel;
		String pTOCCaption;

		enum { coeffStackMax = 256 };
		float pCoeffStack[coeffStackMax];
		unsigned int pCoeffStackIndex;
	}; // class XMLVisitor





	XMLVisitor::XMLVisitor(ArticleData& article, TiXmlDocument& document) :
		pCoeff(1.f),
		pWithinParagraph(false),
		pDocument(document),
		pFilename(article.relativeFilename),
		pState(ArticleData::stNone),
		pArticle(article),
		pLastTOCLevel(0),
		pCoeffStackIndex(0)
	{
		memset(pCoeffStack, 0, sizeof(pCoeffStack));
	}


	XMLVisitor::~XMLVisitor()
	{
	}


	bool XMLVisitor::VisitEnter(const TiXmlDocument& /*doc*/ )
	{
		return true;
	}


	bool XMLVisitor::VisitExit(const TiXmlDocument& /*doc*/)
	{
		return true;
	}


	bool XMLVisitor::VisitEnter(const TiXmlElement& element, const TiXmlAttribute* /*attr*/)
	{
		const TIXML_STRING name = element.ValueTStr();
		const Dictionary::Tag tag = name.c_str();

		pushCoeffFromString(name);
		switch (pState)
		{
			case ArticleData::stNone:
				{
					if (tag.startsWith("pragma:"))
					{
						bool value;
						if (tag == "pragma:tag")
						{
							const AnyString string1 = element.Attribute("value");
							pArticle.insertTags(string1);
							const AnyString string2 = element.Attribute("name");
							pArticle.insertTags(string2);
						}
						else if (tag == "pragma:toc")
						{
							if (TIXML_SUCCESS == element.QueryBoolAttribute("value", &value))
								logs.warning() << pFilename << ": pragma:toc: the field value is deprecated";
							if (TIXML_SUCCESS == element.QueryBoolAttribute("visible", &value))
								pArticle.showTOC = value;
						}
						else if (tag == "pragma:lang")
						{
							// see http://www.seoconsultants.com/meta-tags/language
							const AnyString string = element.Attribute("value");
							pArticle.language = string;
							pArticle.language.trim();
							pArticle.language.toLower();
						}
						else if (tag == "pragma:quicklinks")
						{
							if (TIXML_SUCCESS == element.QueryBoolAttribute("value", &value))
								pArticle.showQuickLinks = value;
							else
								logs.error() << pFilename << ": invalid value for pragma:quicklinks";
						}
						else if (tag == "pragma:history")
						{
							if (TIXML_SUCCESS == element.QueryBoolAttribute("value", &value))
								pArticle.showHistory = value;
							else
								logs.error() << pFilename << ": invalid value for pragma:history";
						}
						else if (tag == "pragma:directoryindex")
						{
							const AnyString string = element.Attribute("src");
							if (not string.empty())
							{
								String src = pArticle.htdocsFilename;
								src << SEP << string;
								IO::Normalize(pArticle.directoryIndex, src);
							}
							else
							{
								String src = pArticle.htdocsFilename;
								src << SEP << ".";
								IO::Normalize(pArticle.directoryIndex, src);
							}

							const CString<42,false> content = element.Attribute("content");
							if (not content.empty())
							{
								if (content == "nofollow")
									pArticle.directoryIndexContent = ArticleData::dicNoFollow;
								else if (content == "noindex")
									pArticle.directoryIndexContent = ArticleData::dicNoIndex;
								else if (content == "all")
									pArticle.directoryIndexContent = ArticleData::dicAll;
								else
									logs.error() << "invalid directory index content flag (expected: 'all', 'nofollow' or 'noindex')";
							}

							if (!string && !content)
								logs.warning() << pFilename << ": pragma:directoryindex: missing attribute 'src' or 'content'";
						}
						else if (tag == "pragma:accesspath")
						{
							CString<32,false> string = element.Attribute("value");
							string.toLower();
							if (string != "quicklinks")
								logs.error() << pFilename << ": invalid access path overlay";
							else
								pArticle.accessPath = string;
						}
						else if (tag == "pragma:weight")
						{
							CString<32,false> string = element.Attribute("value");
							float weight;
							if (!string.to(weight) || weight < 0.f || weight > 10.f)
								logs.error() << pFilename << ": invalid page weight (decimal, range 0..10)";
							else
								pArticle.pageWeight = weight;
						}
						else
							logs.warning() << pFilename << ": unknown setting: " << tag;
					}
					else
					{
						if (tag == "title")
						{
							pState = ArticleData::stTitle;
							pArticle.title.clear();
						}
						else if (tag == "h2")
						{
							pLastTOCLevel = 1;
							pTOCCaption.clear();
							pState = ArticleData::stTOCItem;
						}
						else if (tag == "h3")
						{
							pTOCCaption.clear();
							if (!pLastTOCLevel)
							{
								logs.error() << "found h3 without h2. Adding an empty h2";
								pArticle.tocAppend(1, pTOCCaption);
							}
							pLastTOCLevel = 2;
							pState = ArticleData::stTOCItem;
						}
						else if (tag == "h1")
						{
							logs.error() << "The tag h1 is reserved for the page title";
							return false;
						}
						else if (tag == "tag")
						{
							const AnyString string1 = element.Attribute("value");
							pArticle.insertTags(string1);
							const AnyString string2 = element.Attribute("name");
							pArticle.insertTags(string2);
						}
					}
					if (pWithinParagraph && pArticle.allowedTagsInParagraph.find(tag) == pArticle.allowedTagsInParagraph.end())
					{
						logs.error() << pFilename << ": invalid tag within a paragraph";
						pArticle.error = true;
						return false;
					}
					if (tag == "p")
						pWithinParagraph = true;
					break;
				}
			case ArticleData::stTitle:
				{
					logs.error() << pFilename << ": invalid nested tag for 'title'";
					pArticle.error = true;
					return false;
				}
			case ArticleData::stTOCItem:
				{
					if (tag == "h2" || tag == "h3")
					{
						logs.error() << "invalid nested header  (h2,h3)";
						return false;
					}
					break;
				}
			default:
				break;
		}
		return true;
	}


	bool XMLVisitor::Visit(const TiXmlText& node)
	{
		const TIXML_STRING& text = node.ValueTStr();

		switch (pState)
		{
			case ArticleData::stTitle:
				{
					assert(pArticle.title.capacity() < 1024);
					if (pArticle.title.size() + (unsigned int) text.size() > 512)
						logs.error() << "invalid title length (> 512)";
					else
					{
						if (!pArticle.title.empty() && pArticle.title.last() != ' ')
							pArticle.title += ' ';
						pArticle.title.append(text.c_str(), (unsigned int)text.size());
						pArticle.title.trim();
					}
					break;
				}
			case ArticleData::stTOCItem:
				{
					assert(pTOCCaption.capacity() < 1024);
					if (pTOCCaption.size() + text.size() > 512)
						logs.error() << "invalid caption length (>512)";
					else
					{
						if (!pTOCCaption.empty() && pTOCCaption.last() != ' ')
							pTOCCaption += ' ';
						pTOCCaption.append(text.c_str(), (unsigned int)text.size());
						pTOCCaption.trim();
					}
					break;
				}
			default:
				if (!text.empty())
				{
					AnyString adapter;
					adapter.adapt(text.c_str(), (unsigned int) text.size());
					seo(adapter);
				}
				break;
		}
		return true;
	}


	bool XMLVisitor::Visit(const TiXmlComment&)
	{
		return true;
	}


	bool XMLVisitor::Visit(const TiXmlDeclaration&)
	{
		return true;
	}


	bool XMLVisitor::Visit(const TiXmlUnknown&)
	{
		return true;
	}



	bool XMLVisitor::VisitExit(const TiXmlElement& element)
	{
		const TIXML_STRING& name = element.ValueTStr();

		switch (pState)
		{
			case ArticleData::stNone:
				{
					if (name == "p")
						pWithinParagraph = false;
				}
			case ArticleData::stTitle:
				{
					if (name == "title")
					{
						pState = ArticleData::stNone;
						AnyString s = pArticle.title;

						float oldCoeff = pCoeff;
						pCoeff = 5.f;
						seo(s);
						pCoeff = oldCoeff;
					}
					break;
				}
			case ArticleData::stTOCItem:
				{
					if (name == "h2" || name == "h3")
					{
						pState = ArticleData::stNone;
						float oldCoeff = pCoeff;
						pCoeff = (pLastTOCLevel == 1) ? 3.f : 2.f;
						AnyString s = pTOCCaption;
						seo(s);
						pCoeff = oldCoeff;
						pArticle.tocAppend(pLastTOCLevel, pTOCCaption);
						pTOCCaption.clear();
					}
					break;
				}
			default:
				break;
		}

		popCoeff();
		return true;
	}


	void XMLVisitor::pushCoeff(float coeff)
	{
		if (pCoeffStackIndex < coeffStackMax)
			pCoeffStack[pCoeffStackIndex] = coeff;
		++pCoeffStackIndex;
		pCoeff *= coeff;
	}


	void XMLVisitor::popCoeff()
	{
		if (pCoeffStack)
		{
			pCoeff = 1.f;
			for (unsigned int i = 0; i != pCoeffStackIndex; ++i)
				pCoeff *= pCoeffStack[i];
			--pCoeffStackIndex;
		}
		else
		{
			pCoeff = 1.f;
			pCoeffStackIndex = 0;
		}
	}


	void XMLVisitor::pushCoeffFromString(const TIXML_STRING& name)
	{
		if (name.empty())
		{
			pushCoeff(1.f);
			return;
		}
		if (name == "h2")
		{
			pushCoeff(3.f);
			return;
		}
		if (name == "h3")
		{
			pushCoeff(2.0f);
			return;
		}
		if (name == "h4")
		{
			pushCoeff(1.7f);
			return;
		}
		if (name == "h5")
		{
			pushCoeff(1.60f);
			return;
		}
		if (name == "h6")
		{
			pushCoeff(1.55f);
			return;
		}
		if (name == "b")
		{
			pushCoeff(1.1f);
			return;
		}
		if (name == "i")
		{
			pushCoeff(1.05f);
			return;
		}
		if (name == "var")
		{
			pushCoeff(0.f);
			return;
		}
		if (name == "pre")
		{
			pushCoeff(0.95f);
			return;
		}
		if (name == "code")
		{
			pushCoeff(0.95f);
			return;
		}
		pushCoeff(1.f);
	}


	void XMLVisitor::seo(const AnyString& string)
	{
		if (pCoeff < 0.1f)
			return;
		typedef Yuni::LinkedList<String> List;
		List list;
		string.split(list, " \r\n\t:;!@#$%^&*()_+{}[]|\"'\\/.,?><=-");
		if (list.empty())
			return;

		Dictionary::Word word;
		const List::const_iterator end = list.end();
		for (List::const_iterator i = list.begin(); i != end; ++i)
		{
			word = *i;
			word.toLower();

			Dictionary::WordStat& stat = pArticle.wordCount[word];
			++stat.count;
			if (pCoeff > stat.coeff)
				stat.coeff = pCoeff;
		}
	}




} // namespace anonymous









CompileJob::CompileJob(const Yuni::String& input, const Yuni::String& htdocs) :
	pInput(input),
	pHtdocs(htdocs)
{
}


CompileJob::~CompileJob()
{
}


bool CompileJob::extractOrder(const String& path)
{

	// Getting the raw folder name
	// For the following URL :
	// /Users/milipili/projects/yuni/sources/docs/docs/src/001-en/300-developers/100-recommended-softwares/article.xml
	// We will get
	// /Users/milipili/projects/yuni/sources/docs/docs/src/001-en/300-developers/100-recommended-softwares
	IO::ExtractFilePath(pTmp, path);

	// Looking for the final slash
	const String::Size offset = pTmp.find_last_of("/\\");
	if (offset == String::npos || offset + 4 >= pTmp.size())
		return false;
	// We only want to match string like /XXX-xxxxxx...
	if (pTmp[offset + 4] != '-')
		return false;

	// We are optimistic. The first 3 chars should be a number most of the time
	CString<8,false> s;
	s.resize(3);
	s[2] = pTmp[offset + 3];
	s[1] = pTmp[offset + 2];
	s[0] = pTmp[offset + 1];
	if (!s.to(pArticle.order))
		return false;
	return (pArticle.order < 1000u);
}



void CompileJob::onExecute()
{
	String target;
	unsigned int sourceCount = (unsigned int)pSources.size();
	for (unsigned int  i = 0; i != sourceCount; ++i)
	{
		assert(i < pSources.size());
		const String& entry = pSources[i];

		// Resetting data related to the article
		pArticle.reset();
		// The original filename (absolute)
		pArticle.originalFilename = entry;
		// The relative filename, from the source folder
		pArticle.relativeFilename.assign(entry.c_str() + pInput.size() + 1, entry.size() - pInput.size() - 1);

		// Last modification time
		pArticle.modificationTime = IO::File::LastModificationTime(entry);

		// The final filename within the htdocs folder
		pArticle.htdocsFilename.clear() << '/' << pArticle.relativeFilename;
		pArticle.htdocsFilename.replace('\\', '/');
		{
			String::Size offset = pArticle.htdocsFilename.find_last_of('/');
			if (offset != String::npos)
				pArticle.htdocsFilename.resize(offset);
		}
		if (!pArticle.htdocsFilename)
			pArticle.htdocsFilename = "/";

		String::Size offset = 0;
		do
		{
			offset = pArticle.htdocsFilename.find('/', offset);
			if (pArticle.htdocsFilename.size() < 4 || offset > pArticle.htdocsFilename.size() - 4)
				break;
			++offset;
			if (pArticle.htdocsFilename[offset + 3] != '-')
				break;

			bool isDigit = true;
			for (unsigned int j = offset; j < offset + 3; ++j)
			{
				if (!String::IsDigit(pArticle.htdocsFilename[j]))
				{
					isDigit = false;
					break;
				}
			}
			if (isDigit)
				pArticle.htdocsFilename.erase(offset, 4);
		}
		while (true);

		// Post analyzis about the article
		if (!analyzeArticle() || pArticle.error)
			continue;

		// Writing the article in the database
		DocIndex::Write(pArticle);
		// Preparing a new job, with a copy of the data related to the article
		JobWriter::Add(pInput, pHtdocs, pArticle);
	}
}


bool CompileJob::analyzeArticle()
{
	if (Program::verbose)
		logs.info() << "reading " << pArticle.relativeFilename;

	// Article order
	{
		if (!extractOrder(pArticle.originalFilename))
			pArticle.order = 1000u;
		if (Program::debug)
			logs.info() << "  :: " << pArticle.relativeFilename << ", order = " << pArticle.order;
	}

	TiXmlDocument doc;
	if (!doc.LoadFile(pArticle.originalFilename.c_str(), TIXML_ENCODING_UTF8))
	{
		logs.error() << pArticle.relativeFilename << ", l" << doc.ErrorRow() << ": " << doc.ErrorDesc();
		return false;
	}

	// Analyze the XML document
	XMLVisitor visitor(pArticle, doc);
	if (!doc.Accept(&visitor) || visitor.error())
	{
		pArticle.error = true;
		return false;
	}
	if (!pArticle.title)
		IO::ExtractFileName(pArticle.title, pArticle.htdocsFilename, false);

	// TOC refactoring
	pArticle.tocRefactoring();

	return true;
}



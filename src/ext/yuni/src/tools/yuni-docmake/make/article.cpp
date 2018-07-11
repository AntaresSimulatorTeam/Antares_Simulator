
#include "article.h"
#include "../logs.h"
#include "program.h"
#include "indexes.h"

using namespace Yuni;
using namespace Yuni::Tool::DocMake;



ArticleData::ArticleData() :
	error(true)
{
	allowedTagsInParagraph.insert("a");
	allowedTagsInParagraph.insert("b");
	allowedTagsInParagraph.insert("i");
	allowedTagsInParagraph.insert("u");
	allowedTagsInParagraph.insert("br");
	allowedTagsInParagraph.insert("code");
	allowedTagsInParagraph.insert("sub");
	allowedTagsInParagraph.insert("sup");
	allowedTagsInParagraph.insert("big");
	allowedTagsInParagraph.insert("button");
	allowedTagsInParagraph.insert("em");
	allowedTagsInParagraph.insert("img");
	allowedTagsInParagraph.insert("input");
	allowedTagsInParagraph.insert("kbd");
	allowedTagsInParagraph.insert("small");
	allowedTagsInParagraph.insert("span");
	allowedTagsInParagraph.insert("strong");
	allowedTagsInParagraph.insert("textarea");
}


ArticleData::~ArticleData()
{
}


void ArticleData::reset()
{
	assert(title.capacity() < 1024);
	assert(accessPath.capacity() < 1024 * 1024);

	id = -1;
	modificationTime = 0;
	pageWeight = 1.0f;
	coeff = 1.0f;
	error = false;
	order = (unsigned int) -1;
	showTOC = true;
	showHistory = true;
	showQuickLinks = true;
	directoryIndexContent = dicAll;

	language = "en";
	title.clear();
	tags.clear();
	directoryIndex.clear();
	accessPath.clear();
	tocItems.clear();
	wordCount.clear();
}



void ArticleData::tocAppend(unsigned int level, const String& caption)
{
	TOCItem* item = new TOCItem();
	String& itID = item->hrefID;
	String& itCaption = item->caption;

	item->level = level;
	itCaption = caption;
	itCaption.trim();
	if (!itCaption)
		itCaption = "Missing Header";

	bool lastWasInvalid = true;
	const String::const_utf8iterator end = itCaption.utf8end();
	for (String::const_utf8iterator i = itCaption.utf8begin(); i != end; ++i)
	{
		const char c = (char) *i;
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
		{
			itID += c;
			lastWasInvalid = false;
		}
		else
		{
			if (!lastWasInvalid)
				itID += '_';
			lastWasInvalid = true;
		}
	}
	itID.trim('_');
	itID.toLower();

	// Adding line feed
	itCaption.replace(". ", ".<br />");

	// adding the new toc item
	tocItems.push_back(item);
}


void ArticleData::tocRefactoring()
{
	if (tocItems.size() <= 1)
	{
		tocItems.clear();
		showTOC = false;
	}
	else
	{
		typedef std::set<String>  Set;
		Set  set;
		String tmp;
		for (unsigned int i = 0; i != tocItems.size(); ++i)
		{
			TOCItem& item = *(tocItems[i]);
			String& id = item.hrefID;
			if (set.find(id) != set.end())
			{
				for (unsigned int rank = 2; rank < 1000; ++rank)
				{
					tmp.clear() << id << '_' << rank;
					if (set.find(tmp) == set.end())
					{
						id << '_' << rank;
						break;
					}
				}
			}
			set.insert(id);

			if (Program::debug)
				logs.info() << "  :: " << relativeFilename << ": h" << item.level << ' ' << item.caption;
		}
	}
}


void ArticleData::insertTags(const AnyString& text)
{
	if (!text)
		return;
	Dictionary::Tag::Vector list;
	text.split(list, " ,;|\t\r\n/\\");
	if (!list.empty())
	{
		const Dictionary::Tag::Vector::iterator end = list.end();
		for (Dictionary::Tag::Vector::iterator i = list.begin(); i != end; ++i)
		{
			(*i).toLower();
			tags.insert(*i);
		}
	}
}


void ArticleData::reloadTagsFromDatabase()
{
	DocIndex::RetrieveTagList(*this);
}



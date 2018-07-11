#ifndef __YUNI_DOCMAKE_ARTICLE_H__
# define __YUNI_DOCMAKE_ARTICLE_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include "dictionary.h"
# include <deque>
# include <set>


	class ArticleData
	{
	public:
		typedef std::deque<float>  CoeffStack;


		enum State
		{
			stNone = 0,
			stTitle,
			stTOCItem,
		};

		enum DirectoryIndexContent
		{
			//! The article won't appear in a directory index
			dicNoIndex = 0,
			//! Only the node will be added
			dicNoFollow,
			//! The node + all its children will be added
			dicAll,
			//! The maximum number of flags
			dicMax,
		};

		class TOCItem
		{
		public:
			typedef Yuni::SmartPtr<TOCItem>  Ptr;
			typedef std::vector<Ptr> Vector;

		public:
			//! Item level (1: h1, 2: h2)
			unsigned int level;
			//! ID
			Yuni::String hrefID;
			//! Caption
			Yuni::String caption;
		};

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		ArticleData();
		//! Destructor
		~ArticleData();
		//@}

		void reset();

		void tocAppend(unsigned int level, const Yuni::String& caption);
		void tocRefactoring();

		/*!
		** \brief Insert a list of tags
		**
		** \param text A list of tags, separated by spaces or special characters
		*/
		void insertTags(const AnyString& text);

		void reloadTagsFromDatabase();

	public:
		//! Article ID
		Yuni::sint64 id;
		//! The original XML file
		Yuni::String originalFilename;
		//! Relative filename
		Yuni::String relativeFilename;
		//! The target filename within the htdocs folder
		Yuni::String htdocsFilename;

		//! Language
		Yuni::CString<16,false> language;

		//! Date of the last modification
		Yuni::sint64 modificationTime;

		//!
		Dictionary::TagSet  allowedTagsInParagraph;
		//! Page weight
		float pageWeight;
		//! The current coefficient
		float coeff;
		//! Title of the page
		Yuni::String title;

		//! A non empty value to force the display of the access path
		Yuni::String accessPath;

		//! An error has occured
		bool error;

		//! Order
		unsigned int order;

		//! Directory index
		Yuni::String directoryIndex;
		//! Flag to display the TOC (Table Of Content)
		bool showTOC;
		//! Flag to display quick links (on the left)
		bool showQuickLinks;
		//! Flag to display the page history
		bool showHistory;

		//! SEO
		Dictionary::WordsCount wordCount;
		//! TOC items
		TOCItem::Vector tocItems;
		//! Tags
		Dictionary::TagSet tags;

		//! Directory content index
		DirectoryIndexContent directoryIndexContent;

	}; // class ArticleData




#endif // __YUNI_DOCMAKE_ARTICLE_H__

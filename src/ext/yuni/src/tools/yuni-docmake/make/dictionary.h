#ifndef __DICTIONARY_H__
# define __DICTIONARY_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <map>
# include <set>


namespace Dictionary
{

	//! A single word
	typedef Yuni::CString<32,false>  Word;
	//! A single tag
	typedef Yuni::CString<42,false>  Tag;

	//! Word ID
	typedef int  WordID;

	/*!
	** \brief Statistics for a single word
	*/
	class WordStat
	{
	public:
		WordStat() :
			coeff(0.f), count(0)
		{}
		float coeff;
		unsigned int count;
	};


	//! Statistics for a set of words
	typedef std::map<Word, WordStat>  WordsCount;

	//! Set of tags
	typedef std::set<Tag>  TagSet;


	//! Association between a word and its ID
	typedef std::map<Word, WordID>  AllWords;


	extern Yuni::Mutex mutex;

	extern AllWords  allWords;



	/*!
	** \brief Preload data from the cache
	*/
	void PreloadFromIndexDB();

	/*!
	** \brief
	** \return -1 if not found
	*/
	WordID FindWordID(const Word& word);


} // namespace Dictionary

#endif // __DICTIONARY_H__

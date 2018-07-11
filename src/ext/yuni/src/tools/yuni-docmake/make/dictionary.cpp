
#include "dictionary.h"
#include "../sqlite/sqlite3.h"
#include "indexes.h"
#include "../logs.h"
#include "program.h"



using namespace Yuni;


namespace Dictionary
{

	AllWords  allWords;

	Yuni::Mutex mutex;



	WordID FindWordID(const Word& word)
	{
		Yuni::MutexLocker locker(mutex);
		const AllWords::const_iterator it = allWords.find(word);
		if (it == allWords.end())
			return -1;
		return it->second;
	}


	void PreloadFromIndexDB()
	{
		Yuni::MutexLocker locker(mutex);
		allWords.clear();
		sqlite3* handle = (sqlite3*) DocIndex::DatabaseHandle();
		if (!handle)
			return;

		char** result;
		int rowCount, colCount;
		if (SQLITE_OK != sqlite3_get_table(handle, "SELECT id,term FROM terms", &result, &rowCount, &colCount, NULL))
			return;

		if (rowCount)
		{
			if (!Tool::DocMake::Program::quiet)
			{
				if (rowCount == 1)
					logs.info() << "Preloading 1 term from the index db";
				else
					logs.info() << "Preloading " << rowCount << " terms from the index db";
			}

			unsigned int y = 2;
			for (unsigned int row = 0; row < (unsigned int) rowCount; ++row)
			{
				const AnyString sid  = result[y++];
				const Word term = result[y++];
				allWords[term] = sid.to<WordID>();
			}
		}
		sqlite3_free_table(result);
	}




} // namespace Dictionary

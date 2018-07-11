
#include "indexes.h"
#include "../sqlite/sqlite3.h"
#include <yuni/io/file.h>
#include <yuni/core/math.h>
#include <yuni/datetime/timestamp.h>
#include "../logs.h"
#include "index-db.hxx"
#include "program.h"
#ifndef YUNI_OS_WINDOWS
# include "stdlib.h" // man 3 system
#endif


# define SEP IO::Separator


using namespace Yuni;
using namespace Yuni::Tool::DocMake;


namespace DocIndex
{



	namespace // anonymous
	{

		enum
		{
			dbVersion = 10, // arbitrary value
		};

		static sqlite3* gDB = nullptr;


		template<class StringT>
		static sint64 RetrieveArticleID(const StringT& href)
		{
			if (!gDB || !href)
				return -1;

			sqlite3_stmt* stmt;
			if (SQLITE_OK != sqlite3_prepare_v2(gDB, "SELECT id FROM articles WHERE html_href = $1", -1, &stmt, NULL))
				return -1;
			sqlite3_bind_text(stmt, 1, href.c_str(), href.size(), NULL);
			if (SQLITE_ROW != sqlite3_step(stmt))
			{
				sqlite3_finalize(stmt);
				return -1;
			}
			const AnyString id = (const char*) sqlite3_column_text(stmt, 0);
			sint64 result;
			if (!id.to(result))
			{
				sqlite3_finalize(stmt);
				return -1;
			}
			sqlite3_finalize(stmt);
			return result;
		}


		static bool ResetDBIndex(const String& filename)
		{
			if (!Program::quiet)
				logs.info() << "the index database needs to be rebuilt";

			// Close the sqlite handle
			Close();
			// Destroy the sqlite database
			IO::File::Delete(filename);
			// Try to reopen it
			switch (sqlite3_open(filename.c_str(), &gDB))
			{
				case SQLITE_OK:
					break;
				default:
					logs.error() << "impossible to re-open the database after deletion.";
					return false;
			}

			Clob script;
			char* message = nullptr;

			// Create tables
			PrepareSQLScript(script);
			if (SQLITE_OK != sqlite3_exec(gDB, script.c_str(), NULL, NULL, &message))
			{
				logs.error() << "database: " << message;
				sqlite3_free(message);
			}

			script.clear() << "INSERT INTO index_header (version) VALUES (" << (unsigned int) dbVersion << ");";
			if (SQLITE_OK != sqlite3_exec(gDB, script.c_str(), NULL, NULL, &message))
			{
				logs.error() << "database: " << message;
				sqlite3_free(message);
			}

			return true;
		}


		static bool UsePragma()
		{
			// UTF-8
			if (SQLITE_OK != sqlite3_exec(gDB, "PRAGMA encoding = \"UTF-8\"; ", NULL, NULL, NULL))
			{
				logs.error() << "impossible to use the UTF8 encoding";
				return false;
			}
			// Foreign keys
			if (SQLITE_OK != sqlite3_exec(gDB, "PRAGMA foreign_keys = 1;", NULL, NULL, NULL))
			{
				logs.error() << "impossible to enable foreign keys";
				return false;
			}
			return true;
		}


		static bool CheckDatabaseIntegrity(const String& dbfilename)
		{
			// prepare the SQL statement from the command line
			static const char* const query = "SELECT version,dirty FROM index_header";

			sqlite3_stmt* stmt;
			if (SQLITE_OK != sqlite3_prepare_v2(gDB, query, -1, &stmt, 0))
			{
				sqlite3_finalize(stmt);
				if (!ResetDBIndex(dbfilename))
					return false;
				if (SQLITE_OK != sqlite3_prepare_v2(gDB, query, -1, &stmt, 0))
				{
					sqlite3_finalize(stmt);
					stmt = nullptr;
					if (!Program::quiet)
						logs.info() << "The database index format needs update. Performing a full reindex";
				}
			}
			if (stmt && SQLITE_ROW == sqlite3_step(stmt))
			{
				const AnyString version = (const char*) sqlite3_column_text(stmt, 0);
				const AnyString dirty   = (const char*) sqlite3_column_text(stmt, 1);
				if (dirty.to<bool>())
				{
					logs.info() << "The database index is marked as dirty. Performing a full reindex";
				}
				else
				{
					if (version.to<int>() == dbVersion)
					{
						sqlite3_finalize(stmt);
						return true;
					}
					if (!Program::quiet)
						logs.info() << "The database index format needs update. Performing a full reindex";
				}
			}
			sqlite3_finalize(stmt);
			return ResetDBIndex(dbfilename);
		}

	} // anonymous namespace


	void* DatabaseHandle()
	{
		return gDB;
	}


	bool Open()
	{
		if (gDB)
		{
			sqlite3_close(gDB);
			gDB = nullptr;
		}

		if (Program::clean)
			IO::File::Delete(Program::indexCacheFilename);

		switch (sqlite3_open(Program::indexCacheFilename.c_str(), &gDB))
		{
			case SQLITE_OK:
				{
					if (!UsePragma() || !CheckDatabaseIntegrity(Program::indexCacheFilename))
					{
						Close();
						return false;
					}
					break;
				}
			case SQLITE_PERM:
				logs.error() << "not enough permissions to open " << Program::indexCacheFilename;
				return false;
			case SQLITE_BUSY:
				logs.error() << "The index database is locked.";
				return false;
			case SQLITE_CORRUPT:
				logs.error() << "The index database is malformed";
				return false;
			case SQLITE_CANTOPEN:
				logs.error() << "Unable to open " << Program::indexCacheFilename;
				return false;
			default:
				return false;
		}

		// Mark the database index as dirty
		if (SQLITE_OK != sqlite3_exec(gDB, "UPDATE index_header SET dirty = 1", NULL, NULL, NULL))
			return false;
		return true;
	}


	void Close()
	{
		if (gDB)
		{
			// Mark the database index as non-dirty
			sqlite3_exec(gDB, "UPDATE index_header SET dirty = 0", NULL, NULL, NULL);
			// Close the handle
			sqlite3_close(gDB);
			gDB = nullptr;
		}
	}


	void Vacuum()
	{
		if (gDB)
			sqlite3_exec(gDB, "VACUUM;", NULL, NULL, NULL);
	}


	Yuni::sint64 ArticleLastModificationTimeFromCache(const String& filename)
	{
		if (!filename)
			return -1;

		// prepare the SQL statement from the command line
		sqlite3_stmt* stmt;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, "SELECT modified FROM articles WHERE rel_path = $1", -1, &stmt, 0))
			return -1;
		sqlite3_bind_text(stmt, 1, filename.c_str(), filename.size(), NULL);

		if (SQLITE_ROW == sqlite3_step(stmt))
		{
			const AnyString modified = (const char*) sqlite3_column_text(stmt, 0);
			sint64 value;
			if (!modified.to<sint64>(value))
				value = -1;
			sqlite3_finalize(stmt);
			return value;
		}
		return -1;
	}


	void Write(ArticleData& article)
	{
		if (!gDB)
			return;

		CString<256> query;

		// Delete the article, and all its data
		query.clear() << "DELETE FROM articles WHERE rel_path = $1;";
		sqlite3_stmt* stmt = nullptr;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, query.c_str(), -1, &stmt, NULL))
			logs.error() << "invalid SQL query " << query;
		else
		{
			sqlite3_bind_text(stmt, 1, article.relativeFilename.c_str(), article.relativeFilename.size(), NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		String parent = article.htdocsFilename;
		if (!parent)
			parent = '/';
		else
		{
			String::Size offset = parent.find_last_of("/\\");
			if (offset < parent.size())
			{
				if (!offset)
					parent = '/';
				else
					parent.resize(offset);
			}
		}

		// Insert the new article
		{
			query.clear() << "INSERT INTO articles (parent_order, weight, show_quick_links"
				<< ", show_history"
				<< ", show_toc, modified, rel_path,html_href, title,parent,directory_index,lang)"
				<< " VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12);";
			if (SQLITE_OK != sqlite3_prepare_v2(gDB, query.c_str(), -1, &stmt, NULL))
			{
				logs.error() << "invalid SQL query: " << query;
				return;
			}
			else
			{
				sqlite3_bind_int(stmt,    1,  (int)article.order);
				sqlite3_bind_double(stmt, 2,  article.pageWeight);
				sqlite3_bind_int(stmt,    3,  (article.showQuickLinks ? 1 : 0));
				sqlite3_bind_int(stmt,    4,  (article.showHistory ? 1 : 0)); // show history
				sqlite3_bind_int(stmt,    5,  (article.showTOC ? 1 : 0)); // show toc
				sqlite3_bind_int64(stmt,  6,  article.modificationTime);
				sqlite3_bind_text(stmt,   7,  article.relativeFilename.c_str(), article.relativeFilename.size(), NULL);
				sqlite3_bind_text(stmt,   8,  article.htdocsFilename.c_str(), article.htdocsFilename.size(), NULL);
				sqlite3_bind_text(stmt,   9,  article.title.c_str(), article.title.size(), NULL);
				sqlite3_bind_text(stmt,   10, parent.c_str(), parent.size(), NULL);
				sqlite3_bind_int(stmt,    11, (int)article.directoryIndexContent);
				sqlite3_bind_text(stmt,   12, article.language.c_str(), article.language.size(), NULL);
				sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		// Getting the article ID
		article.id = RetrieveArticleID(article.htdocsFilename);
		if (article.id < 0)
		{
			logs.error() << "Invalid article ID after inserting, " << article.htdocsFilename;
			return;
		}

		if (not article.accessPath.empty())
		{
			query.clear() << "UPDATE articles SET force_access_path = $1 WHERE rel_path = $2;";
			if (SQLITE_OK != sqlite3_prepare_v2(gDB, query.c_str(), -1, &stmt, NULL))
				logs.error() << "invalid SQL query " << query;
			else
			{
				sqlite3_bind_text(stmt, 1, article.accessPath.c_str(), article.accessPath.size(), NULL);
				sqlite3_bind_text(stmt, 2, article.relativeFilename.c_str(), article.relativeFilename.size(), NULL);
				sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		if (not article.directoryIndex.empty())
		{
			// We want UNIX-style paths
			# ifdef YUNI_OS_WINDOWS
			String directoryIndex = article.directoryIndex;
			directoryIndex.replace('\\', '/');
			# else
			const String& directoryIndex = article.directoryIndex;
			# endif
			query.clear() << "UPDATE articles SET dir_index = $1 WHERE rel_path = $2;";
			if (SQLITE_OK != sqlite3_prepare_v2(gDB, query.c_str(), -1, &stmt, NULL))
				logs.error() << "invalid SQL query " << query;
			else
			{
				sqlite3_bind_text(stmt, 1, directoryIndex.c_str(), directoryIndex.size(), NULL);
				sqlite3_bind_text(stmt, 2, article.relativeFilename.c_str(), article.relativeFilename.size(), NULL);
				sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		// TOC
		if (!article.tocItems.empty())
		{
			for (unsigned int i = 0; i != article.tocItems.size(); ++i)
			{
				// alias to the current TOC item
				assert(article.tocItems[i] && "invalid toc item");
				const ArticleData::TOCItem& item = *(article.tocItems[i]);

				query.clear() << "INSERT INTO toc (html_href,indx,lvl,href_id,caption) VALUES ($1," << i << ','
					<< item.level << ",$2,$3);";
				sqlite3_prepare_v2(gDB, query.c_str(), -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, article.htdocsFilename.c_str(), article.htdocsFilename.size(), NULL);
				sqlite3_bind_text(stmt, 2, item.hrefID.c_str(), item.hrefID.size(), NULL);
				sqlite3_bind_text(stmt, 3, item.caption.c_str(), item.caption.size(), NULL);
				sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		// Tags
		if (!article.tags.empty())
		{
			Dictionary::TagSet::const_iterator end = article.tags.end();
			for (Dictionary::TagSet::const_iterator i = article.tags.begin(); i != end; ++i)
			{
				const Dictionary::Tag& tagname = *i;

				query.clear() << "INSERT INTO tags_per_article (article_id,tagname) VALUES (" << article.id << ",$1);";
				int error;
				if (SQLITE_OK == (error = sqlite3_prepare_v2(gDB, query.c_str(), query.size(), &stmt, NULL)))
				{
					sqlite3_bind_text(stmt, 1, tagname.c_str(), tagname.size(), NULL);
					sqlite3_step(stmt);
					sqlite3_finalize(stmt);
				}
				else
					logs.error() << "impossible to register tag for " << article.htdocsFilename << ", err." << error;
			}
		}
	}


	void RemoveNonExistingEntries()
	{
		if (!gDB)
			return;

		if (!Program::quiet)
			logs.info() << "Looking for deprecated entries in the database";

		CString<512> s = "DELETE FROM articles WHERE rel_path = \"\";";
		{
			sqlite3_stmt* stmt;
			sqlite3_prepare_v2(gDB, s.c_str(), -1, &stmt, NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}

		char** result;
		int rowCount, colCount;
		if (SQLITE_OK != sqlite3_get_table(gDB, "SELECT rel_path FROM articles", &result, &rowCount, &colCount, NULL))
			return;

		if (rowCount)
		{
			if (!Program::quiet)
			{
				if (rowCount == 1)
					logs.info() << "1 article available in the index db";
				else
					logs.info() << rowCount << " articles available in the index db";
			}
			unsigned int y = 1;
			for (unsigned int row = 0; row < (unsigned int) rowCount; ++row, ++y)
			{
				const AnyString relPath = result[y];
				s.clear() << Program::input << SEP << relPath;
				if (!IO::File::Exists(s))
				{
					if (!Program::quiet)
						logs.info() << "The entry '" << relPath << "' is deprecated";

					s.clear() << "DELETE FROM articles WHERE rel_path = $1;";
					sqlite3_stmt* stmt;
					sqlite3_prepare_v2(gDB, s.c_str(), s.size(), &stmt, NULL);
					sqlite3_bind_text(stmt, 1, relPath.c_str(), relPath.size(), NULL);
					sqlite3_step(stmt);
					sqlite3_finalize(stmt);
				}
			}
		}

		sqlite3_free_table(result);
	}


	bool AppendArticleTitleFromPath(String& out, const String& path)
	{
		if (!gDB)
			return false;

		sqlite3_stmt* stmt;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, "SELECT title FROM articles WHERE html_href = $1", -1, &stmt, NULL))
			return false;
		sqlite3_bind_text(stmt, 1, path.c_str(), path.size(), NULL);
		if (SQLITE_ROW != sqlite3_step(stmt))
		{
			sqlite3_finalize(stmt);
			return false;
		}
		const AnyString title = (const char*) sqlite3_column_text(stmt, 0);
		if (!title)
		{
			sqlite3_finalize(stmt);
			return false;
		}
		out += title;
		sqlite3_finalize(stmt);
		return true;
	}




	namespace // anonymous
	{

		static void InternalBuildDirectoryIndex(Clob& out, const String& path, unsigned int level)
		{
			// We want UNIX-styles paths
			# ifdef YUNI_OS_WINDOWS
			String srcPath = path;
			srcPath.replace('\\', '/');
			# else
			const String& srcPath = path;
			# endif

			char** result;
			int rowCount, colCount;
			String query = "SELECT title, html_href,directory_index FROM articles WHERE parent = \"";
			query << srcPath << "\" AND directory_index > 0 ORDER BY parent_order ASC, title";
			if (SQLITE_OK != sqlite3_get_table(gDB, query.c_str(), &result, &rowCount, &colCount, NULL))
				return;

			if (!rowCount)
			{
				sqlite3_free_table(result);
				return;
			}

			if (!level)
			{
				for (unsigned int x = 0; x != level; ++x)
					out << '\t';
				out << "<div class=\"directoryindex\">Directory Index<br />\n";
			}
			for (unsigned int x = 0; x != level; ++x)
				out << '\t';
			out << "<ul>\n";
			unsigned int y = 3;
			for (unsigned int row = 0; row < (unsigned int) rowCount; ++row)
			{
				const AnyString title = result[y++];
				const String href  = result[y++];
				unsigned int dic = AnyString(result[y++]).to<unsigned int>();
				if (dic >= ArticleData::dicMax)
					dic = ArticleData::dicAll;

				if (!href) // must not be empty
					continue;

				for (unsigned int x = 0; x != level; ++x)
					out << '\t';
				out << "<li><a href=\"@{ROOT}";
				if (href[0] != '/')
					out << '/';
				out << href;
				if (!Program::shortUrl)
					out << "/" << Program::indexFilename;
				out << "\">" << title << "</a></li>\n";

				if (dic >= ArticleData::dicAll)
					InternalBuildDirectoryIndex(out, href, level + 1);
			}

			sqlite3_free_table(result);

			for (unsigned int x = 0; x != level; ++x)
				out << '\t';
			out << "</ul>\n";

			if (!level)
			{
				for (unsigned int x = 0; x != level; ++x)
					out << '\t';
				out << "</div>\n";
			}
		}

	} // anonymous namespace



	void BuildDirectoryIndex(Clob& out, const String& path)
	{
		out.clear();
		InternalBuildDirectoryIndex(out, path, 0);
	}



	void BuildSitemap()
	{
		String filename;
		filename << Program::htdocs << SEP << "sitemap.xml";
		if (Program::verbose)
			logs.info() << "writing " << filename;

		IO::File::Stream out;
		if (!out.openRW(filename))
		{
			logs.error() << "sitemap: impossible to write " << filename;
			return;
		}

		Clob dat;
		// Begining of the sitemap
		dat << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
			<< "<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\">\n";


		float weightMin = 0.f;
		float weightMax = 0.f;
		char** result;
		int rowCount, colCount;
		String href;

		// Looking for weights limits
		{
			const char* const query = "SELECT MIN(weight), MAX(weight) FROM articles WHERE weight > 0;";
			if (SQLITE_OK == sqlite3_get_table(gDB, query, &result, &rowCount, &colCount, NULL))
			{
				if (rowCount == 1)
				{
					unsigned int y = 2;
					const AnyString minW = result[y++];
					const AnyString maxW = result[y++];

					weightMin = minW.to<float>();
					weightMax = maxW.to<float>();

					if (Math::Equals(weightMin, weightMax))
					{
						// The two values must not be equal to avoid a division by zero
						weightMin -= 0.1f;
						weightMax += 0.1f;
					}
				}
				sqlite3_free_table(result);
			}
		}

		const char* const query = "SELECT html_href,weight,modified FROM articles WHERE weight > 0.2 ORDER BY html_href";
		if (SQLITE_OK == sqlite3_get_table(gDB, query, &result, &rowCount, &colCount, NULL))
		{
			String tmp;

			if (rowCount)
			{
				CString<64,false> formattedDate;

				unsigned int y = 3;
				for (unsigned int row = 0; row < (unsigned int) rowCount; ++row)
				{
					href = result[y++];
					const AnyString weightStr   = result[y++];
					const String modifiedStr = result[y++];

					if (href.size() <= 1) // avoid invalid paths, such as '/'
						continue;

					// Weight of the article
					float weight = weightStr.to<float>();

					// Checking if the hef is less than 2048 chars
					if (href.size() >= 2048)
					{
						logs.warning() << "sitemap: invalid href (> 2048 char), skipped : " << href;
						continue;
					}

					// Priority, for the sitemap (%)
					float priority = ((weight - weightMin) / (weightMax - weightMin));
					if (priority < 0.1f)
						priority = 0.1f;
					else
					{
						if (priority > 1.f)
							priority = 1.f;
					}

					// Escaping
					// TODO use a better routine for doing that
					href.replace("&", "&amp;");
					href.replace("'", "&apos;");
					href.replace("\"", "&quot;");
					href.replace("<", "&lt;");
					href.replace("≤", "&le;");
					href.replace(">", "&gt;");
					href.replace("≥", "&ge;");
					href.replace("//", "/");

					dat << "<url>\n";
					tmp.clear() << Program::webroot << href;
					dat << "\t<loc>" << tmp << "</loc>\n";
					// The default priority in a sitemap is 0.5
					if (!Math::Equals(priority, 0.5f))
						dat << "\t<priority>" << priority << "</priority>\n";
					dat << "\t<lastmod>";
					DateTime::TimestampToString(dat, "%Y-%m-%d", modifiedStr.to<sint64>(), false);
					dat << "</lastmod>\n";
					dat << "</url>\n";
				}
			}
			sqlite3_free_table(result);
		}
		dat << "</urlset>\n";
		out << dat;

		// trying gzip
		out.close();
		# ifndef YUNI_OS_WINDOWS
		//href.clear() << "gzip -f -9 \"" << filename << "\"";
		//system(href.c_str());
		# endif
	}



	static int TryToFindWordID(const Dictionary::Word& term)
	{
		sqlite3_stmt* stmt;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, "SELECT id FROM terms WHERE term = $1", -1, &stmt, NULL))
			return -1;
		sqlite3_bind_text(stmt, 1, term.c_str(), term.size(), NULL);
		if (SQLITE_ROW != sqlite3_step(stmt))
		{
			sqlite3_finalize(stmt);
			return -1;
		}
		const AnyString idstr = (const char*) sqlite3_column_text(stmt, 0);
		int result = -1;
		if (!(!idstr))
		{
			if (!idstr.to(result))
				result = -1;
		}
		sqlite3_finalize(stmt);
		return result;
	}


	int RegisterWordReference(const Dictionary::Word& term)
	{
		int id = TryToFindWordID(term);
		if (id < 0)
		{
			sqlite3_stmt* stmt;
			const char* const query = "INSERT INTO terms (term) VALUES ($1);";
			sqlite3_prepare_v2(gDB, query, -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, term.c_str(), term.size(), NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			// Try again
			id = TryToFindWordID(term);
		}
		return id;
	}


	int FindArticleID(const Yuni::String& href)
	{
		sqlite3_stmt* stmt;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, "SELECT id FROM articles WHERE html_href = $1", -1, &stmt, NULL))
			return -1;
		sqlite3_bind_text(stmt, 1, href.c_str(), href.size(), NULL);
		if (SQLITE_ROW != sqlite3_step(stmt))
		{
			sqlite3_finalize(stmt);
			return -1;
		}
		const AnyString idstr = (const char*) sqlite3_column_text(stmt, 0);
		int result = -1;
		if (!(!idstr))
		{
			if (!idstr.to(result))
				result = -1;
		}
		sqlite3_finalize(stmt);
		return result;
	}


	void RegisterWordIDsForASingleArticle(ArticleID articleid, const int* termid,
		const int* countInArticle,
		const float* weights,
		unsigned int count)
	{
		CString<1024> query;
		query << "BEGIN;\n";
		query << "DELETE FROM terms_per_article WHERE article_id = " << articleid << ";\n";
		for (unsigned int i = 0; i != count; ++i)
		{
			query << "INSERT INTO terms_per_article (term_id,article_id,count_in_page,weight) VALUES ("
				<< termid[i] << ','
				<< articleid << ','
				<< countInArticle[i] << ','
				<< weights[i] << ");\n";
		}
		query << "COMMIT;\n";

		sqlite3_exec(gDB, query.c_str(), NULL, NULL, NULL);
	}


	static uint64  FindMaxOccurrenceForAnyTerm()
	{
		const char* const query = "SELECT SUM(count_in_page) AS s from terms_per_article GROUP BY term_id ORDER BY s DESC LIMIT 1;";
		sqlite3_stmt* stmt;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, query, -1, &stmt, NULL))
			return 0;

		if (SQLITE_ROW != sqlite3_step(stmt))
		{
			sqlite3_finalize(stmt);
			return 0;
		}
		const AnyString rstr = (const char*) sqlite3_column_text(stmt, 0);
		const uint64 r = rstr.to<uint64>();
		sqlite3_finalize(stmt);
		return r;
	}


	static void  UpdateAllRelativeTermWeight(uint64 maxO)
	{
		char** result;
		int rowCount, colCount;
		const char* const query = "SELECT term_id, SUM(count_in_page) FROM terms_per_article GROUP BY term_id;";
		if (SQLITE_OK != sqlite3_get_table(gDB, query, &result, &rowCount, &colCount, NULL))
			return;

		CString<128 * 1024> s;
		s << "BEGIN;\n";
		if (rowCount)
		{
			unsigned int y = 1;
			for (unsigned int row = 0; row < (unsigned int) rowCount; ++row)
			{
				const AnyString termID = result[++y];
				const AnyString scount  = result[++y];
				if (!termID || !scount)
					continue;
				unsigned int count = scount.to<unsigned int>();
				double d = 1. - (count * 0.5 / (double)maxO);
				s << "UPDATE terms SET weight_rel_others = " << d << " WHERE id = " << termID << ";\n";
			}
		}

		sqlite3_free_table(result);

		if (rowCount)
		{
			s << "COMMIT;\n";
			sqlite3_exec(gDB, s.c_str(), NULL, NULL, NULL);
		}
	}


	void UpdateAllSEOWeights()
	{
		// Finding the maximum occurence of any term
		{
			uint64 maxO = FindMaxOccurrenceForAnyTerm();
			UpdateAllRelativeTermWeight(maxO);
		}
		// calculating the total weight for each term
		{
			const char* const query = "UPDATE terms SET weight = weight_user * weight_rel_others;";
			sqlite3_exec(gDB, query, NULL, NULL, NULL);
		}
	}


	void BuildSEOTermReference(Clob& data)
	{
		char** result;
		int rowCount, colCount;
		CString<512,false> query;
		query << "SELECT w.id,w.term, t.article_id,t.count_in_page,t.weight * w.weight"
			<< " FROM terms_per_article AS t, terms as w"
			<< " WHERE w.id = t.term_id ORDER BY term_id";
		if (SQLITE_OK != sqlite3_get_table(gDB, query.c_str(), &result, &rowCount, &colCount, NULL))
			return;

		if (rowCount)
		{
			sint64 oldTermID = -1;
			unsigned int y = 5;
			for (unsigned int row = 0; row < (unsigned int) rowCount; ++row)
			{
				const sint64 termid = AnyString(result[y++]).to<sint64>();
				const AnyString term = result[y++];
				const AnyString articleID = result[y++];
				const AnyString scount   = result[y++];
				const AnyString sweight  = result[y++];
				if (termid < 0)
					continue;
				if (termid != oldTermID)
				{
					oldTermID = termid;
					if (row)
						data << "]);\n";
					data << "f(" << termid << ",'" << term << "',[";
				}
				else
					data << ',';

				data
					<< "{a:" << articleID // article ID
					<< ",c:" << scount    // count
					<< ",w:" << sweight   // weight
					<< '}';
			}
			data << "]);\n";
		}
		sqlite3_free_table(result);
	}



	void BuildSEOArticlesReference()
	{
		sqlite3_stmt* stmt;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, "SELECT id,html_href,title,weight FROM articles;", -1, &stmt, NULL))
			return;

		CString<1024 * 128> s("if (1) { var f=function(id,d) {SEO.articles[id] = d};");

		while (SQLITE_ROW == sqlite3_step(stmt))
		{
			const AnyString articleID = (const char*) sqlite3_column_text(stmt, 0);
			const AnyString href      = (const char*) sqlite3_column_text(stmt, 1);
			const AnyString title     = (const char*) sqlite3_column_text(stmt, 2);
			const AnyString sweight   = (const char*) sqlite3_column_text(stmt, 3);
			s
				<< "f(" << articleID << ",{"
				<< "t:\"" << title << '"'// article ID
				<< ",h:\"" << href << '"'// article ID
				<< ",w:" << sweight   // weight
				<< "});";
		}
		sqlite3_finalize(stmt);

		s << " }\n";

		// Writing the JS file
		String filename;
		filename << Program::htdocs << SEP << "seo" << SEP << "data.js";
		IO::File::AppendContent(filename, s);
	}


	void AppendChildrenList(Yuni::String& text, const String& path, const String& current)
	{
		if (!path)
			return;

		sqlite3_stmt* stmt;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, "SELECT title,html_href FROM articles WHERE parent = $1 ORDER BY parent_order,LOWER(title)", -1, &stmt, NULL))
			return;
		sqlite3_bind_text(stmt, 1, path.c_str(), path.size(), NULL);
		unsigned int count = 0;
		while (SQLITE_ROW == sqlite3_step(stmt))
		{
			if (!count++)
			{
				text << '\n';
				text << "\t<ul>\n";
			}
			const AnyString title = (const char*) sqlite3_column_text(stmt, 0);
			const AnyString href  = (const char*) sqlite3_column_text(stmt, 1);
			text << "\t<li><a ";
			if (title.equalsInsensitive(current))
				text << "class=\"itemselected\" ";
			text << "href=\"@{ROOT}" << href << "/@{INDEX}\">" << title << "</a></li>\n";
		}
		if (count)
			text << "\t</ul>";

		sqlite3_finalize(stmt);
		return;
	}


	void RetrieveTagList(ArticleData& article)
	{
		article.tags.clear();
		if (article.id < 0)
			return;

		sqlite3_stmt* stmt;
		if (SQLITE_OK != sqlite3_prepare_v2(gDB, "SELECT tagname FROM tags_per_article WHERE article_id = $1", -1, &stmt, NULL))
			return;
		sqlite3_bind_int64(stmt, 1, article.id);
		while (SQLITE_ROW == sqlite3_step(stmt))
		{
			const AnyString tagname = (const char*) sqlite3_column_text(stmt, 0);
			article.tags.insert(tagname);
		}
		sqlite3_finalize(stmt);
	}




} // namespace DocIndex

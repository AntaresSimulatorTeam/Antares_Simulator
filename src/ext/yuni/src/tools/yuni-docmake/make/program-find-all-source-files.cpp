
#include "program.h"
#include <yuni/io/directory/info.h>
#include "job.h"
#include "../logs.h"
#include "indexes.h"

#define SEP IO::Separator



namespace Yuni
{
namespace Tool
{
namespace DocMake
{


	void Program::findAllSourceFiles()
	{
		if (!Program::quiet)
			logs.notice() << "Looking for articles...";

		unsigned int upToDateCount = 0;
		std::vector<CompileJob*> jobs;
		jobs.resize(nbJobs);
		for (unsigned int i = 0; i != jobs.size(); ++i)
			jobs[i] = new CompileJob(input, htdocs);

		IO::Directory::Info info(input);
		String tmp;
		unsigned int slotIndex = 0;
		unsigned int count = 0;
		String relative;

		for (IO::Directory::Info::recursive_iterator i = info.recursive_begin(); i != info.recursive_end(); ++i)
		{
			// assert
			assert(!(!(*i)));
			// aliases
			const String& name = *i;
			const String& filename = i.filename();

			// The current element is either a folder, either a file
			if (i.isFolder())
			{
				// Detecting pseudo folders from source controls tools, such as
				// subversion or git
				if (filename.contains(".svn") || filename.contains(".git"))
					continue;

				// Ok, the folder interrests us
				// checking if th default article file is present
				tmp.clear() << filename << SEP << "article.xml";
				if (!IO::File::Exists(tmp))
				{
					logs.warning() << "missing article.xml in " << filename;
					String content;
					content << "<title></title>\n";
					content << "<pragma:directoryindex src=\".\" />\n";
					IO::File::SetContent(tmp, content);
				}
			}
			else
			{
				// This should be a file
				// In this phase, the only files which interrest us are all
				// named 'article.xml'
				if (!i.isFile() || name != "article.xml")
					continue;
				// A few asserts...
				assert(filename.size() > input.size());
				assert(slotIndex < jobs.size());

				// Checking for local modifications
				if (!Program::clean)
				{
					// The relative filename
					relative.assign(filename.c_str() + input.size() + 1, filename.size() - input.size() - 1);
					sint64 lastWriteFromCache = DocIndex::ArticleLastModificationTimeFromCache(relative);
					if (lastWriteFromCache > 0)
					{
						sint64 lastWrite = IO::File::LastModificationTime(filename);
						// Trying to check if we really have to perform an analyzis
						if (lastWriteFromCache == lastWrite)
						{
							++upToDateCount;
							continue;
						}
					}
				}

				// Creating a new job for generating this article
				jobs[slotIndex]->add(filename);
				if (++slotIndex >= jobs.size())
					slotIndex = 0;
				++count;
			}
		} // for, all files and folders, recursively


		// Statistics about the articles to generate
		if (count)
		{
			if (!Program::quiet)
			{
				logs.info() << count << (count > 1 ? " articles, " : " article, ") << nbJobs
					<< (nbJobs > 1 ? " threads" : " thread");
				logs.info();
			}
			for (unsigned int i = 0; i != jobs.size(); ++i)
				queueService += jobs[i];
		}
		else
		{
			// Actually there is nothing to do (??). Destroying all
			// useless jobs
			for (unsigned int i = 0; i != jobs.size(); ++i)
				delete jobs[i];
		}

		if (!Program::quiet)
		{
			switch (upToDateCount)
			{
				case 0:
					break;
				case 1:
					logs.info() << "1 article up-to-date";
					break;
				default:
					logs.info() << upToDateCount << " articles already up to date";
			}
		}
	}





} // namespace DocMake
} // namespace Tool
} // namespace Yuni



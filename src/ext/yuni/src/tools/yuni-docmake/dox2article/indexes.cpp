
#include "indexes.h"
#include <yuni/io/file.h>
#include "../logs.h"
#include "options.h"
#include "compound.h"


#define SEP IO::Separator



namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{


	void CreateIndex()
	{
		String filename;
		filename << Options::target << SEP << "article.xml";
		IO::File::Stream file;
		if (file.openRW(filename))
		{
			file
				<< "<title>Doxygen</title>\n"
				<< "<pragma:weight value=\"0.55\" />\n"
				<< "<pragma:directoryindex content=\"nofollow\" />"
				<< "\n\n"
				<< "<h2>API Reference</h2>\n"
				<< "<p><b>Beware: THIS DOCUMENTATION IS INCOMPLETE AND WE'RE CURRENTLY WORKING ON IT</b></p>\n"
				<< "<ul>\n"
				<< "<li><a href=\"class/@{INDEX}\">All classes</a></li>\n"
				<< "<li><a href=\"namespace/@{INDEX}\">All namespaces</a></li>\n"
				<< "</ul>\n"
				;
		}
	}




	void CreateNamespaceIndex()
	{
		String filename;
		filename << Options::target << SEP << "namespace";
		if (!IO::Directory::Create(filename))
			return;
		filename << SEP << "article.xml";
		IO::File::Stream file;
		if (file.openRW(filename))
		{
			file << "<title>Namespaces</title>\n\n";
			file << "<h2>All namespaces</h2>\n";
			file << "<p>This is a list of all namespaces in Yuni.</p>\n";
			file << "<table>\n";
			file << "<tr><th>Name</th><th>Description</th></tr>";
			unsigned int total = 0;

			Compound::Map::iterator end = allSymbolsByName[kdNamespace].end();
			for (Compound::Map::iterator i = allSymbolsByName[kdNamespace].begin(); i != end; ++i)
			{
				const Compound::Ptr compoundptr = i->second;
				const Compound& compound = *compoundptr;
				if (compound.name.find_first_of("<@") != String::npos)
					continue;

				file << "<tr><td>\n";
				file << "<a href=\"../" << compound.htdocs << "/@{INDEX}\">" << compound.name << "</a>\n";
				file << "</td><td></td></tr>\n";
				++total;
			}

			file << "</table>\n";
			switch (total)
			{
				case 0: file << "<p><i>(no namespace)</i></p>";break;
				case 1: file << "<p><i>(total: 1 namespace)</i></p>";break;
				default: file << "<p><i>(total: " << total << " namespaces)</i></p>";break;
			}
			file << "\n";
			
			logs.info() << "index: " << total << " namespaces";
		}
	}





	void CreateClassIndex()
	{
		String filename;
		filename << Options::target << SEP << "class";
		if (!IO::Directory::Create(filename))
			return;
		filename << SEP << "article.xml";
		IO::File::Stream file;
		if (file.openRW(filename))
		{
			file << "<title>Classes</title>\n\n";
			file << "<h2>All classes</h2>\n";
			file << "<p>This is a list of all classes in Yuni.</p>\n";
			file << "<table>\n";
			file << "<tr><th>Name</th><th>Description</th></tr>";
			unsigned int total = 0;

			Compound::Map::iterator end = allSymbolsByName[kdClass].end();
			for (Compound::Map::iterator i = allSymbolsByName[kdClass].begin(); i != end; ++i)
			{
				const Compound::Ptr compoundptr = i->second;
				const Compound& compound = *compoundptr;
				if (compound.name.find_first_of("<@") != String::npos)
					continue;

				file << "<tr><td>\n";
				file << "<a href=\"../" << compound.htdocs << "/@{INDEX}\">" << compound.name << "</a>\n";
				file << "</td><td></td></tr>\n";
				++total;
			}

			file << "</table>\n";
			switch (total)
			{
				case 0: file << "<p><i>(no class)</i></p>";break;
				case 1: file << "<p><i>(total: 1 class)</i></p>";break;
				default: file << "<p><i>(total: " << total << " classes)</i></p>";break;
			}
			file << "\n";

			logs.info() << "index: " << total << " classes";
		}
	}






} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni


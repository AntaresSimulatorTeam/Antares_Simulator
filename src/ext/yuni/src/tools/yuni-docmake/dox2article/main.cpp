
#include <yuni/yuni.h>
#include "../logs.h"

#include "compound.h"
#include "options.h"
#include "read-index.h"
#include "job-compound-explorer.h"
#include "job-compound-writer.h"
#include "indexes.h"




namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{



	class Program
	{
	public:
		Program()
		{
			allSymbols = new Compound::Map[kdMax];
			allSymbolsByName = new Compound::Map[kdMax];
		}

		~Program()
		{
			delete[] allSymbols;
			delete[] allSymbolsByName;
		}


		int operator () () const
		{
			logs.checkpoint() << "Edalene Doxygen to Article";

			if (!ReadXMLCatalogIndex(Options::doxygenXMLIndex))
				return EXIT_FAILURE;

			// Start the queue service
			Job::queueService.start();

			// Analyze all symbols
			Job::CompoundExplorer::Dispatch();

			// Writing articles
			Job::CompoundWriter::Dispatch();


			Job::queueService.stop();

			logs.info() << "writing index files";
			CreateNamespaceIndex();
			CreateClassIndex();
			CreateIndex();
			logs.info() << "done.";

			return 0;
		}

	}; // class Program




} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni




int main(int /*argc*/, char** /*argv*/)
{
	logs.applicationName("edln-dox2article");
	Yuni::Edalene::Dox2Article::Program program;
	return program();
}

